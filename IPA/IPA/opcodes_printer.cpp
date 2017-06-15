#include "opcode_printer.h"
#include "nodes.h"

#include <assert.h>


void OpCodePrinter::print(OpCodes code)
{
	if (!code.is_compiled())
	{
		m_stream << "<uncompiled>" << std::endl << std::endl;
		return;
	}

	Function* function = code.function();
	m_stream << function->name() << std::endl;
	Op* op = code.start_operation();

	while (true)
	{
		switch (op->code())
		{
		case(OpEmptyCode):
			m_stream << std::endl;
			return;

		case(OpIF):
			m_stream << "if " << op->a() << std::endl;
			break;
		case(OpJUMP):
			m_stream << "jmp " << op->sbx() << std::endl;
			break;

		case(OpCALLLocal):
			m_stream << "call.local " << op->a() << " <- " << op->b() << std::endl;
			break;
		case(OpCALLStatic):
			m_stream << "call.static " << op->bx() << std::endl;
			break;
		case(OpCALLVirtual):
			m_stream << "call.virtual " << op->a() << "." << op->b() << std::endl;
			break;

		case(Op32LOAD_CONSTANT):
			m_stream << "loadc " << op->a() << " -> " << op->b() << std::endl;
			break;
		case(Op32LOAD_MEMBER):
			m_stream << "loadm " << op->a() << "->" << op->b() << " -> " << op->c() << std::endl;
			break;
		case(Op32LOAD_STATIC):
			m_stream << "loads " << op->bx() << " -> " << op->a() << std::endl;
			break;

		case(Op32SET_LOCAL):
			m_stream << "setl " << op->a() << " -> " << op->c() << std::endl;
			break;
		case(Op32SET_MEMBER):
			m_stream << "setm " << op->a() << "->" << op->b() << " <- " << op->c() << std::endl;
			break;
		case(Op32SET_STATIC):
			m_stream << "sets " << op->a() << " -> " << op->bx() << std::endl;
			break;

		case(Op32RETURN):
			m_stream << "return " << op->a() << std::endl;
			break;
		case(OpRETURN_VOID):
			m_stream << "return" << std::endl;
			break;

		case(OpI32ADD):
			m_stream << "i32.add " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;
		case(OpI32SUB):
			m_stream << "i32.sub " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;
		case(OpI32MUL):
			m_stream << "i32.mul " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;
		case(OpI32DIV):
			m_stream << "i32.div " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;
		case(OpI32MOD):
			m_stream << "i32.mod " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;

		case(OpI32LT):
			m_stream << "i32.lt " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;
		case(OpI32LE):
			m_stream << "i32.le " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;

		case(OpI32GT):
			m_stream << "i32.gt " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;
		case(OpI32GE):
			m_stream << "i32.ge " << op->a() << " " << op->b() << " -> " << op->c() << std::endl;
			break;

		case(Op32NOT):
			m_stream << "32.not " << op->a() << " -> " << op->c() << std::endl;
			break;
		default:
			assert(false && "Unimplemented opcode printer. ");
		}

		++op;
	}
}

