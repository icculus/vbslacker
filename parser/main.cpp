/* This will exist until we turn this parse into a shared library */

#include <iostream>
#include "BasicLexer.hpp"
#include "BasicParser.hpp"

int main()
{
	try {
		BasicLexer lexer(std::cin);
		BasicParser parser(lexer);

	   parser.start();
	} catch(exception& e) {
	   std::cerr << "exception: " << e.what() << std::endl;
	}

	return 0;
}
