#include <xemmai/engine.h>

#include <clocale>
#include <cstring>

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	bool verbose = false;
	{
		char** end = argv + argc;
		char** q = argv;
		for (char** p = argv; p < end; ++p) {
			if ((*p)[0] == '-' && (*p)[1] == '-') {
				const char* v = *p + 2;
				if (std::strcmp(v, "verbose") == 0) verbose = true;
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
	xemmai::t_engine engine(1 << 10, verbose, argc, argv);
	return static_cast<int>(engine.f_run());
}
