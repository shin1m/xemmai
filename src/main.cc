#include <xemmai/engine.h>

#include <cstring>

int main(int argc, char* argv[])
{
	bool verbose = false;
	{
		char** end = argv + argc;
		char** q = argv;
		for (char** p = argv; p < end; ++p) {
			if ((*p)[0] == '-' && (*p)[1] == '-') {
				const char* v = *p + 2;
				if (std::strcmp(v, "default-module-path") == 0) {
					std::puts(xemmai::portable::f_convert(XEMMAI__MACRO__L(XEMMAI_MODULE_PATH)).c_str());
					return 0;
				} else if (std::strcmp(v, "verbose") == 0) {
					verbose = true;
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
	xemmai::t_engine engine(verbose, argc, argv);
	return engine.f_run();
}
