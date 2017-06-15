#include "nodes.h"

#include "project.h"
#include "tokenizer.h"
#include "parser.h"
#include "links.h"

#include "server_compiler.h"

#include <limits>
#include <algorithm>
#include <assert.h>
#include <sys/stat.h>


const Node::NodeType Project::s_node_type =  ProjectType;
const Node::NodeType Type::s_node_type =     TypeType;
const Node::NodeType Module::s_node_type =   ModuleType;
const Node::NodeType Variable::s_node_type = VariableType;
const Node::NodeType Function::s_node_type = FunctionType;
const Node::NodeType Class::s_node_type = ClassType;
const Node::NodeType Block::s_node_type = BlockType;


Node* Node::child(const std::string& name) const
{
	for (auto it = children_begin(); it != children_end(); ++it)
		if ((*it)->name() == name)
			return *it;
	return nullptr;
}


// =========================================================================================================
// Type
// =========================================================================================================


Type* Type::Primitive(Project* project, Token::Primitive data)
{
	switch (data)
	{
	case Token::VoidPrimitive:
		return project->void_t();
	case Token::BoolPrimitive:
		return project->bool_t();
	case Token::I8Primitive:
		return project->i8_t();
	case Token::U8Primitive:
		return project->u8_t();
	case Token::I16Primitive:
		return project->i16_t();
	case Token::U16Primitive:
		return project->u16_t();
	case Token::I32Primitive:
		return project->i32_t();
	case Token::U32Primitive:
		return project->u32_t();
	case Token::I64Primitive:
		return project->i64_t();
	case Token::U64Primitive:
		return project->u64_t();
	case Token::F32Primitive:
		return project->f32_t();
	case Token::F64Primitive:
		return project->f64_t();
	default:
		assert(false && "Type::Primitive no primitive found. ");
		return nullptr;
	}
}


// =========================================================================================================
// Value
// =========================================================================================================


Value::Value(Type* type, StoredLocation location, bool is_readonly)
	: m_type(type), m_address(std::numeric_limits<u32>::max()), m_location(location), m_is_readonly(is_readonly)
{}


// =========================================================================================================
// Project
// =========================================================================================================


Project::Project(int argc, char* argv[])
	: Node(nullptr, ProjectType, "<project-name>", Node::StaticFlag | Node::ContainerFlag), m_has_error(false)
{
	m_sources = Setting::Create(this, "sources");

	for (int i = 1; i < argc; i++)
	{
		std::string arg(argv[i]);

		auto pos = arg.find_first_of('=');

		std::string name(arg.substr(0, pos));
		std::string value(arg.substr(pos + 1));

		Setting::Get(this, name);
	}

	initialize_builtins();

	m_initialization_function = Function::CreateStatic(this, "<initialization_function>");
	m_initialization_function->return_type(void_t());

	Module::GetOrCreate(this, "C:/Users/Spankarn/Desktop/funzy/test.ipa");
}


Project::~Project()
{}


void Project::parse()
{
	while (!m_uncompiled_modules.empty())
	{
		Module* module = m_uncompiled_modules.front();
		module->parse();

		m_uncompiled_modules.pop();
	}
}


void Project::link()
{
	if (has_error())
		return;

	Linker linker(this);
	linker.link_all();
}


void Project::compile()
{
	if (has_error())
		return;
	
	ServerCompiler compiler(this);
	compiler.compile();
}


void Project::initialize_builtins()
{
	m_void = Type::CreatePrimitive("void", this, 0, Token::VoidPrimitive, NoFlags);
	m_bool = Type::CreatePrimitive("bool", this, 1, Token::BoolPrimitive, NoFlags);
	m_i8 =  Type::CreatePrimitive("i8", this, 1, Token::I8Primitive, IntegerFlag | SignedFlag);
	m_u8 =  Type::CreatePrimitive("u8", this, 1, Token::U8Primitive, IntegerFlag | UnsignedFlag);
	m_i16 = Type::CreatePrimitive("i16", this, 2, Token::I16Primitive, IntegerFlag | SignedFlag);
	m_u16 = Type::CreatePrimitive("u16", this, 2, Token::U16Primitive, IntegerFlag | UnsignedFlag);
	m_i32 = Type::CreatePrimitive("i32", this, 4, Token::I32Primitive, IntegerFlag | SignedFlag);
	m_u32 = Type::CreatePrimitive("u32", this, 4, Token::U32Primitive, IntegerFlag | UnsignedFlag);
	m_i64 = Type::CreatePrimitive("i64", this, 8, Token::I64Primitive, IntegerFlag | SignedFlag);
	m_u64 = Type::CreatePrimitive("u64", this, 8, Token::U64Primitive, IntegerFlag | UnsignedFlag);
	m_f32 = Type::CreatePrimitive("f32", this, 4, Token::F32Primitive, DecimalFlag | SignedFlag);
	m_f64 = Type::CreatePrimitive("f64", this, 8, Token::F64Primitive, DecimalFlag | SignedFlag);

	m_string = Class::Create(this, "String");
	m_callable = Class::Create(this, "Callable");
}


// =========================================================================================================
// Module
// =========================================================================================================


Module* Module::GetOrCreate(Project* project, const std::string& path)
{
	Module* module = Get(project, path);

	if (module == nullptr)
		module = new Module(project, path);

	return module;
}


Module* Module::Get(Project* project, const std::string& path)
{
	for (auto it = project->iterate<Module>(); !it.reached_end(); ++it)
		if (it->name() == path)
			return *it;

	return nullptr;
}


void Module::parse()
{
	m_source.open();

	Tokenizer tokenizer(source());

	Parser parser(tokenizer, this);

	parser.parse();

	m_source.close();
}


void Module::resolve_links()
{
	auto failed_link_iterator = m_unresolved_links.begin();

	for (auto it = m_unresolved_links.begin(); it != m_unresolved_links.end(); ++it)
	{
		try
		{
			(*it)->resolve();
			delete *it;
		}
		catch (LinkerError* /*error*/)
		{
			*failed_link_iterator = *it;
			++failed_link_iterator;
		}
	}

	m_unresolved_links.resize(failed_link_iterator - m_unresolved_links.begin());
}


void Module::import(bool relative, const std::vector<Token>& from, const Token& as, const std::vector<Token>& import)
{
	if (relative)
	{

	}

	m_import_statments.push_back(ImportStmt(this, nullptr, from, as, import));
}


Module::Module(Project* project, const std::string& path)
	: Node(project, NodeType::ModuleType, path, Node::StaticFlag | Node::ContainerFlag), m_source(path), m_error_context(project)
{
	project->m_uncompiled_modules.push(this);
}


// =========================================================================================================
// Variable
// =========================================================================================================


Variable* Variable::CreateStatic(Node* owner, Type* type, const std::string& name)
{
	return new Variable(owner, type, name, StaticFlag);
}


Variable* Variable::CreateMember(Class* owner, Type* type, const std::string& name)
{
	return new Variable(owner, type, name, MemberFlag);
}


Variable* Variable::CreateLocal(Block* owner, Type* type, const std::string& name)
{
	return new Variable(owner, type, name, LocalFlag);
}


Variable::Variable(Node* owner, Type* type, const std::string& name, Flag flags)
	: Node(owner, Node::VariableType, name, flags), Value(type, Value::UnkownLocation)
{
	if (is_local())
		m_location = Value::StackLocation;
	else if (is_static())
		m_location = Value::HeapLocation;
	else
		m_location = Value::ObjectLocation;
}


// =========================================================================================================
// Function
// =========================================================================================================


Function* Function::CreateStatic(Node* function, const std::string& name)
{
	return new Function(function, name, StaticFlag | ContainerFlag);
}


Function* Function::CreateMethod(Class* cls, const std::string& name)
{
	return new Function(cls, name, StaticFlag | ContainerFlag);
}


Function::Function(Node* owner, const std::string& name, Flag flags)
	: Node(owner, NodeType::FunctionType, name, flags), Value(owner->project()->callable_t(), Value::HeapLocation, true), m_return_type(nullptr)
{
	m_block = Block::Create(this, "<entry-block>");
	m_closure_tuple = nullptr;
	m_this_type = nullptr;
	m_overrides = nullptr;
}


Function::Function(Class* cls, const std::string& name, Flag flags)
	: Node(cls, Node::FunctionType, name, flags), Value(cls->project()->callable_t(), Value::HeapLocation, true), m_return_type(nullptr)
{
	m_block = Block::Create(this, "<entry-block>");
	m_closure_tuple = nullptr;
	m_this_type = cls;
	m_overrides = nullptr;
}


// =========================================================================================================
// Class
// =========================================================================================================


Class* Class::Create(Node* owner, const std::string& name, const std::vector<std::string>& template_parameters, const std::vector<Type*>& extends)
{
	return new Class(owner, name, template_parameters, extends);
}


// =========================================================================================================
// Constant
// =========================================================================================================


std::string Constant::to_string() const
{
	switch (m_type->primitive())
	{
	case(Token::I32Primitive):
		return std::to_string(m_integer);
	case(Token::F32Primitive):
		return std::to_string(m_float);
	case(Token::F64Primitive):
		return std::to_string(m_double);
	default:
		assert(false && "Couldn't convert constant to string. ");
	}

	return "";
}

