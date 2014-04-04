#include <xemmai/engine.h>

#include <xemmai/portable/path.h>

#include <clocale>
#include <cstring>
#include <cwctype>
#include <set>
#ifdef __unix__
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
	std::pair<std::wstring, size_t> f_read_path(wint_t& a_c)
	{
		std::vector<wchar_t> path;
		while (a_c != WEOF && a_c != L'\n' && a_c != L':') {
			path.push_back(a_c);
			a_c = std::getwchar();
		}
		size_t i = 0;
		if (a_c == L':') {
			a_c = std::getwchar();
			i = f_read_integer(a_c);
		}
		return std::make_pair(portable::t_path(std::wstring(path.begin(), path.end())), i);
	}
	void f_print_break_points()
	{
		for (auto& pair : v_break_points) {
			std::fprintf(stderr, "%ls\n", pair.first.c_str());
			for (auto line : pair.second) std::fprintf(stderr, "\t%d\n", line);
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
			a_line = debug->f_set_break_point(a_line, 1).first;
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
				line = debug->f_set_break_point(line, 1).first;
				if (line > 0) lines.insert(line);
			}
			if (lines.empty())
				v_break_points.erase(i);
			else
				i->second = std::move(lines);
		}
		v_engine.f_debug_continue();
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
	void f_print(t_fiber::t_context* a_context)
	{
		v_engine.f_context_print(a_context->v_lambda, a_context->f_pc());
	}
	void f_print(t_object* a_thread)
	{
		auto context = f_as<t_fiber&>(f_as<t_thread&>(a_thread).v_active).v_context;
		if (context) f_print(context);
	}
	void f_print_stack(t_object* a_thread)
	{
		for (auto context = f_as<t_fiber&>(f_as<t_thread&>(a_thread).v_active).v_context; context; context = context->f_next()) {
			if (context->f_native() > 0) std::fputs("<native code>\n", stderr);
			f_print(context);
		}
	}
	void f_print_thread(size_t a_i, t_object* a_thread)
	{
		std::fprintf(stderr, "[%d]: %p\n", a_i, a_thread);
		f_print(a_thread);
	}
	void f_print_threads(const std::vector<t_object*>& a_threads)
	{
		size_t i = 0;
		for (auto p : a_threads) f_print_thread(i++, p);
	}
	void f_prompt(t_object* a_thread)
	{
		std::vector<t_object*> threads;
		v_engine.f_threads([&threads](t_object* a_thread)
		{
			threads.push_back(a_thread);
		});
		std::fprintf(stderr, "debugger stopped: %p\n", a_thread);
		f_print(a_thread);
		while (true) {
			std::fprintf(stderr, "debugger> ");
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
			case L'p':
				c = std::getwchar();
				switch (c) {
				case L's':
					c = std::getwchar();
					if (c == L'\n') f_print_stack(a_thread);
					break;
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
				case L'l':
					c = std::getwchar();
					if (c == L'\n') f_print_threads(threads);
					break;
				default:
					if (std::iswdigit(c)) {
						size_t i = f_read_integer(c);
						if (c == L'\n' && i < threads.size()) {
							a_thread = threads[i];
							f_print_thread(i, a_thread);
						}
					}
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
	t_debugger(t_engine& a_engine) : v_engine(a_engine)
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
	bool debug = false;
	{
		char** end = argv + argc;
		char** q = argv;
		for (char** p = argv; p < end; ++p) {
			if ((*p)[0] == '-' && (*p)[1] == '-') {
				const char* v = *p + 2;
				if (std::strcmp(v, "verbose") == 0)
					verbose = true;
				else if (std::strcmp(v, "debug") == 0)
					debug = true;
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
	if (debug) {
#ifdef __unix__
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif
	}
	xemmai::t_engine engine(1 << 10, verbose, argc, argv);
	if (!debug) return static_cast<int>(engine.f_run(nullptr));
	::t_debugger debugger(engine);
	return static_cast<int>(engine.f_run(&debugger));
}
