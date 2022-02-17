#ifndef RUNTIME_H
#define RUNTIME_H
#include "common.h"

namespace ast {
	struct Type;
	struct Callable;
	struct Function;
}
class Project;

/* \brief Runtime handles the stack and heap. 
 * Two runtimes can be used for the same project but will not share any managed data.
 */
class Runtime
{
public:
	Runtime(Project* project)
		: m_project(project) {
		m_stack = new i32[4 * 200];
		m_stack_ptr = m_stack;
		m_stack_end = m_stack + 4 * 200;

		m_heap = new i64[4 * 200];
		m_heap_ptr = m_heap;
		m_heap_end = m_heap + 4 * 200;
	}
	~Runtime()
	{}

	// Initializes all global variables and runs all decorators. 
	void initialize();

	Runtime& start_call(ast::Function* function);
	Runtime& arg(u8 value);
	Runtime& arg(i8 value);
	Runtime& arg(u16 value);
	Runtime& arg(i16 value);
	Runtime& arg(u32 value);
	Runtime& arg(i32 value);
	Runtime& arg(u64 value);
	Runtime& arg(i64 value);
	Runtime& arg(f32 value);
	Runtime& arg(f64 value);
	// Runtime& arg(Object obj);

	void call(void* return_value = nullptr, ast::Type* return_type = nullptr);

private:
	Project* m_project;

	ast::Function* m_call;
	i32 m_current_argument_index;

	i32* m_stack;
	i32* m_stack_ptr;
	i32* m_stack_end;

	i64* m_heap;
	i64* m_heap_ptr;
	i64* m_heap_end;
};


#endif // RUNTIME_H