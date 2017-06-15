#ifndef AST_PRINTER_H
#define AST_PRINTER_H
#include "nodes.h"
#include "stmt.h"

#include <ostream>
#include <map>


class ASTPrinter
{
public:
	ASTPrinter(std::ostream& stream)
		: m_stream(stream), m_indention(), m_temp_values_counter(0)
	{}

	void print(Node* node);
	void print(Project* project);
	void print(Module* module);
	void print(Variable* variable);
	void print(Function* function);
	void print(Class* cls);
	void print(Block* block);

	void print(Stmt* stmt);
	void print(ControlFlowStmt* stmt);
	void print(IfStmt* stmt);
	void print(ForStmt* stmt);
	void print(WhileStmt* stmt);
	void print(OperandStmt* stmt);
	void print(CallStmt* stmt);

	std::ostream& print(Type* type);
	std::ostream& print(Value* type);

private:
	std::ostream& indent() { return m_stream << std::string(m_indention * 4, ' '); }

	std::ostream& m_stream;

	int m_indention;

	int m_temp_values_counter;
	std::map<Value*, int> m_temp_values;
};

#endif // AST_PRINTER_H