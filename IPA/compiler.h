#ifndef COMPILER_H
#define COMPILER_H
#include "common.h"
#include "tokenizer.h"
#include "opcodes.h"

#include <assert.h>
#include <vector>
#include <unordered_map>
#include <forward_list>


class Project;
class Runtime;
class Module;

class Compiler;
class ModuleCompiler;
class CompilerAllocator;


namespace ast {

	struct Type;
	struct Callable;
	struct Scope;

	struct Node;
	struct Decl;
	struct Variable;
	struct Function;
	struct Struct;
	struct Block;

	struct Stmt;
	struct IfStmt;
	struct ForStmt;
	struct WhileStmt;
	struct ReturnStmt;
	struct ExprStmt;

	struct Expr;
	struct LoadExpr;
	struct OperandExpr;
	struct CallExpr;
	struct ArrayAccessExpr;
	struct CastExpr;

	/*
     */
	struct Type {

		static void InitializePrimitveTypes();
		static Type* GetPrimitiveOrAssert(Primitive primitive);

		u32 size;
		u32 flags;

		bool is_callable() const { return (flags & CALLABLE) == CALLABLE; }
		bool is_struct()   const { return (flags & STRUCT) == STRUCT; }
		bool is_integer()  const { return (flags & INTEGER) == INTEGER; }
		bool is_signed()   const { return (flags & SIGNED) == SIGNED; }
		bool is_unsigned() const { return (flags & UNSIGNED) == UNSIGNED; }
		bool is_decimal()  const { return (flags & INTEGER) == INTEGER; }
		bool can_explicitly_cast_to(Type* target) const {
			return (is_signed() && target->is_signed()) ||
				(is_unsigned() && target->is_unsigned()) ||
				(is_decimal() && target->is_decimal());
		}
		Type* get_explicit_type_conversion_target_or_null(Type* target) {
			if (can_explicitly_cast_to(target))
				return size > target->size ? this : target;
			return nullptr;
		}

	protected:
		Type(u32 size, u32 flags)
			: size(size), flags(flags) {}

		~Type() = default;

		static const u32 PRIMITIVE_MASK = 0xF;
		static const u32 INTEGER = 0x10;
		static const u32 DECIMAL = 0x20;
		static const u32 SIGNED = INTEGER;
		static const u32 UNSIGNED = 0x40 & INTEGER;
		static const u32 CALLABLE = 0x80;
		static const u32 STRUCT = 0x100;
		static const u32 UNRESOLVED = 0x200;

	};

	/*
	 */
	struct Callable : public Type {

		Callable(Type* return_type, const std::vector<Type*>& m_argument_types);

		Type* return_type;
		Type** argument_types;
		i32 arguments_count;
	};

	/*
	 */
	class Visitor {
	public:

		inline void accept(Node* node);
		inline void accept(Scope* scope);

		virtual void visit(Scope* scope) {}

		virtual void visit(Variable* variable) {}
		virtual void visit(Struct* structure) {}
		virtual void visit(Function* funnction) {}
		virtual void visit(Block* block) {}

		virtual void visit(IfStmt* expr) {}
		virtual void visit(ForStmt* expr) {}
		virtual void visit(WhileStmt* expr) {}
		virtual void visit(ReturnStmt* expr) {}
		virtual void visit(ExprStmt* expr) {}

		virtual void visit(LoadExpr* expr) {}
		virtual void visit(OperandExpr* expr) {}
		virtual void visit(CallExpr* expr) {}
		virtual void visit(ArrayAccessExpr* expr) {}
		virtual void visit(CastExpr* expr) {}

	private:

	};

	/*
	 */
	struct Scope {
		static Scope* Create(CompilerAllocator* allocator, Scope* parent);
		void initialize(Scope* parent);

		Scope* parent;
		Decl** declerations;
		Decl** declerations_map;
		i32 declerations_count;
		i32 declerations_map_size;

		Decl* get_decleration_or_null(const Token& name);
		Decl* get_decleration_or_null(const std::string& name);
		void fill_out_declerations(CompilerAllocator* allocator, Decl** declerations, i32 count);
	};

	/*
	 */
	struct Node {
		u32 node_type;
		bool been_visited;

		static const u32 NODE = 1u << 0;
		static const u32 DECL = 1u << 1;
		static const u32 VARIABLE = 1u << 2;
		static const u32 STRUCT = 1u << 3;
		static const u32 FUNCTION = 1u << 4;
		static const u32 BLOCK = 1u << 5;
		static const u32 IMPORTED_DECL = 1u << 6;

		static const u32 STMT = 1u << 7;
		static const u32 EXPR = 1u << 8;
		static const u32 LOAD_EXPR = 1u << 9;
		static const u32 IF_STMT = 1u << 10;
		static const u32 FOR_STMT = 1u << 11;
		static const u32 WHILE_STMT = 1u << 12;
		static const u32 RETURN_STMT = 1u << 13;
		static const u32 BREAK_STMT = 1u << 14;
		static const u32 CONTINUE_STMT = 1u << 15;
		static const u32 EXPR_STMT = 1u << 16;

		static const u32 OPERAND_EXPR = 1u << 17;
		static const u32 CALL_EXPR = 1u << 18;
		static const u32 ARRAY_ACCESS_EXPR = 1u << 19;
		static const u32 CAST_EXPR = 1u << 20;

		static const u32 s_node_type = NODE;

		template<typename T>
		inline T* as_or_null();
		template<typename T>
		inline T* as_or_assert();

		void accept(Visitor* visitor);

	protected:
		void init(u32 node_type);
		~Node() = delete;
	};

	/*
	 */
	struct Block : public Node {
		static const u32 s_node_type = BLOCK | Node::s_node_type;

		static Block* Create(CompilerAllocator* allocator, Scope* scope, Variable** local_variables, i32 local_variables_count, Stmt** statements, i32 statements_count);
		
		Scope* scope;
		Variable** local_variables;
		Stmt** statements;
		i32 local_variables_count, statements_count;

	protected:
		void init(CompilerAllocator* allocator, Scope* scope, Variable** local_variables, i32 local_variables_count, Stmt** statements, i32 statements_count);
		~Block() = delete;
	};

	/*
	 */
	struct Decl : public Node {
		static const u32 s_node_type = DECL | Node::s_node_type;

		static const u32 GLOBAL = 0x1;
		static const u32 LOCAL  = 0x2;
		static const u32 MEMBER = 0x4;
		static const u32 CONST  = 0x8;

		Token name;
		u32 decl_flags;

	protected:
		void init(u32 node_type, const Token& name, u32 flags);
		~Decl() = delete;
	};

	/*
	 */
	struct ImportedDecl : public Decl {
		static const u32 s_node_type = IMPORTED_DECL | Decl::s_node_type;


	protected:
		void init();
		~ImportedDecl() = delete;
	};

	/* Variables represent static, member and local variables.
	 */
	struct Variable : public Decl {
		static const u32 s_node_type = VARIABLE | Decl::s_node_type;

		static Variable* Create(CompilerAllocator* allocator, Type* type, const Token& name, Expr* default_value, u32 flags);

		Type* type;
		Expr* default_value;

	protected:
		void init(Type* type, const Token& name, Expr* default_value, u32 flags);
		~Variable() = delete;
	};

	/*
	 */
	struct Struct : public Decl {
		static const u32 s_node_type = STRUCT | Decl::s_node_type;

		static Struct* Create(CompilerAllocator* allocator, const Token& name);
		
		Scope* scope;
		Type* type;

	protected:
		void init(const Token& name);
		~Struct() = delete;
	};

	/*
	 */
	struct Function : public Decl {
		static const u32 s_node_type = FUNCTION | Decl::s_node_type;

		static Function* Create(CompilerAllocator* allocator, const Token& name, Variable** arguments, i32 arguments_count, Type* return_type);

		Callable* type;
		Type* return_type;
		Variable** arguments;
		i32 arguments_count;

		Block* body;
		Function* next_overload;

	protected:
		void init(CompilerAllocator* allocator, const Token& name, Variable** arguments, i32 arguments_count, Type* return_type);
		~Function() = delete;
	};


	// ============================================================================================================
	// |                                      Statments & Expressions                                             |
	// ============================================================================================================

	/*
	 */
	struct Stmt : public Node {
		static const u32 s_node_type = STMT | Node::s_node_type;

	protected:
		void init(u32 node_type);
		~Stmt() = delete;
	};

	/*
	 */
	struct Expr : public Node {
		static const u32 s_node_type = EXPR | Node::s_node_type;

		Type* type;

	protected:
		void init(u32 node_type, Type* type);
		~Expr() = delete;
	};

	/*
	 */
	struct LoadExpr : public Expr {
		static const u32 s_node_type = LOAD_EXPR | Expr::s_node_type;

		static LoadExpr* CreateLoadVariable(CompilerAllocator* allocator, Variable* variable);
		static LoadExpr* CreateLoadConstant(CompilerAllocator* allocator, const Token& constant);
		static LoadExpr* CreateLoadMember(CompilerAllocator* allocator, Expr* structure, Variable* variable);

		Token constant;
		Decl* loaded_decl;
		Expr* structure_expr;

	protected:
		void init(const Token& constant, Variable* variable, Expr* structure_expr);
		~LoadExpr() = delete;
	};

	/*
	 */
	struct IfStmt : public Stmt {
		static const u32 s_node_type = IF_STMT | Stmt::s_node_type;

		static IfStmt* Create(CompilerAllocator* allocator, Expr* condition, Block* true_block, Block* false_block);
		
		Expr* condition;
		Block* true_block;
		Block* false_block;

	protected:
		void init(Expr* condition, Block* true_block, Block* false_block);
		~IfStmt() = delete;
	};

	/*
	 */
	struct ForStmt : public Stmt {
		static const u32 s_node_type = FOR_STMT | Stmt::s_node_type;

		static ForStmt* Create(CompilerAllocator* allocator, Expr* array_expr, Block* block);
		static ForStmt* Create(CompilerAllocator* allocator, Expr* low_expr, Expr* high_expr, Block* block);

		Expr* array_expr;
		Expr* low_expr;
		Expr* high_expr;
		Variable* it_var;
		Variable* index_var;
		Block* block;

	protected:
		void init(Expr* array_expr, Expr* low_expr, Expr* high_expr, Block* block);
		~ForStmt() = delete;
	};

	/*
	 */
	struct WhileStmt : public Stmt {
		static const u32 s_node_type = WHILE_STMT | Stmt::s_node_type;

		static WhileStmt* Create(CompilerAllocator* allocator, Expr* condition, Block* body);

		Expr* condition;
		Block* loop_body;

	protected:
		void init(Expr* condition, Block* body);
		~WhileStmt() = delete;
	};

	/*
	 */
	struct ReturnStmt : public Stmt {
		static const u32 s_node_type = RETURN_STMT | Stmt::s_node_type;

		static ReturnStmt* Create(CompilerAllocator* allocator, Expr* return_value);

		Expr* return_value;

	protected:
		void init(Expr* return_value);
		~ReturnStmt() = delete;
	};

	/*
	 */
	struct ExprStmt : public Stmt {
		static const u32 s_node_type = EXPR_STMT | Stmt::s_node_type;

		static ExprStmt* Create(CompilerAllocator* allocator, Expr* expr);

		Expr* expr;

	protected:
		void init(Expr* expr);
		~ExprStmt() = delete;
	};

	/* \brief OperandExpr represents an operation on native types. (Overloaded operators is replaced by calls in the linker. )
	 */
	struct OperandExpr : public Expr {
		static const u32 s_node_type = OPERAND_EXPR | Expr::s_node_type;

		static OperandExpr* Create(CompilerAllocator* allocator, Operand operand, Expr* lhs, Expr* rhs);

		Operand operand;
		Expr* lhs;
		Expr* rhs;

	protected:
		void init(Operand operand, Expr* lhs, Expr* rhs);
		~OperandExpr() = delete;
	};


	/* \brief CallExpr handles calling other functions.
	 */
	struct CallExpr : public Expr {
		static const u32 s_node_type = CALL_EXPR | Expr::s_node_type;

		static CallExpr* Create(CompilerAllocator* allocator, Expr* callable, Expr** arguments, i32 arguments_count);

		Expr* callable;
		Expr** arguments;
		i32 arguments_count;

	protected:
		void init(CompilerAllocator* allocator, Expr* callable, Expr** arguments, i32 arguments_count);
		~CallExpr() = delete;
	};

	/*
	 */
	struct CastExpr : public Expr {
		static const u32 s_node_type = CAST_EXPR | Expr::s_node_type;

		static CastExpr* Create(CompilerAllocator* allocator, Expr* expr, Type* target_type);

		Expr* expr;

	protected:
		void init(Expr* expr, Type* target_type);
		~CastExpr() = delete;
	};

	/*
	 */
	struct ArrayAccessExpr : public Expr {
		static const u32 s_node_type = ARRAY_ACCESS_EXPR | Expr::s_node_type;

		static ArrayAccessExpr* Create(CompilerAllocator* allocator, Expr* array, Expr* index_expr);

		Expr* array_expr;
		Expr* index_expr;

	protected:
		void init(Expr* array, Expr* index_expr);
		~ArrayAccessExpr() = delete;
	};


	template<typename T>
	T* Node::as_or_null() { return (T::s_node_type & node_type) == T::s_node_type ? static_cast<T*>(this) : nullptr; }
	template<typename T>
	T* Node::as_or_assert() { assert((T::s_node_type & node_type) == T::s_node_type);  return static_cast<T*>(this); }

	void Visitor::accept(Node* node)   { if (node)  node->accept(this); }
	void Visitor::accept(Scope* scope) { if (scope) visit(scope);       }
}


struct Link {
	ast::Decl** m_target_decl;
	ast::Type** m_target_type;
	ast::Scope* m_scope;
	Token m_identifier;

	bool resolve(Module* module, ast::Decl* decl);
};


class CompilerError {
public:
	enum class PartType {
		MODULE,
		MESSAGE,
		TOKEN,
		TOKEN_TYPE,
		OPERAND,
		KEYWORD
	};

	static CompilerError* Create(CompilerAllocator* allocator, Module* module);

	CompilerError* token_type(TokenType type);
	CompilerError* operand(Operand operand);
	CompilerError* keyword(Keyword keyword);

	CompilerError* message(const char* msg);
	CompilerError* message(const std::string& str);
	CompilerError* highlight_token(Token token);

	void print(std::ostream& stream) const;

private:
	CompilerError(CompilerAllocator* allocator, Module* module);
	CompilerError(const char* message);
	CompilerError(Token token);
	CompilerError(TokenType type);
	CompilerError(Operand operand);
	CompilerError(Keyword keyword);
	~CompilerError() = delete;

	void append_part(CompilerError* part);

	CompilerError* m_next_part;
	PartType m_part_type;
	union {
		struct {
			CompilerAllocator* m_allocator;
			Module* m_module;
		};
		const char* m_message;
		Token m_token;
		TokenType m_token_type;
		Operand m_operand;
		Keyword m_keyword;
	};
};


class CompilerAllocator {
	struct Block {
		Block* m_prev;
		Block* m_next;
		i64 m_used;
		i64 m_size;
		alignas(8) u8 m_data[1];
	};

public:
	CompilerAllocator();
	~CompilerAllocator();

	const char* copy_str(const char* str);
	const char* copy_str(const std::string& str);

	template<typename T>
	T* copy_array(T* array, i32 count) {
		if (count == 0)
			return nullptr;
		T* result = allocate_array<T>(count);
		memcpy(result, array, sizeof(array[0]) * count);
		return result;
	}

	template<typename T>
	T* allocate_one() { return (T*)allocate_aligned(sizeof(T), alignof(T)); }
	template<typename T>
	T* allocate_array(i32 count) { return (T*)allocate_aligned(sizeof(T) * count, alignof(T)); }

private:
	void* allocate_aligned(i64 byte_count, i64 alignment);
	Block* allocate_block(i64 size);
	
	Block* m_current_block;
};


class Parser {
public:
	Parser(ModuleCompiler* module_compiler);

	void parse();

private:

	void parse_imports();
	void parse_decleration();
	ast::Type* parse_type();

	ast::Block* parse_block();

	void parse_if_stmt(bool is_elif = false);
	void parse_for_stmt();
	void parse_while_stmt();
	void parse_return_stmt();

	ast::Expr* parse_expr(int* precedens_lvl_out = 0);
	ast::Expr* parse_unary_prefix_operators();
	ast::Expr* parse_unary_postfix_operators(ast::Expr* expr);

	bool add_decleration_or_return_false(ast::Decl* decl);
	bool add_local_variables_or_return_false(ast::Variable* variable);
	ast::Variable* get_local_variable_or_null(const Token& identifier);

	Token required(TokenType data);
	Token required(Keyword data);
	Token required(Operand data);

	Token optional(TokenType data);
	Token optional(Keyword data);
	Token optional(Operand data);

	void required_stmt_end_or_raise_garbage_error_and_continue_after_line();
	CompilerError* raise_error_and_continue();
	CompilerError* raise_error_and_stop();
	CompilerError* raise_error_and_continue_after_line();


	struct Context {
		ast::Scope* scope = nullptr;
		ast::Decl* decl = nullptr;
		i32 first_decl_in_scope_index = 0;
		i32 first_stmt_in_block_index = 0;
		i32 first_local_variable_in_function_index = 0;
		i32 first_local_variable_in_scope_index = 0;
	};
	Context update_context(ast::Scope* block);
	Context update_context(ast::Function* function);
	Context update_context(ast::Struct* structure);
	void get_decls(ast::Decl**& decls, i32& count);
	void get_stmts(ast::Stmt**& stmts, i32& count);
	void get_local_variables(ast::Variable**& variables, i32& count);
	void restore_context(Context& context);

	Context m_ctx;
	std::vector<ast::Decl*> m_declerations_stack;
	std::vector<ast::Variable*> m_local_variables_stack;
	std::vector<ast::Stmt*> m_statements_stack;
	std::vector<ast::Decl*> m_exported_declerations;

	Project* m_project;
	ModuleCompiler* m_module_compiler;
	Tokenizer m_tokenizer;

	CompilerAllocator* m_allocator;

	bool m_should_stop = false;
};


class TypeInferer : public ast::Visitor {
public:
	TypeInferer(ModuleCompiler* module_compiler)
		: m_module_compiler(module_compiler) { }

	void infer_types();

	bool mark_visited(ast::Node* node);
	void jump_to(ast::Node* node);

	virtual void visit(ast::Scope* scope) override;

	virtual void visit(ast::Variable* variable) override;
	virtual void visit(ast::Struct* structure) override;
	virtual void visit(ast::Function* funnction) override;
	virtual void visit(ast::Block* block) override;

	virtual void visit(ast::ReturnStmt* expr) override;

	virtual void visit(ast::LoadExpr* expr) override;
	virtual void visit(ast::OperandExpr* expr) override;
	virtual void visit(ast::CallExpr* expr) override;
	virtual void visit(ast::ArrayAccessExpr* expr) override;
	virtual void visit(ast::CastExpr* expr) override;

private:
	ModuleCompiler* m_module_compiler;
};


class ModuleCompiler {
public:
	static ModuleCompiler* Create(Compiler* compiler, Module* module);
	~ModuleCompiler() {}

	void parse_and_link_internals();
	void infer_types_and_do_semantic_analysis();

	ast::Scope* scope() { return m_scope; }
	Source* source() { return &m_source; }
	const Source* source() const { return &m_source; }
	Compiler* compiler() const { return m_compiler; }
	Module* module() const { return m_module; }
	CompilerAllocator* allocator() { return &m_allocator; }

	void add_link(ast::LoadExpr* load_expr, ast::Scope* scope, const Token& name);

	void import(Module* module, const Token& as = Token());
	void import_from(Module* module, const Token& identifier, const Token& as = Token());
	void import_everything(Module* module);

	CompilerError* raise_error();
	bool encountered_error() const { return !m_errors.empty(); }
	void print_errors(std::ostream& stream);

private:
	ModuleCompiler(Module* module, Compiler* compiler);

	Source m_source;
	ast::Scope* m_scope;
	Module* m_module;
	Compiler* m_compiler;

	CompilerAllocator m_allocator;

	std::vector<Link> m_unresolved_links;
	std::vector<CompilerError*> m_errors;
};

 
class Compiler : public ast::Visitor
{
public:
	Compiler(Project* project, Runtime* runtime);
	~Compiler();

	void compile();

	ast::Scope* global_scope() { return m_global_scope; }
	CompilerAllocator* allocator() { return &m_allocator; }

	void add_link(Link* link);

	CompilerError* raise_error();
	void mark_encoutered_error() { m_encountered_errors = true; }
	bool encountered_error() { return m_encountered_errors; }
	void print_errors(std::ostream& stream);

	Module* resolve_module(const std::string& path);


private:
	std::unordered_map<Module*, ModuleCompiler*> m_module_to_module_compilers;
	std::vector<ModuleCompiler*> m_module_compilers;
	int m_module_compilers_index = 0;

	ast::Scope* m_global_scope;
	Project* m_project;
	Runtime* m_runtime;

	CompilerAllocator m_allocator;

	std::vector<Link> m_unresolved_links;

	bool m_encountered_errors;
	std::vector<CompilerError*> m_errors;
};


class FunctionCompiler : public ast::Visitor {
public:
	FunctionCompiler(ast::Function* function)
		: m_function(function)
	{}

	void compile();

	void visit(ast::Function* funnction) override;
	void visit(ast::Block* block) override;


	void visit(ast::IfStmt* expr) override;
	void visit(ast::ForStmt* expr) override;
	void visit(ast::WhileStmt* expr) override;
	void visit(ast::ReturnStmt* expr) override;

	void visit(ast::LoadExpr* expr) override;
	void visit(ast::ExprStmt* expr) override;
	void visit(ast::OperandExpr* expr) override;
	void visit(ast::CallExpr* expr) override;
	void visit(ast::ArrayAccessExpr* expr) override;
	void visit(ast::CastExpr* expr) override;

private:
	ast::Function* m_function;

	bool m_expression_returned_64_bits = false;
	std::vector<OpCode> m_opcodes;
};


#endif // COMPILER_H