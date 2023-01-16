#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"

int main(int argc, char** argv)
{

	doctest::Context ctx;
	ctx.applyCommandLine(argc, argv);
	ctx.setOption("no-breaks", true);
	return ctx.run();
}


