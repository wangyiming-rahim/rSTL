#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <ranges>
#include <any>

#include "include/utility.h"
#include "include/tuple.h"
#include "include/shared_ptr.h"

#define f(x)

int main(int argc, char *argv[])
{
	int x = 5;
	int *p = &x;
	rstl::shared_ptr<int> sp(p);
	return 0;
}