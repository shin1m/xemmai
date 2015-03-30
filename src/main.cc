#include <xemmai/engine.h>

#include <xemmai/portable/path.h>
#include <xemmai/array.h>
#include <xemmai/global.h>
#include <xemmai/io/file.h>

#include <clocale>
#include <cstring>
#include <set>
#ifdef __unix__
#include <unistd.h>
#include <signal.h>
#endif

namespace
{

using namespace xemmai;

class t_debugger : public xemmai::t_debugger
{
	static t_debugger* v_instance;

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
	t_object* v_stopped = nullptr;
	t_object* v_loaded = nullptr;
	std::map<std::wstring, std::set<size_t>> v_break_points;
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
		std::vector<wchar_t> cs;
		while (a_c != WEOF && a_c != a_terminator) {
			cs.push_back(a_c);
			a_c = std::getwchar();
		}
		return std::wstring(cs.begin(), cs.end());
	}
	std::pair<std::wstring, size_t> f_read_path(wint_t& a_c)
	{
		std::wstring path = f_read_string(a_c, L':');
		size_t i = 0;
		if (a_c == L':') {
			a_c = std::getwchar();
			i = f_read_integer(a_c);
		}
		return std::make_pair(portable::t_path(path), i);
	}
	void f_print_break_points()
	{
		for (auto& pair : v_break_points) {
			std::fprintf(v_out, "%ls\n", pair.first.c_str());
			for (auto line : pair.second) std::fprintf(v_out, "\t%" PRIuPTR "\n", static_cast<uintptr_t>(line));
		}
	}
	t_debug_module* f_find_module(const std::wstring& a_path)
	{
		for (auto& pair : v_engine.f_modules()) {
			if (!pair.second) continue;
			auto debug = dynamic_cast<t_debug_module*>(&f_as<t_module&>(pair.second));
			if (debug && debug->v_path == a_path) return debug;
		}
		return nullptr;
	}
	void f_set_break_point(const std::wstring& a_path, size_t a_line)
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
	void f_set_break_points()
	{
		for (auto& pair : v_engine.f_modules()) {
			if (!pair.second) continue;
			auto debug = dynamic_cast<t_debug_module*>(&f_as<t_module&>(pair.second));
			if (!debug) continue;
			auto i = v_break_points.find(debug->v_path);
			if (i == v_break_points.end()) continue;
			std::set<size_t> lines;
			for (auto line : i->second) {
				line = debug->f_set_break_point(line).first;
				if (line > 0) lines.insert(line);
			}
			if (lines.empty())
				v_break_points.erase(i);
			else
				i->second = std::move(lines);
		}
		v_engine.f_debug_continue(v_engine.f_debug_stepping());
	}
	void f_reset_break_point(const std::wstring& a_path, size_t a_line)
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
	t_fiber::t_context* f_context(t_object* a_thread)
	{
		return f_as<t_fiber&>(f_as<t_thread&>(a_thread).v_active).v_context;
	}
	void f_print(t_fiber::t_context* a_context)
	{
		v_engine.f_context_print(v_out, a_context->v_lambda, a_context->f_pc());
	}
	void f_print_contexts(t_fiber::t_context* a_context, t_fiber::t_context* a_current)
	{
		for (; a_context; a_context = a_context->f_next()) {
			if (a_context->f_native() > 0) std::fputs("<native code>\n", v_out);
			if (a_context == a_current) std::fputs("* ", v_out);
			f_print(a_context);
		}
	}
	void f_print_thread(size_t a_i, t_object* a_thread)
	{
		std::fprintf(v_out, "[%" PRIuPTR "]: %p\n", static_cast<uintptr_t>(a_i), a_thread);
		auto context = f_context(a_thread);
		if (context) f_print(context);
	}
	void f_print_threads(const std::vector<t_object*>& a_threads)
	{
		size_t i = 0;
		for (auto p : a_threads) f_print_thread(i++, p);
	}
	template<typename T>
	bool f_is(t_object* a_object)
	{
		return dynamic_cast<t_type_of<T>*>(a_object->f_type_as_type()) != nullptr;
	}
	template<typename T>
	void f_print_sequence(t_object* a_value, size_t a_depth)
	{
		auto& sequence = f_as<const T&>(a_value);
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
	void f_print_value(const t_value& a_value, size_t a_depth)
	{
		switch (a_value.f_tag()) {
		case t_value::e_tag__NULL:
			std::fputs("null", v_out);
			break;
		case t_value::e_tag__BOOLEAN:
			std::fputs(f_as<bool>(a_value) ? "true" : "false", v_out);
			break;
		case t_value::e_tag__INTEGER:
			std::fprintf(v_out, "%" PRIdPTR, f_as<intptr_t>(a_value));
			break;
		case t_value::e_tag__FLOAT:
			std::fprintf(v_out, "%g", f_as<double>(a_value));
			break;
		default:
			std::fprintf(v_out, "@%p", static_cast<t_object*>(a_value));
			if (--a_depth > 0) {
				std::fputc('(', v_out);
				auto p = static_cast<t_object*>(a_value);
				size_t n = p->f_field_size();
				if (n > 0) {
					size_t i = 0;
					while (true) {
						std::fprintf(v_out, "%ls: ", f_as<t_symbol&>(p->f_field_key(i)).f_string().c_str());
						f_print_value(p->f_field_get(i), a_depth);
						if (++i >= n) break;
						std::fputs(", ", v_out);
					}
				}
				std::fputc(')', v_out);
			}
			if (f_is<std::wstring>(a_value)) {
				std::fprintf(v_out, " \"%ls\"", f_as<std::wstring&>(a_value).c_str());
			} else if (f_is<t_tuple>(a_value)) {
				std::fputs(" '(", v_out);
				f_print_sequence<t_tuple>(a_value, a_depth);
				std::fputc(')', v_out);
			} else if (f_is<t_array>(a_value)) {
				std::fputs(" [", v_out);
				f_print_sequence<t_array>(a_value, a_depth);
				std::fputc(']', v_out);
			} else if (f_is<t_dictionary>(a_value)) {
				t_dictionary& dictionary = f_as<t_dictionary&>(a_value);
				std::fputs(" {", v_out);
				if (a_depth <= 0) {
					std::fputs("...", v_out);
				} else {
					t_dictionary::t_iterator i(dictionary);
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
	void f_print_variable(t_fiber::t_context* a_context, const std::wstring& a_name, size_t a_depth)
	{
		const t_value* variable = a_context->f_variable(a_name);
		if (!variable) return;
		f_print_value(*variable, a_depth);
		std::fputc('\n', v_out);
	}
	void f_print_variables(t_fiber::t_context* a_context)
	{
		t_code& code = f_as<t_code&>(f_as<t_lambda&>(a_context->v_lambda).f_code());
		for (auto& pair : code.v_variables) std::fprintf(v_out, "%ls\n", pair.first.c_str());
	}
	void f_prompt(t_object* a_thread)
	{
		std::vector<t_object*> threads;
		v_engine.f_threads([&threads](t_object* a_thread)
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
						while (p->f_next() != context) p = p->f_next();
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
					if (context) context = context->f_next();
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
			t_object* stopped = nullptr;
			t_object* loaded = nullptr;
			bool interrupted = false;
			{
				std::unique_lock<std::mutex> lock(v_mutex);
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
				f_set_break_points();
			else
				v_engine.f_debug_stop();
		}
	}
	void f_interrupted()
	{
		std::lock_guard<std::mutex> lock(v_mutex);
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
			std::lock_guard<std::mutex> lock(v_mutex);
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
	virtual void f_stopped(t_object* a_thread)
	{
		std::lock_guard<std::mutex> lock(v_mutex);
		v_stopped = a_thread;
		v_posted.notify_one();
	}
	virtual void f_loaded(t_object* a_thread)
	{
		std::lock_guard<std::mutex> lock(v_mutex);
		v_loaded = a_thread;
		v_posted.notify_one();
	}
};

t_debugger* t_debugger::v_instance;

}

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	bool verbose = false;
	int debug = -1;
	{
		char** end = argv + argc;
		char** q = argv;
		for (char** p = argv; p < end; ++p) {
			if ((*p)[0] == '-' && (*p)[1] == '-') {
				const char* v = *p + 2;
				if (std::strcmp(v, "verbose") == 0) {
					verbose = true;
				} else if (std::strncmp(v, "debug", 5) == 0) {
					debug = 2;
					if (v[5] == '=') std::sscanf(v + 6, "%u", &debug);
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
	if (debug >= 0) {
#ifdef __unix__
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif
	}
	xemmai::t_engine engine(1 << 10, verbose, argc, argv);
	if (debug < 0) return static_cast<int>(engine.f_run(nullptr));
	io::t_file out(debug == 2 ? dup(2) : debug, "w");
	std::setbuf(out, NULL);
	::t_debugger debugger(engine, out);
	return static_cast<int>(engine.f_run(&debugger));
}
