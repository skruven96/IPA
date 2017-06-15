#include "ast_printer.h"
#include "nodes.h"
#include "stmt.h"


void ASTPrinter::print(Node* node)
{
	Node::NodeType type = node->node_type();

	switch (type)
	{
	case Node::ProjectType:
		print(Node::Cast<Project>(node));
		break;
	case Node::ModuleType:
		print(Node::Cast<Module>(node));
		break;
	case Node::VariableType:
		print(Node::Cast<Variable>(node));
		break;
	case Node::FunctionType:
		print(Node::Cast<Function>(node));
		break;
	case Node::BlockType:
		print(Node::Cast<Block>(node));
		break;
	case Node::TypeType:
		print(Node::Cast<Type>(node));
		break;
	case Node::ClassType:
		print(Node::Cast<Class>(node));
		break;
	case(Node::ControlFlowStmtType):
		print(Node::Cast<ControlFlowStmt>(node));
		break;
	case(Node::IfStmtType):
		print(Node::Cast<IfStmt>(node));
		break;
	case(Node::ForStmtType):
		print(Node::Cast<ForStmt>(node));
		break;
	case(Node::WhileStmtType):
		print(Node::Cast<WhileStmt>(node));
		break;
	case(Node::OperandStmtType):
		print(Node::Cast<OperandStmt>(node));
		break;
	case(Node::CallStmtType):
		print(Node::Cast<CallStmt>(node));
		break;
	default:
		assert(false && "Unimplemented node type in ast printer. ");
	}

	m_stream << std::endl;
}


void ASTPrinter::print(Project* project)
{
	indent() << "project " << project->name() << std::endl;

	for (auto it = project->iterate(); !it.reached_end(); ++it)
		print(*it);

	m_stream << std::endl << std::endl;
}


void ASTPrinter::print(Module* module)
{
	m_stream << module->source()->path() << std::endl << std::endl;

	++m_indention;

	ErrorContext* error_context = module->error_context();
	indent() << "errors:" << std::endl;

	++m_indention;
	if (error_context->no_errors())
		indent() << "<no-errors>\n";
	else
	{
		for (auto it = error_context->begin(); it != error_context->end(); ++it)
			indent() << (*it)->stringify() << std::endl;
	}
	--m_indention;

	indent() << "\nimports:\n";
	++m_indention;
	indent() << "<nothing>\n\n";
	--m_indention;

	for (auto it = module->iterate(); !it.reached_end(); ++it)
		print(*it);

	--m_indention;
}


void ASTPrinter::print(Variable* variable)
{
	indent() << "var " << variable->name() << " #" << variable << " : ";
	print(variable->type());
}


void ASTPrinter::print(Function* function)
{
	indent() << "def " << function->name() << " #" << function << ":\n";

	++m_indention;
	
	indent() << "constants:\n";
	indent() << "arguments:\n";

	for (auto it = function->iterate(); !it.reached_end(); ++it)
		print(*it);

	--m_indention;
}


void ASTPrinter::print(Class* cls)
{
	++m_indention;

	--m_indention;
}


void ASTPrinter::print(Block* block)
{
	indent() << block->name() << std::endl;

	++m_indention;

	for (auto it = block->iterate(); !it.reached_end(); ++it)
		print(*it);

	--m_indention;
}


void ASTPrinter::print(Stmt* stmt)
{
	switch (stmt->node_type())
	{
	case(Node::ControlFlowStmtType):
		print(Node::Cast<ControlFlowStmt>(stmt));
		break;
	case(Node::IfStmtType):
		print(Node::Cast<IfStmt>(stmt));
		break;
	case(Node::ForStmtType):
		print(Node::Cast<ForStmt>(stmt));
		break;
	case(Node::WhileStmtType):
		print(Node::Cast<WhileStmt>(stmt));
		break;
	case(Node::OperandStmtType):
		print(Node::Cast<OperandStmt>(stmt));
		break;
	case(Node::CallStmtType):
		print(Node::Cast<CallStmt>(stmt));
		break;
	default:
		assert(false && "Print for stmt type not implemented. ");
	}
}


void ASTPrinter::print(ControlFlowStmt* stmt)
{
	if (Value* return_value = stmt->value())
	{
		indent() << "ret ";
		print(return_value);
		m_stream << std::endl;
	}
	else
		indent() << "ret" << std::endl;
}


void ASTPrinter::print(IfStmt* stmt)
{
	indent() << "if "; print(stmt->condition()); m_stream << " " << stmt->true_block()->name() << " else " << stmt->false_block()->name() << std::endl;
}


void ASTPrinter::print(ForStmt* stmt)
{

}


void ASTPrinter::print(WhileStmt* stmt)
{

}


void ASTPrinter::print(OperandStmt* stmt)
{
	Token::Operand opr = stmt->operand().operand();

	indent();
	switch (opr)
	{
	case Token::SetOperand:
		m_stream << "set ";
		break;
	case Token::AddOperand:
		m_stream << "add ";
		break;
	case Token::SubOperand:
		m_stream << "sub ";
		break;
	case Token::MulOperand:
		m_stream << "mul ";
		break;
	case Token::DivOperand:
		m_stream << "div ";
		break;
	case Token::ModOperand:
		m_stream << "mod ";
		break;
	case Token::LtOperand:
		m_stream << "lt ";
		break;
	case Token::GtOperand:
		m_stream << "gt ";
		break;
	case Token::NotOperand:
		m_stream << "not ";
		break;
	case Token::BinaryAndOperand:
		m_stream << "band ";
		break;
	case Token::BinaryOrOperand:
		m_stream << "bor ";
		break;
	case Token::BinaryXorOperand:
		m_stream << "bxor ";
		break;
	case Token::BinaryNotOperand:
		m_stream << "bnot ";
		break;
	case Token::AddSetOperand:
		m_stream << "add.set ";
		break;
	case Token::SubSetOperand:
		m_stream << "sub.set ";
		break;
	case Token::MulSetOperand:
		m_stream << "mul.set ";
		break;
	case Token::DivSetOperand:
		m_stream << "div.set ";
		break;
	case Token::ModSetOperand:
		m_stream << "mod.set ";
		break;
	case Token::IncrementOperand:
		m_stream << "inc ";
		break;
	case Token::DecrementOperand:
		m_stream << "dec ";
		break;
	case Token::EqualsOperand:
		m_stream << "eq ";
		break;
	case Token::NotEqualsOperand:
		m_stream << "neq ";
		break;
	case Token::LesserEqualsOperand:
		m_stream << "lte ";
		break;
	case Token::GreaterEqualsOperand:
		m_stream << "gte ";
		break;
	case Token::BinaryLShiftOperand:
		m_stream << "bls ";
		break;
	case Token::BinaryRShiftOperand:
		m_stream << "brs ";
		break;
	default:
		assert(false && "Unimplemented Operand in ASTPrinter::print(OperandStmt*). ");
	}

	if (stmt->is_unary_prefix())
	{
		print(stmt->rhs());
	}
	else if (stmt->is_unary_postfix())
	{
		print(stmt->lhs());
	}
	else
	{
		print(stmt->lhs());
		m_stream << " and ";
		print(stmt->rhs());
	}

	m_stream << " -> "; print(stmt->target());
}


void ASTPrinter::print(CallStmt* stmt)
{
	indent() << "call "; print(stmt->callable());
}


std::ostream& ASTPrinter::print(Value* value)
{
	if (value->stored_in_constants())
	{
		m_stream << "[" << static_cast<Constant*>(value)->index() << "]";
	}
	else
	{
		m_stream << "%";

		int& i = m_temp_values[value];
		if (i == 0) i = ++m_temp_values_counter;
		m_stream << std::to_string(i);
	}

	return m_stream;
}


std::ostream& ASTPrinter::print(Type* type)
{
	m_stream << type->name() << " #" << type;
	return m_stream;
}

