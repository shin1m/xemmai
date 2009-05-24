#include <xemmai/engine.h>

int main(int argc, char* argv[])
{
	xemmai::t_engine engine(argc, argv);
	if (argc < 2) {
		std::fwprintf(stderr, L"usage: %hs [options] <script> ...\n", argv[0]);
		return -1;
	}
	return engine.f_run();
}
