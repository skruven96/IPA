#ifndef STMT_H
#define STMT_H
#include "nodes.h"
#include "tokenizer.h"

#include <assert.h>


class Parser;
class Linker;


class Stmt : public Node
{
public:
	static Node::NodeType s_node_type;

	Block* owner() const { return m_owner; }
	
protected:
	Stmt(Block* owner, Node::NodeType type)
		: Node(owner, type), m_owner(owner)
	{
		owner->m_statments.push_back(this);
	}

	Block* m_owner;
};


/* \brief TargetingStmt represent any stmt that sets a target value.
*/
class TargetingStmt : public Stmt
{
public:
	static const NodeType s_node_type;

	Value* target() const { return *m_target; }
	void target(Value* target) { m_target = target; }

protected:
	TargetingStmt(Block* owner, Node::NodeType type, Value* target)
		: Stmt(owner, type), m_target(target)
	{
		if (target == nullptr)
			m_target = Value::CreateTemp(owner->project()->i32_t());
	}

	Handle<Value> m_target;
};


/* \brief LoadStmt loads constants, static or member variables into a target. 
 */
class LoadStmt : public TargetingStmt
{
public:
	static const NodeType s_node_type;

	static LoadStmt* LoadConstant(Block* owner, Constant* constant, Value* target);
	static LoadStmt* LoadStatic(Block* owner, Value* value, Value* target);
	static LoadStmt* LoadMember(Block* owner, Value* from, Variable* member, Value* target);

private:
	Handle<Value> m_from;
	Handle<Value> m_value;
};


/* \brief CopyStmt copys a value into a target static or member variable. 
 */
class CopyStmt : public TargetingStmt
{
public:
	static const NodeType s_node_type;
	
	static const CopyStmt* Create(Block* owner, Value* value, Value* target);

	Value* target() const { return *m_target; }

private:
	Handle<Value> m_value;
};


/* Is a statment that controls the flow the of the execution somehow which is any of these 4:
 *     break, continue, raise and return.
 */
class ControlFlowStmt : public Stmt
{
public:
	static Node::NodeType s_node_type;

	static ControlFlowStmt* CreateReturn(Block* owner, Value* return_value = nullptr) { return new ControlFlowStmt(owner, return_value); }
	static ControlFlowStmt* CreateRaise(Block* owner, Value* error_thrown = nullptr)  { return new ControlFlowStmt(owner, error_thrown); }
	static ControlFlowStmt* CreateBreak(Block* owner)    { return new ControlFlowStmt(owner, nullptr); }
	static ControlFlowStmt* CreateContinue(Block* owner) { return new ControlFlowStmt(owner, nullptr); }

	Value* value() const { return *m_value; }

private:
	ControlFlowStmt(Block* owner, Value* value)
		: Stmt(owner, ControlFlowStmtType), m_value(value)
	{}

	Handle<Value> m_value;
};


class IfStmt : public Stmt
{
public:
	static Node::NodeType s_node_type;

	static IfStmt* Create(Block* owner, Value* condition) { return new IfStmt(owner, condition); }

	Value* condition() const { return *m_condition; }
	Block* true_block() const { return m_true_block; }
	Block* false_block() const { return m_false_block; }

private:
	IfStmt(Block* owner, Value* condition)
		: Stmt(owner, IfStmtType), m_true_block(Block::Create(owner, "<true-block>")), m_false_block(Block::Create(owner, "<false-block>")), m_condition(condition)
	{}

	Handle<Value> m_condition;
	Block* m_true_block;
	Block* m_false_block;

	friend class Parser;
};


class ForStmt : public Stmt
{
public:
	static NodeType s_node_type;
private:

};


class WhileStmt : public Stmt
{
public:
	static NodeType s_node_type;

	static WhileStmt* Create(Block* owner) { return new WhileStmt(owner); }

	Block* block() const { return m_block; }

	Value* condition() const { return *m_condition; }
	void condition(Value* condition) { m_condition = condition; }

private:
	WhileStmt(Block* owner)
		: Stmt(owner, WhileStmtType), m_block(Block::Create(owner, "<while-block>"))
	{}

	Handle<Value> m_condition;
	Block* m_block;
};


/* \brief OperandStmt represents an operation on native types. (Overloaded operators is replaced by calls in the linker. )
 * 
 * 
 */
class OperandStmt : public TargetingStmt
{
public:
	static NodeType s_node_type;

	static OperandStmt* Create(Block* owner, Token operand, Value* lhs, Value* rhs, Value* target) { return new OperandStmt(owner, operand, lhs, rhs, target); }

	Token operand() const { return m_operand; }
	Type* term_type() const { return m_term_type; }

	Value* lhs() const { return *m_lhs; }
	Value* rhs() const { return *m_rhs; }

	bool is_unary_prefix() const { return *m_lhs == nullptr; }
	bool is_unary_postfix() const { return *m_rhs == nullptr; }

private:
	OperandStmt(Block* owner, Token operand, Value* lhs, Value* rhs, Value* target)
		: TargetingStmt(owner, OperandStmtType, target), m_operand(operand), m_term_type(nullptr), m_lhs(lhs), m_rhs(rhs)
	{}
	
	Token m_operand;
	Type* m_term_type;
	Handle<Value> m_lhs;
	Handle<Value> m_rhs;

	friend class Linker;
};


/* \brief CallStmt handles calling other functions. 
 * 
 * A call expects the arguments to be in left to right order at the beginning of the stack when called. 
 * 
 */
class CallStmt : public TargetingStmt
{
public:
	typedef std::vector<Handle<Value>>::const_iterator argument_iterator;

	static NodeType s_node_type;

	static CallStmt* Create(Block* owner, Value* callable, const std::vector<Value*>& arguments, Value* return_target) { return new CallStmt(owner, callable, arguments, return_target); }

	Value* callable() const { return *m_callable; }

	argument_iterator arguments_begin() const { return m_arguments.begin(); }
	argument_iterator arguments_end() const { return m_arguments.end(); }

private:
	CallStmt(Block* owner, Value* callable, const std::vector<Value*>& arguments, Value* return_target)
		: TargetingStmt(owner, CallStmtType, return_target), m_callable(callable), m_arguments(arguments.size())
	{
		for (u32 i = 0; i < arguments.size(); i++)
			m_arguments[i] = arguments[i];
	}

	Handle<Value> m_callable;

	std::vector<Handle<Value>> m_arguments;
};


#endif // STMT_H