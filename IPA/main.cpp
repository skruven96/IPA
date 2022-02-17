#include "project.h"
#include "compiler.h"
#include "runtime.h"

#include "ast_printer.h"
#include "opcode_printer.h"

#include <iostream>


int main(int argc, char* argv[])
{
	Project project(argc, argv);
	Runtime runtime(&project);

	{
		Compiler compiler(&project, &runtime);

		compiler.compile();
		if (compiler.encountered_error()) {
			compiler.print_errors(std::cout);
			return -1;
		}
	}

	/*if (!project.encountered_error())
	{
		Runtime runtime(&project);
		runtime.initialize();

		Variable* var = module->scope()->get_variable_or_null("main");
		Function* function = var->as_function_or_null();
		if (function) {
			int return_value;
			runtime.start_call(function)
				.arg(12)
				.call(&return_value, Type::S32());
			std::cout << "ret-value: " << return_value << std::endl;
		}
	}*/
	
	return 0;
}

