#ifndef RUNTIME_H
#define RUNTIME_H
#include "nodes.h"


/* \brief Runtime handles the stack and heap. 
 * Two runtimes can be used for the same project but will not share any managed data.
 */
class Runtime
{
public:
	Runtime(Project* project)
		: m_project(project)
	{
		m_stack = new u8[4 * 200];
		m_stack_ptr = m_stack;
		m_stack_end = m_stack + 4 * 200;

		m_heap = new u8[4 * 200];
		m_heap_ptr = m_heap;
		m_heap_end = m_heap + 4 * 200;
	}
	~Runtime()
	{}

	// Initializes all global variables and runs all decorators. 
	void initialize();

	Runtime& start_call(Function* function);
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

	void call(void* return_value = nullptr, Type* return_type = nullptr);
	void call(void* return_value, OpCodes codes);

private:
	Project* m_project;

	Function* m_call;
	Function::argument_iterator m_argument;

	u8* m_stack;
	u8* m_stack_ptr;
	u8* m_stack_end;

	u8* m_heap;
	u8* m_heap_ptr;
	u8* m_heap_end;
};


#endif // RUNTIME_H