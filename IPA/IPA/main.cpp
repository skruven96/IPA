#include "project.h"
#include "runtime.h"

#include "ast_printer.h"
#include "opcode_printer.h"

#include <iostream>


int main(int argc, char* argv[])
{
	Project project(argc, argv);
	Module* module = static_cast<Module*>(project.child("C:/Users/Spankarn/Desktop/funzy/test.ipa"));

	project.parse();
	project.link();
	project.compile();

	ASTPrinter printer(std::cout);
	printer.print(&project);

	OpCodePrinter code_printer(std::cout);
	for (auto it = project.recursive_iterate<Function>(); !it.reached_end(); ++it)
		code_printer.print(it->opcode());

	if (!project.has_error())
	{
		Runtime runtime(&project);
		runtime.initialize();

		int return_value;
		runtime.start_call(static_cast<Function*>(module->child("main")))
			.arg(12)
			.call(&return_value, project.i32_t());
		std::cout << "ret-value: " << return_value << std::endl;
	}

	char q;
	std::cin >> q;

	return 0;
}

