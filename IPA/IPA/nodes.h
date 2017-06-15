#ifndef NODES_H
#define NODES_H
#include "handles.h"
#include "types.h"
#include "tokenizer.h"
#include "setting.h"
#include "error.h"
#include "opcodes.h"

#include <assert.h>
#include <vector>
#include <utility>
#include <string.h>
#include <algorithm>
#include <map>
#include <stack>


class UnresolvedLink;
class ServerFunctionCompiler;

class Project;
class Variable;
class Function;
class Class;
class Stmt;

class Node;
class Type;
class ImportStmt;
class Module;
class Block;
class Constant;


template<typename SPECIFIC>
class ASTIterator;


template<typename SPECIFIC>
class ASTRecursiveIterator;


/*
 */
class Node
{
public:
	typedef std::vector<Node*>::const_iterator children_iterator;

	static const u32 ONE = 1;


	enum NodeType : u32
	{
		ProjectType  = ONE,
		ModuleType   = ONE << 1,
		VariableType = ONE << 2,
		FunctionType = ONE << 3,
		ValueType    = ONE << 4,
		TypeType     = ONE << 5,
		ClassType    = ONE << 6,
		BlockType    = ONE << 7,

		StmtType            = ONE << 8,
		ControlFlowStmtType = ONE << 9,
		IfStmtType          = ONE << 10,
		ForStmtType         = ONE << 11,
		WhileStmtType       = ONE << 12,
		OperandStmtType     = ONE << 13,
		CallStmtType        = ONE << 14
	};


	enum Flag : u32
	{
		NoFlags,

		// Shared flags
		ServerFlag = ONE << 0,
		ClientFlag = ONE << 1,

		StaticFlag	  = ONE << 2,
		ContainerFlag = ONE << 3,

		// Variable flags
		ArgumentFlag = ONE << 14,
		LocalFlag    = ONE << 14,
		MemberFlag   = ONE << 15,

		// Function flags
		VirtualFlag = ONE << 14,
		ClosureFlag = ONE << 15,

		// Type flags (Shared with class)
		IntegerFlag =  ONE << 11,
		DecimalFlag =  ONE << 12,
		SignedFlag =   ONE << 13,
		UnsignedFlag = ONE << 14,

		// Class flags
		TemplateFlag = ONE << 15
	};


	template<typename TARGET_TYPE>
	inline static TARGET_TYPE* Cast(Node* node)
	{
		if (node->node_type() == TARGET_TYPE::s_node_type)
			return static_cast<TARGET_TYPE*>(node);
		return nullptr;
	}

	template<typename TARGET_TYPE>
	inline static TARGET_TYPE* FastCast(Node* node)
	{
		assert(node->is_typeof<TARGET_TYPE>() && "");
		return static_cast<TARGET_TYPE*>(node);
	}


	const std::string& name() const { return m_name; }
	NodeType node_type() const { return m_node_type; }
	Node* parent() const { return m_parent; }
	Project* project() const { return m_project; }

	bool is_static() const { return (m_flags & StaticFlag) == StaticFlag; }
	bool is_container() const { return (m_flags & ContainerFlag) == ContainerFlag; }

	bool is_server_side() const { return (m_flags & ServerFlag) == ServerFlag; }
	bool is_client_side() const { return (m_flags & ClientFlag) == ClientFlag; }

	children_iterator children_begin() const { return m_children.cbegin(); }
	children_iterator children_end() const { return m_children.cend(); }

	Node* child(const std::string& name) const;

	template<typename TYPE = Node>
	bool is_typeof() const { return node_type() == TYPE::s_node_type; }

	template<typename SPECIFIC = Node>
	ASTIterator<SPECIFIC> iterate() const { return ASTIterator<SPECIFIC>(this); }

	template<typename SPECIFIC = Node>
	ASTRecursiveIterator<SPECIFIC> recursive_iterate() const { return ASTRecursiveIterator<SPECIFIC>(this); }


protected:
	Node(Node* parent, NodeType type, const std::string& name = "", Flag flags = StaticFlag)
		: m_parent(parent), m_node_type(type), m_name(name), m_flags(flags)
	{
		if (parent != nullptr)
		{
			m_project = m_parent->project();
			parent->m_children.push_back(this);
		}
		else
		{
			assert(is_typeof<Project>() && "Nodes needs a parent. ");
			m_project = Node::FastCast<Project>(this);
		}
	}
	~Node()
	{
		if (m_parent != nullptr)
		{
			auto it = std::find(m_parent->children_begin(), m_parent->children_end(), this);
			m_parent->m_children.erase(it);
		}
	}

	Flag m_flags;

private:
	NodeType m_node_type;
	std::string m_name;

	Project* m_project;

	Node* m_parent;
	std::vector<Node*> m_children;

	template<typename T>
	friend class ASTIterator;
	template<typename T>
	friend class ASTRecursiveIterator;
};


inline Node::Flag operator~(Node::Flag a)
{
	return static_cast<Node::Flag>(~static_cast<u16>(a));
}
inline Node::Flag operator|(Node::Flag a, Node::Flag b)
{
	return static_cast<Node::Flag>(static_cast<u16>(a) | static_cast<u16>(b));
}
inline Node::Flag operator&(Node::Flag a, Node::Flag b)
{
	return static_cast<Node::Flag>(static_cast<u16>(a) & static_cast<u16>(b));
}


inline Node::NodeType operator~(Node::NodeType a)
{
	return static_cast<Node::NodeType>(~static_cast<u16>(a));
}
inline Node::NodeType operator|(Node::NodeType a, Node::NodeType b)
{
	return static_cast<Node::NodeType>(static_cast<u16>(a) | static_cast<u16>(b));
}
inline Node::NodeType operator&(Node::NodeType a, Node::NodeType b)
{
	return static_cast<Node::NodeType>(static_cast<u16>(a) & static_cast<u16>(b));
}


/*
 */
class Project : public Node
{
public:
	static const NodeType s_node_type;

	Project(int argc, char* argv[]);
	~Project();

	Function* initialization_function() const { return m_initialization_function; }

	void parse();
	void link();
	void compile();

	Type* void_t() const { return m_void; }
	Type* bool_t() const { return m_bool; }
	Type* i8_t()  const { return m_i8; }
	Type* u8_t()  const { return m_u8; }
	Type* i16_t() const { return m_i16; }
	Type* u16_t() const { return m_u16; }
	Type* i32_t() const { return m_i32; }
	Type* u32_t() const { return m_u32; }
	Type* i64_t() const { return m_i64; }
	Type* u64_t() const { return m_u64; }
	Type* f32_t() const { return m_f32; }
	Type* f64_t() const { return m_f64; }
	Type* string_t() const { return m_string; }
	Type* callable_t() const { return m_string; }

	void thrown_error() { m_has_error = true; }
	bool has_error() const { return m_has_error; }

private:
	void initialize_builtins();

	std::queue<Module*> m_uncompiled_modules;

	std::map<std::string, Setting*> m_settings;

	Function* m_initialization_function;

	bool m_has_error;

	Type* m_void;
	Type* m_bool;
	Type* m_i8;
	Type* m_u8;
	Type* m_i16;
	Type* m_u16;
	Type* m_i32;
	Type* m_u32;
	Type* m_i64;
	Type* m_u64;
	Type* m_f32;
	Type* m_f64;

	Type* m_string;
	Type* m_callable;

	Setting* m_sources;
	Setting* m_root_dirs;

	Setting* m_css_output_dir;
	Setting* m_html_output_dir;
	Setting* m_javascript_output_dir;

	friend class Module;
	friend class Setting;
	friend class Node;
};


/*
 */
class Type : public Node, public Usable<Type>
{
public:
	static const NodeType s_node_type;

	static Type* CreatePrimitive(const std::string& name, Node* owner, u32 size, Token::Primitive primitive, Flag flags) { return new Type(name, owner, size, primitive, flags); }
	static Type* Primitive(Project* project, Token::Primitive data);

	u32 size() const { return m_size; }

	Token::Primitive primitive() const { return m_primitive; }

	bool is_primtive() const { return m_primitive != Token::NoPrimitive; }

	bool is_integer() const  { return (m_flags & IntegerFlag) == IntegerFlag; }
	bool is_decimal() const  { return (m_flags & DecimalFlag) == DecimalFlag; }
	bool is_signed() const   { return (m_flags & SignedFlag) == SignedFlag; }
	bool is_unsigned() const { return (m_flags & UnsignedFlag) == UnsignedFlag; }

protected:
	Type(const std::string& name, Node* owner, u32 size, Token::Primitive primitive, Flag flags)
		: Node(owner, Node::TypeType, name, StaticFlag | flags), m_size(size), m_primitive(primitive)
	{}

	Type(const std::string& name, Node* owner, u32 size)
		: Node(owner, Node::ClassType, name), m_size(size), m_primitive(Token::NoPrimitive)
	{}

	u32 m_size;
	Token::Primitive m_primitive;
};


/*
 */
class Value : public Usable<Value>
{
public:
	enum StoredLocation : u8
	{
		UnkownLocation,
		HeapLocation,
		StackLocation,
		VTableLocation,
		ConstantsLocation,
		ObjectLocation
	};

	static Value* CreateTemp(Type* type) { return new Value(type, StackLocation, true); }

	Type* type() const { return *m_type; }
	void type(Type* type) { m_type = type; }

	StoredLocation location_stored() const { return m_location; }
	bool stored_on_heap() const { return m_location == HeapLocation; } ///< True if it's a static value stored on the heap. 
	bool stored_on_stack() const { return m_location == StackLocation; } ///< True if it's a local value stored on the stack. 
	bool stored_in_vtable() const { return m_location == VTableLocation; } ///< True if it's a member function stored in the virtual table of an object. 
	bool stored_in_constants() const { return m_location == ConstantsLocation; } ///< True if it's a constant value stored in the functions constants data. 
	bool stored_in_object() const { return m_location == ObjectLocation; } ///< True if it's a member variable stored in an object. 

	bool is_readonly() const { return m_is_readonly; } ///< True if it's is a value that can't be written to. (functions, temporary stack values and constants) 

	u32 address() const { return m_address; } ///< The address of where the value is stored on the stack, heap ... or where ever it's stored. 
	void address(u32 address) { m_address = address; } ///< Set the address of where the value is stored the stack, heap ... or where ever it's stored. 

protected:
	Value(Type* type, StoredLocation location, bool is_readonly = false);

	Handle<Type> m_type;

	u32 m_address;

	bool m_is_readonly;
	StoredLocation m_location;
};


/*
 */
class Module : public Node
{
public:
	/* ImportStmt:
	* An import stmt is used to load variables from modules.
	*/
	class ImportStmt
	{
	private:
		/*
		* @param importee The module that is importing.
		* @param module The module you are importing from the first element is the module.
		* @param from The path to the container importing or importing from.
		* @param as An alternative name from the imported content.
		* @param import The elements to import from the container.
		*/
		ImportStmt(Module* importee, Module* module, const std::vector<Token>& from, const Token& as, const std::vector<Token>& import)
			: m_importee(importee), m_module(module), m_from(from), m_import(import), m_as(as)
		{}

		Module* m_importee;
		Module* m_module;

		std::vector<Token> m_import;
		std::vector<Token> m_from;
		Token m_as;

		friend class Module;
	};

	static const NodeType s_node_type;

	static Module* GetOrCreate(Project* project, const std::string& path);
	static Module* Get(Project* project, const std::string& path);

	void parse();
	void resolve_links();

	ErrorContext* error_context() { return &m_error_context; }
	const ErrorContext* error_context() const { return &m_error_context; }

	Source* source() { return &m_source; }
	const Source* source() const { return &m_source; }

	void import(bool relative, const std::vector<Token>& from, const Token& as, const std::vector<Token>& import = {});

private:
	Module(Project* project, const std::string& path);
	~Module() {}

	// Can't copy modules
	Module(const Module&) = delete;
	Module& operator=(const Module&) = delete;


	ErrorContext m_error_context;
	Source m_source;

	std::vector<ImportStmt> m_import_statments;
	std::vector<UnresolvedLink*> m_unresolved_links;

	friend class Parser;
	friend class UnresolvedLink;
};
 

/* Variables represent static, member and local variables.
 */
class Variable : public Node, public Value
{
public:
	static const NodeType s_node_type;

	static Variable* CreateStatic(Node* owner, Type* type, const std::string& name);
	static Variable* CreateMember(Class* owner, Type* type, const std::string& name);
	static Variable* CreateLocal(Block* owner, Type* type, const std::string& name);

	bool is_argument()  const { return (m_flags & ArgumentFlag) == ArgumentFlag; }
	bool is_local()  const { return (m_flags & LocalFlag) == LocalFlag; }
	bool is_member() const { return (m_flags & MemberFlag) == MemberFlag; }

	Value* default_value() const { return *m_default_value; }

private:
	Variable(Node* owner, Type* type, const std::string& name, Flag flags);

	Variable(const Variable& other) = delete;
	Variable& operator=(const Variable& rhs) = delete;

	Handle<Value> m_default_value;

	friend class Parser;
};


/* \brief FunctionOverload represent both static functions, member functions(methods) and closure functions.
 *
 */
class Function : public Node, public Value
{
public:
	static const NodeType s_node_type;

	typedef std::vector<Variable*>::const_iterator argument_iterator;

	static Function* CreateStatic(Node* owner, const std::string& name);
	static Function* CreateMethod(Class* object, const std::string& name);

	void add_argument(Variable* argument) { m_arguments.push_back(argument); }

	int num_arguments() const { return m_arguments.size(); }
	int num_required_arguments() const { return m_arguments.size(); }
	argument_iterator arguments_begin() const { return m_arguments.cbegin(); }
	argument_iterator arguments_end() const { return m_arguments.cend(); }

	void overrides(Function* method) { m_overrides = method; method->m_overrided_by.push_back(this); }
	Function* overrides() const { return m_overrides; }

	void return_type(Type* type) { m_return_type = type; }
	Type* return_type() const { return *m_return_type; }

	Type* this_type() const { return *m_this_type; }
	Type* closure_tuple() const { return *m_closure_tuple; }

	bool is_method() const { return *m_this_type != nullptr; }
	bool is_closure_function() const { return *m_closure_tuple != nullptr; }

	Block* block() const { return m_block; }
	OpCodes opcode() const { return m_opcode; }

private:
	Function(Node* function, const std::string& name, Flag flags);
	Function(Class* cls, const std::string& name, Flag flags);

	std::vector<Variable*> m_arguments;
	std::vector<Function*> m_overloads;
	std::vector<Function*> m_overrided_by;

	Function* m_overrides;

	Handle<Type> m_return_type;
	Handle<Type> m_this_type;
	Handle<Type> m_closure_tuple;

	Block* m_block;
	OpCodes m_opcode;

	friend class ServerFunctionCompiler;
};


/*
 */
class Block : public Node
{
public:
	static const NodeType s_node_type;

	typedef std::vector<Constant*>::const_iterator constant_iterator;
	typedef std::vector<Stmt*>::const_iterator stmt_iterator;

	static Block* Create(Node* owner, const std::string& name) { return new Block(owner, name); }

	constant_iterator constants_begin() const { return m_constants.cbegin(); }
	constant_iterator constants_end() const { return m_constants.cend(); }

	stmt_iterator stmts_begin() const { return m_statments.cbegin(); }
	stmt_iterator stmts_end() const { return m_statments.cend(); }

	bool is_empty() const { return m_statments.empty(); }

private:
	Block(Node* owner, const std::string& name)
		: Node(owner, Node::BlockType, name, ContainerFlag | StaticFlag)
	{}

	std::vector<Stmt*> m_statments;
	std::vector<Constant*> m_constants;

	friend class Stmt;
	friend class Constant;
};


/*
 */
class Class : public Type
{
public:
	static const NodeType s_node_type;

	static Class* Create(Node* owner, const std::string& name, const std::vector<std::string>& template_parameters = {}, const std::vector<Type*>& extends = {});

private:
	Class(Node* owner, const std::string& name, const std::vector<std::string>& template_parameters = {}, const std::vector<Type*>& extends = {})
		: Type(name, owner, sizeof(u32))
	{}
};


/*
*/
class Widget : public Class
{
public:
	static const NodeType s_node_type;

private:

};


/*
 */
class Constant : public Value
{
public:
	static Constant* Integer(Block* owner, i64 value)				{ return new Constant(owner, value); }
	static Constant* Float(Block* owner,  f32 value)				{ return new Constant(owner, value); }
	static Constant* Double(Block* owner, f64 value)				{ return new Constant(owner, value); }
	static Constant* String(Block* owner, const std::string& value)	{ return new Constant(owner, value); }

	i64 integer() const	       { assert(*m_type == m_block->project()->i32_t()    && "Constant is of wrong type. "); return m_integer; }
	f32 decimal_float() const  { assert(*m_type == m_block->project()->f32_t()    && "Constant is of wrong type. "); return m_float; }
	f64 decimal_double() const { assert(*m_type == m_block->project()->f64_t()    && "Constant is of wrong type. "); return m_double; }
	std::string string() const { assert(*m_type == m_block->project()->string_t() && "Constant is of wrong type. "); return std::string(m_string); }

	template<typename T>
	T as_integer() const { return static_cast<T>(m_integer); }

	int index() const { return std::find(m_block->constants_begin(), m_block->constants_end(), this) - m_block->constants_begin(); }
	std::string to_string() const;
	~Constant()
	{
		if (*m_type == m_block->project()->string_t())
			delete[] m_string;
	}

private:
	Constant(Block* owner, i64 value)
		: Value(owner->project()->i32_t(), Value::ConstantsLocation, true), m_block(owner), m_integer(value)
	{
		owner->m_constants.push_back(this);
	}
	Constant(Block* owner, f32 value)
		: Value(owner->project()->f32_t(), Value::ConstantsLocation, true), m_block(owner), m_float(value)
	{
		owner->m_constants.push_back(this);
	}
	Constant(Block* owner, f64 value)
		: Value(owner->project()->f64_t(), Value::ConstantsLocation, true), m_block(owner), m_double(value)
	{
		owner->m_constants.push_back(this);
	}
	Constant(Block* owner, const std::string& value)
		: Value(owner->project()->string_t(), Value::ConstantsLocation, true), m_block(owner)
	{
		owner->m_constants.push_back(this);
	}

	Block* m_block;
	union
	{
		i64 m_integer;
		f32 m_float;
		f64 m_double;
		char* m_string;
	};
};


// ===============================================
// ASTIterator implementation
// ===============================================


template<typename SPECIFIC>
class ASTIterator
{
public:
	ASTIterator()
		: m_container(nullptr)
	{}
	ASTIterator(const Node* start);

	void operator++();

	SPECIFIC* operator->() const { return static_cast<SPECIFIC*>(*m_current); }
	SPECIFIC* operator*() const { return static_cast<SPECIFIC*>(*m_current); }

	bool reached_end() const { return m_current == m_container->m_children.cend(); }

private:
	void skip_wrong_nodes();

	const Node* m_container;
	Node::children_iterator m_current;
};


template<typename SPECIFIC>
class ASTRecursiveIterator
{
public:
	ASTRecursiveIterator()
		: m_container(nullptr)
	{}
	ASTRecursiveIterator(const Node* start);

	void operator++();

	SPECIFIC* operator->() const { return static_cast<SPECIFIC*>(*m_current); }
	SPECIFIC* operator*() const { return static_cast<SPECIFIC*>(*m_current); }

	bool reached_end() const { return m_current == m_container->m_children.cend(); }

private:
	void skip_wrong_nodes();

	const Node* m_container;
	Node::children_iterator m_current;
	std::stack<Node::children_iterator> m_iterators;
};


template<typename SPECIFIC>
ASTIterator<SPECIFIC>::ASTIterator(const Node* start)
	: m_container(start), m_current(start->m_children.cbegin())
{
	skip_wrong_nodes();
}


template<typename SPECIFIC>
void ASTIterator<SPECIFIC>::operator++()
{
	++m_current;
	skip_wrong_nodes();
}


template<typename SPECIFIC>
void ASTIterator<SPECIFIC>::skip_wrong_nodes()
{
	if (reached_end()) return;
	while ((*m_current)->node_type() != SPECIFIC::s_node_type)
	{
		++m_current;
		if (reached_end()) return;
	}
}


template<>
inline void ASTIterator<Node>::skip_wrong_nodes()
{}


template<typename SPECIFIC>
ASTRecursiveIterator<SPECIFIC>::ASTRecursiveIterator(const Node* start)
	: m_container(start), m_current(start->m_children.cbegin())
{
	skip_wrong_nodes();
}


template<typename SPECIFIC>
void ASTRecursiveIterator<SPECIFIC>::operator++()
{
	if ((*m_current)->is_container())
	{
		m_iterators.push(m_current);
		m_container = *m_current;
		m_current = m_container->m_children.cbegin();
		skip_wrong_nodes();
		return;
	}

	++m_current;
	skip_wrong_nodes();
}


template<typename SPECIFIC>
void ASTRecursiveIterator<SPECIFIC>::skip_wrong_nodes()
{
	if (!reached_end())
	{
		if ((*m_current)->node_type() == SPECIFIC::s_node_type) return;

		while ((*m_current)->node_type() != SPECIFIC::s_node_type)
		{
			if ((*m_current)->is_container())
			{
				m_iterators.push(m_current);
				m_container = (*m_current);
				m_current = m_container->m_children.cbegin();
				skip_wrong_nodes();
				return;
			}

			++m_current;
			if (reached_end())
			{
				if (m_iterators.size() != 0)
				{
					m_container = m_container->parent();
					m_current = m_iterators.top();
					m_iterators.pop();

					++m_current;
					skip_wrong_nodes();
				}
				break;
			}
		}
	}
	else if (m_iterators.size() != 0)
	{
		m_container = m_container->parent();
		m_current = m_iterators.top();
		m_iterators.pop();
		
		++m_current;
		skip_wrong_nodes();
	}
}


template<>
inline void ASTRecursiveIterator<Node>::skip_wrong_nodes()
{}


#endif // NODES_H