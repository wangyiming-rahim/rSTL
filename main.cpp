#include <iostream>
#include <string>

#include "include/utility.h"
#include "include/type_traits.h"

int main(int argc, char *argv[])
{
	rstl::pair<int, double> p(2, 3);

	std::cout << p << std::endl;


	return 0;
}