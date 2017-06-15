#ifndef COMPILER_H
#define COMPILER_H
#include "nodes.h"
#include "opcodes.h"

#include <vector>
#include <map>
#include <set>
#include <utility>


class Project;
class Value;
class Module;
class Container;
class Function;

class ControlFlowStmt;
class OperandStmt;
class IfStmt;
class CallStmt;


class ServerCompiler
{
public:
	ServerCompiler(Project* project)
		: m_project(project), m_stack_size(0)
	{}

	void compile();

private:
	void determine_argument_addresses(Function* function) const;

	Project* m_project;
	u32 m_stack_size;
};


class ServerFunctionCompiler
{
public:
	ServerFunctionCompiler(Function* function)
		: m_function(function)
	{}

	void compile();

private:
	
	void compile_block(Block* block);
	void compile_control_flow_stmt(ControlFlowStmt* stmt);
	void compile_operand(OperandStmt* operand);
	void compile_call(CallStmt* stmt);

	u16 push_register(Value* value); ///< Pushes a new register to the top of the stack and return the address. 

	u16 read_value(Value* value, Type* wanted_type); ///< Reads a value and converts it to wanted type if needed and returns the register it's reed into.
	u16 write_value(Value* target, Type* value_type); ///< Writes the value stored in the location of the return value into target.

	static OpCode type_code(Type* type);
	static OpCode size_code(u32 size);

	struct Register
	{
		u16 m_address;
		i16 m_uses;
	};

	struct ArgumentSetter
	{
		bool m_is_c;
		u32 m_operation_index;

		Value* m_argument;
	};

	Function* m_function;
	std::vector<Op> m_opcodes;

	std::vector<u8> m_constants_data; ///< Holds the binary data of the functions constants. 

	std::vector<bool> m_stack_uses; ///< Holds which bytes is ocuppied, true for occupied false for free. 
	std::map<Value*, Register> m_registers; ///< A map of the values currently residing on the stack.

	std::vector<ArgumentSetter> m_argument_stores; ///< Holds the offset of each operation targeting an argument. 
};


#endif // COMPILER_H