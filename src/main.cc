#include <xemmai/portable/path.h>
#include <xemmai/global.h>
#include <thread>
#include <clocale>
#include <cstring>
#ifdef __unix__
#include <signal.h>
#endif
#ifdef _WIN32
#include <io.h>
#endif

namespace
{

using namespace xemmai;

class t_debugger : public xemmai::t_debugger
{
	static inline t_debugger* v_instance;

#ifdef _WIN32
	static BOOL WINAPI f_interrupted(DWORD a_type)
	{
		if (a_type != CTRL_C_EVENT) return FALSE;
		v_instance->f_interrupted();
		return TRUE;
	}
#endif

	t_engine& v_engine;
	std::FILE* v_out;
	std::thread v_thread;
	std::mutex v_mutex;
	std::condition_variable v_posted;
	t_thread* v_stopped = nullptr;
	t_debug_script* v_loaded = nullptr;
	std::map<std::wstring, std::set<size_t>, std::less<>> v_break_points;
	bool v_interrupted = false;
#ifdef __unix__
	std::thread v_interrupter;

	void f_interrupter()
	{
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		while (true) {
			int no;
			sigwait(&set, &no);
			if (!v_instance) break;
			f_interrupted();
		}
	}
#endif

	size_t f_read_integer(wint_t& a_c)
	{
		size_t i = 0;
		while (std::iswdigit(a_c)) {
			i = i * 10 + (a_c - L'0');
			a_c = std::getwchar();
		}
		return i;
	}
	std::wstring f_read_string(wint_t& a_c, wchar_t a_terminator)
	{
		std::wstring s;
		while (a_c != WEOF && a_c != a_terminator) {
			s.push_back(a_c);
			a_c = std::getwchar();
		}
		return s;
	}
	std::pair<std::wstring, size_t> f_read_path(wint_t& a_c)
	{
		std::wstring path = f_read_string(a_c, L':');
		size_t i = 0;
		if (a_c == L':') {
			a_c = std::getwchar();
			i = f_read_integer(a_c);
		}
		return {portable::t_path(path), i};
	}
	void f_print_break_points()
	{
		for (auto& pair : v_break_points) {
			std::fprintf(v_out, "%ls\n", pair.first.c_str());
			for (auto line : pair.second) std::fprintf(v_out, "\t%zu\n", line);
		}
	}
	t_debug_script* f_find_module(std::wstring_view a_path)
	{
		for (auto& pair : v_engine.f_modules()) {
			auto& body = pair.second->f_as<t_module>().v_body;
			if (!body) continue;
			auto debug = dynamic_cast<t_debug_script*>(&body->f_as<t_module::t_body>());
			if (debug && debug->v_path == a_path) return debug;
		}
		return nullptr;
	}
	void f_set_break_point(std::wstring_view a_path, size_t a_line)
	{
		auto debug = f_find_module(a_path);
		if (debug) {
			a_line = debug->f_set_break_point(a_line).first;
			if (a_line <= 0) return;
		}
		auto i = v_break_points.lower_bound(a_path);
		if (i == v_break_points.end() || i->first != a_path) i = v_break_points.emplace_hint(i, a_path, std::set<size_t>());
		i->second.insert(a_line);
	}
	void f_set_break_points(t_debug_script* a_debug)
	{
		auto i = v_break_points.find(a_debug->v_path);
		if (i != v_break_points.end()) {
			std::set<size_t> lines;
			for (auto line : i->second) {
				line = a_debug->f_set_break_point(line).first;
				if (line > 0) lines.insert(line);
			}
			if (lines.empty())
				v_break_points.erase(i);
			else
				i->second = std::move(lines);
		}
		v_engine.f_debug_continue(v_engine.f_debug_stepping());
	}
	void f_reset_break_point(std::wstring_view a_path, size_t a_line)
	{
		auto i = v_break_points.lower_bound(a_path);
		if (i == v_break_points.end() || i->first != a_path) return;
		i->second.erase(a_line);
		auto debug = f_find_module(a_path);
		if (debug) {
			a_line = debug->f_reset_break_point(a_line, 1).first;
			if (a_line > 0) i->second.erase(a_line);
		}
		if (i->second.empty()) v_break_points.erase(i);
	}
	t_debug_context* f_context(t_thread* a_thread)
	{
		return a_thread->v_internal->v_active->v_fiber->v_context;
	}
	void f_print(t_context* a_context)
	{
		v_engine.f_context_print(v_out, f_as<t_lambda*>(a_context->v_lambda), a_context->v_pc);
	}
	void f_print_contexts(t_debug_context* a_context, t_context* a_current)
	{
		for (; a_context; a_context = a_context->v_next) {
			if (a_context == a_current) std::fputs("* ", v_out);
			f_print(a_context);
		}
	}
	void f_print_thread(size_t a_i, t_thread* a_thread)
	{
		std::fprintf(v_out, "[%zu]: %p\n", a_i, a_thread);
		auto context = f_context(a_thread);
		if (context) f_print(context);
	}
	void f_print_threads(const std::vector<t_thread*>& a_threads)
	{
		size_t i = 0;
		for (auto p : a_threads) f_print_thread(i++, p);
	}
	template<typename T>
	bool f_is(t_object* a_object)
	{
		return a_object->f_type()->f_derives<T>();
	}
	template<typename T>
	void f_print_sequence(t_object* a_value, size_t a_depth)
	{
		auto& sequence = a_value->f_as<T>();
		if (a_depth <= 0) {
			std::fputs("...", v_out);
		} else if (sequence.f_size() > 0) {
			f_print_value(sequence[0], a_depth);
			for (size_t i = 1; i < sequence.f_size(); ++i) {
				std::fputs(", ", v_out);
				f_print_value(sequence[i], a_depth);
			}
		}
	}
	void f_print_value(const t_pvalue& a_value, size_t a_depth)
	{
		switch (a_value.f_tag()) {
		case c_tag__NULL:
			std::fputs("null", v_out);
			break;
		case c_tag__FALSE:
			std::fputs("false", v_out);
			break;
		case c_tag__TRUE:
			std::fputs("true", v_out);
			break;
		case c_tag__INTEGER:
			std::fprintf(v_out, "%" PRIdPTR, f_as<intptr_t>(a_value));
			break;
		case c_tag__FLOAT:
			std::fprintf(v_out, "%g", f_as<double>(a_value));
			break;
		default:
			std::fprintf(v_out, "@%p", static_cast<t_object*>(a_value));
			if (--a_depth > 0) {
				std::fputc('(', v_out);
				auto type = a_value->f_type();
				auto n = type->v_instance_fields;
				if (n > 0) {
					size_t i = 0;
					while (true) {
						std::fprintf(v_out, "%ls: ", type->f_fields()[i].first->f_as<t_symbol>().f_string().c_str());
						f_print_value(a_value->f_fields()[i], a_depth);
						if (++i >= n) break;
						std::fputs(", ", v_out);
					}
				}
				std::fputc(')', v_out);
			}
			if (f_is<t_string>(a_value)) {
				std::fprintf(v_out, " \"%ls\"", static_cast<const wchar_t*>(a_value->f_as<t_string>()));
			} else if (f_is<t_tuple>(a_value)) {
				std::fputs(" '(", v_out);
				f_print_sequence<t_tuple>(a_value, a_depth);
				std::fputc(')', v_out);
			} else if (f_is<t_list>(a_value)) {
				std::fputs(" [", v_out);
				f_print_sequence<t_list>(a_value, a_depth);
				std::fputc(']', v_out);
			} else if (f_is<t_map>(a_value)) {
				auto& map = a_value->f_as<t_map>();
				std::fputs(" {", v_out);
				if (a_depth <= 0) {
					std::fputs("...", v_out);
				} else {
					t_map::t_iterator i(map);
					if (i.f_entry()) {
						while (true) {
							f_print_value(i.f_entry()->f_key(), a_depth);
							std::fputs(": ", v_out);
							f_print_value(i.f_entry()->v_value, a_depth);
							i.f_next();
							if (!i.f_entry()) break;
							std::fputs(", ", v_out);
						}
					}
				}
				std::fputc('}', v_out);
			}
		}
	}
	void f_print_variable(t_context* a_context, std::wstring_view a_name, size_t a_depth)
	{
		auto variable = a_context->f_variable(a_name);
		if (!variable) return;
		f_print_value(*variable, a_depth);
		std::fputc('\n', v_out);
	}
	void f_print_variables(t_context* a_context)
	{
		auto& code = a_context->v_lambda->f_as<t_lambda>().f_code()->f_as<t_code>();
		for (auto& pair : code.v_variables) std::fprintf(v_out, "%ls\n", pair.first.c_str());
	}
	void f_prompt(t_thread* a_thread)
	{
		std::vector<t_thread*> threads;
		v_engine.f_threads([&threads](auto a_thread)
		{
			threads.push_back(a_thread);
		});
		std::fprintf(v_out, "debugger stopped: %p\n", a_thread);
		auto context = f_context(a_thread);
		if (context) f_print(context);
		while (true) {
			std::fprintf(v_out, "debugger> ");
			wint_t c = std::getwchar();
			switch (c) {
			case WEOF:
				v_engine.f_debug_continue();
				return;
			case L'b':
				c = std::getwchar();
				switch (c) {
				case L'l':
					c = std::getwchar();
					if (c == L'\n') f_print_break_points();
					break;
				case L'r':
					{
						c = std::getwchar();
						auto path = f_read_path(c);
						if (c == L'\n' && path.second > 0) f_reset_break_point(path.first, path.second);
					}
					break;
				case L's':
					{
						c = std::getwchar();
						auto path = f_read_path(c);
						if (c == L'\n' && path.second > 0) f_set_break_point(path.first, path.second);
					}
					break;
				}
				break;
			case L'c':
				c = std::getwchar();
				if (c == L'\n') {
					v_engine.f_debug_continue();
					return;
				}
				break;
			case L'd':
				c = std::getwchar();
				if (c == L'\n') {
					auto p = f_context(a_thread);
					if (p != context) {
						while (p->v_next != context) p = p->v_next;
						context = p;
					}
					if (context) f_print(context);
				}
				break;
			case L'l':
				c = std::getwchar();
				if (c == L'\n' && context) f_print_variables(context);
				break;
			case L'p':
				{
					c = std::getwchar();
					size_t depth = 2;
					if (std::iswdigit(c)) depth = f_read_integer(c);
					std::wstring name = f_read_string(c, L'\n');
					if (c == L'\n' && context) f_print_variable(context, name, depth);
				}
				break;
			case L's':
				c = std::getwchar();
				if (c == L'\n') {
					v_engine.f_debug_continue(a_thread);
					return;
				}
				break;
			case L't':
				c = std::getwchar();
				switch (c) {
				case L'c':
					c = std::getwchar();
					if (c == L'\n') f_print_contexts(f_context(a_thread), context);
					break;
				case L'l':
					c = std::getwchar();
					if (c == L'\n') f_print_threads(threads);
					break;
				default:
					if (std::iswdigit(c)) {
						size_t i = f_read_integer(c);
						if (c == L'\n' && i < threads.size()) {
							a_thread = threads[i];
							context = f_context(a_thread);
							f_print_thread(i, a_thread);
						}
					}
				}
				break;
			case L'u':
				c = std::getwchar();
				if (c == L'\n') {
					if (context) context = context->v_next;
					if (context) f_print(context);
				}
				break;
			}
			while (c != WEOF && c != L'\n') c = std::getwchar();
		}
	}
	void f_run()
	{
		while (true) {
			t_thread* stopped = nullptr;
			t_debug_script* loaded = nullptr;
			bool interrupted = false;
			{
				std::unique_lock lock(v_mutex);
				while (true) {
					if (!v_instance) return;
					std::swap(stopped, v_stopped);
					std::swap(loaded, v_loaded);
					std::swap(interrupted, v_interrupted);
					if (stopped || loaded || interrupted) break;
					v_posted.wait(lock);
				}
			}
			if (stopped)
				f_prompt(stopped);
			else if (loaded)
				f_set_break_points(loaded);
			else
				v_engine.f_debug_stop();
		}
	}
	void f_interrupted()
	{
		std::lock_guard lock(v_mutex);
		v_interrupted = true;
		v_posted.notify_one();
	}

public:
	t_debugger(t_engine& a_engine, std::FILE* a_out) : v_engine(a_engine), v_out(a_out)
	{
		v_instance = this;
		v_thread = std::thread(&t_debugger::f_run, this);
#ifdef __unix__
		v_interrupter = std::thread(&t_debugger::f_interrupter, this);
#endif
#ifdef _WIN32
		SetConsoleCtrlHandler(f_interrupted, TRUE);
#endif
	}
	~t_debugger()
	{
		{
			std::lock_guard lock(v_mutex);
			v_instance = nullptr;
			v_posted.notify_one();
		}
#ifdef __unix__
		pthread_kill(v_interrupter.native_handle(), SIGINT);
		v_interrupter.join();
#endif
#ifdef _WIN32
		SetConsoleCtrlHandler(f_interrupted, FALSE);
#endif
		v_thread.join();
	}
	virtual void f_stopped(t_thread* a_thread)
	{
		std::lock_guard lock(v_mutex);
		v_stopped = a_thread;
		v_posted.notify_one();
	}
	virtual void f_loaded(t_debug_script& a_debug)
	{
		std::lock_guard lock(v_mutex);
		v_loaded = &a_debug;
		v_posted.notify_one();
	}
};

}

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	t_engine::t_options options;
#ifdef _WIN32
	const char* debug = nullptr;
#else
	int debug = -1;
#endif
	{
		char** end = argv + argc;
		char** q = argv;
		for (char** p = argv; p < end; ++p) {
			if ((*p)[0] == '-' && (*p)[1] == '-') {
				const char* v = *p + 2;
				if (std::strcmp(v, "verbose") == 0) {
					options.v_verbose = true;
				} else if (std::strncmp(v, "collector-threshold=", 20) == 0) {
					std::sscanf(v + 20, "%zu", &options.v_collector__threshold);
				} else if (std::strncmp(v, "debug", 5) == 0) {
#ifdef _WIN32
					debug = v[5] == '=' ? v + 6 : v + 5;
#else
					debug = 2;
					if (v[5] == '=') std::sscanf(v + 6, "%u", &debug);
#endif
				}
			} else {
				*q++ = *p;
			}
		}
		argc = q - argv;
	}
	if (argc < 2) {
		std::fprintf(stderr, "usage: %s [options] <script> ...\n", argv[0]);
		return -1;
	}
#ifdef __unix__
	if (debug >= 0) {
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		pthread_sigmask(SIG_BLOCK, &set, NULL);
	}
#endif
	xemmai::t_engine engine(options, argc, argv);
#ifdef _WIN32
	if (!debug) return static_cast<int>(engine.f_run(nullptr));
	auto fd = debug[0] ? creat(debug, S_IREAD | S_IWRITE) : dup(2);
	if (fd == -1) portable::f_throw_system_error();
	auto out = fdopen(fd, "w");
#else
	if (debug < 0) return static_cast<int>(engine.f_run(nullptr));
	auto out = fdopen(debug == 2 ? dup(2) : debug, "w");
#endif
	if (!out) portable::f_throw_system_error();
	std::unique_ptr<std::FILE, int(*)(std::FILE*)> close(out, std::fclose);
	std::setbuf(out, NULL);
	::t_debugger debugger(engine, out);
	return static_cast<int>(engine.f_run(&debugger));
}
