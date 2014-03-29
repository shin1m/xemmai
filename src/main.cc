#include <xemmai/engine.h>

#include <clocale>
#include <cstring>
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

	std::thread v_thread;
	std::mutex v_mutex;
	std::condition_variable v_posted;
	t_object* v_stopped = nullptr;
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

	void f_print(t_engine* a_engine, t_fiber::t_context* a_context)
	{
		a_engine->f_context_print(a_context->v_lambda, a_context->f_pc());
	}
	void f_print(t_engine* a_engine, t_object* a_thread)
	{
		auto context = f_as<t_fiber&>(f_as<t_thread&>(a_thread).v_active).v_context;
		if (context) f_print(a_engine, context);
	}
	void f_print_stack(t_engine* a_engine, t_object* a_thread)
	{
		for (auto context = f_as<t_fiber&>(f_as<t_thread&>(a_thread).v_active).v_context; context; context = context->f_next()) {
			if (context->f_native() > 0) std::fputs("<native code>\n", stderr);
			f_print(a_engine, context);
		}
	}
	void f_stopped(t_engine* a_engine, t_object* a_thread)
	{
		std::vector<t_object*> threads;
		a_engine->f_thread_list([&threads](t_object* a_thread)
		{
			threads.push_back(a_thread);
		});
		std::fprintf(stderr, "debugger stopped: %p\n", a_thread);
		f_print(a_engine, a_thread);
		while (true) {
			std::fprintf(stderr, "debugger> ");
			int c = std::getchar();
			switch (c) {
			case EOF:
				a_engine->f_debug_continue();
				return;
			case 'c':
				c = std::getchar();
				if (c == '\n') {
					a_engine->f_debug_continue();
					return;
				}
				break;
			case 'p':
				c = std::getchar();
				switch (c) {
				case 's':
					c = std::getchar();
					if (c == '\n') f_print_stack(a_engine, a_thread);
					break;
				}
				break;
			case 's':
				c = std::getchar();
				if (c == '\n') {
					a_engine->f_debug_continue(a_thread);
					return;
				}
				break;
			case 't':
				c = std::getchar();
				switch (c) {
				case 'l':
					c = std::getchar();
					if (c == '\n') {
						size_t i = 0;
						for (auto p : threads) {
							std::fprintf(stderr, "[%d]: %p\n", i++, p);
							f_print(a_engine, p);
						}
					}
					break;
				default:
					if (std::isdigit(c)) {
						size_t i = 0;
						do {
							i += c - '0';
							c = std::getchar();
						} while (std::isdigit(c));
						if (c == '\n' && i < threads.size()) {
							a_thread = threads[i];
							std::fprintf(stderr, "[%d]: %p\n", i, a_thread);
							f_print(a_engine, a_thread);
						}
					}
				}
				break;
			}
			while (c != EOF && c != '\n') c = std::getchar();
		}
	}
	void f_run(t_engine* a_engine)
	{
		while (true) {
			t_object* stopped = nullptr;
			bool interrupted = false;
			{
				std::unique_lock<std::mutex> lock(v_mutex);
				while (true) {
					if (!v_instance) return;
					std::swap(stopped, v_stopped);
					std::swap(interrupted, v_interrupted);
					if (stopped || interrupted) break;
					v_posted.wait(lock);
				}
			}
			if (stopped)
				f_stopped(a_engine, stopped);
			else
				a_engine->f_debug_stop();
		}
	}
	void f_interrupted()
	{
		std::lock_guard<std::mutex> lock(v_mutex);
		v_interrupted = true;
		v_posted.notify_one();
	}

public:
	t_debugger(t_engine& a_engine)
	{
		v_instance = this;
		v_thread = std::thread(&t_debugger::f_run, this, &a_engine);
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
