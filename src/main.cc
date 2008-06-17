#include <xemmai/portable/convert.h>
#include <xemmai/engine.h>
#include <xemmai/module.h>

int main(int argc, char* argv[])
{
	using namespace xemmai;
	t_engine engine(argc, argv);
	if (argc < 2) {
		std::fwprintf(stderr, L"usage: %hs [options] <script> ...\n", argv[0]);
		return -1;
	}
	return t_module::f_execute(portable::f_convert(argv[1]));
}
