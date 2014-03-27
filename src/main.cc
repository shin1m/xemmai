#include <xemmai/engine.h>

#include <clocale>
#include <cstring>
#ifdef __unix__
#include <unistd.h>
#include <signal.h>

namespace
{

using namespace xemmai;

class t_debugger : public xemmai::t_debugger
{
	enum t_tag
	{
		e_tag__NONE,
		e_tag__STOPPED,
		e_tag__INTERRUPTED
	};

	int v_fds[2];
	std::thread v_thread;
	bool v_done = false;
	std::thread v_waiter;

	template<typename T>
	T f_read()
	{
		T value;
		size_t m = 0;
		do m += read(v_fds[0], reinterpret_cast<char*>(&value) + m, sizeof(T) - m); while (m < sizeof(T));
		return value;
	}
	template<typename T>
	void f_post(T a_value)
	{
		write(v_fds[1], &a_value, sizeof(T));
	}
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
			switch (f_read<t_tag>()) {
			case e_tag__STOPPED:
				f_stopped(a_engine, f_read<t_object*>());
				break;
			case e_tag__INTERRUPTED:
				a_engine->f_debug_stop();
				break;
			default:
				return;
			}
		}
	}
	void f_wait()
	{
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		while (true) {
			int no;
			sigwait(&set, &no);
			if (v_done) break;
			f_post(e_tag__INTERRUPTED);
		}
	}

public:
	t_debugger(t_engine& a_engine)
	{
		pipe(v_fds);
		v_thread = std::thread(&t_debugger::f_run, this, &a_engine);
		v_waiter = std::thread(&t_debugger::f_wait, this);
	}
	~t_debugger()
	{
		v_done = true;
		pthread_kill(v_waiter.native_handle(), SIGINT);
		v_waiter.join();
		f_post(e_tag__NONE);
		v_thread.join();
		close(v_fds[0]);
		close(v_fds[1]);
	}
	virtual void f_stopped(t_object* a_thread)
	{
		f_post(e_tag__STOPPED);
		f_post(a_thread);
	}
};

}
#endif

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
#ifdef __unix__
	if (debug) {
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		pthread_sigmask(SIG_BLOCK, &set, NULL);
	}
#endif
	xemmai::t_engine engine(1 << 10, verbose, argc, argv);
#ifdef __unix__
	if (!debug) return static_cast<int>(engine.f_run(nullptr));
	::t_debugger debugger(engine);
	return static_cast<int>(engine.f_run(&debugger));
#else
	return static_cast<int>(engine.f_run(nullptr));
#endif
}
