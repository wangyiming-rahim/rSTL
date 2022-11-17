#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <ranges>
#include <any>

#include "include/utility.h"
#include "include/tuple.h"
#include "include/shared_ptr.h"
#include "unique_ptr.h"


#define f(x)

int main(int argc, char *argv[])
{
	std::string* sp = new std::string("hello");
	rstl::unique_ptr<std::string> up1(sp);
//	rstl::unique_ptr<std::string> up2(sp);
	std::cout << "-=---" << std::endl;
	return 0;
}