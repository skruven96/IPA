#include "server_compiler.h"
#include "nodes.h"
#include "stmt.h"


const OpCode operator+(const OpCode lhs, const OpTypeSpecificOffsets rhs)
{
	return static_cast<const OpCode>(static_cast<const u16>(lhs) + static_cast<const u16>(rhs));
}
const OpCode operator+(const OpCode lhs, const OpSizeSpecificOffsets rhs)
{
	return static_cast<const OpCode>(static_cast<const u16>(lhs) + static_cast<const u16>(rhs));
}


void ServerCompiler::compile()
{
	// Determine the address of all static functions in the heap
	for (auto it = m_project->recursive_iterate<Function>(); !it.reached_end(); ++it)
	{
		if ((*it)->is_static())
		{
			m_stack_size += m_stack_size % (*it)->type()->size();

			(*it)->address(m_stack_size);

			m_stack_size += (*it)->type()->size();
		}
	}

	// Determine the address of all static variables in the heap
	for (auto it = m_project->recursive_iterate<Variable>(); !it.reached_end(); ++it)
	{
		if ((*it)->stored_on_heap())
		{
			m_stack_size += m_stack_size % (*it)->type()->size();

			(*it)->address(m_stack_size);

			m_stack_size += (*it)->type()->size();

		}
	}

	// Determine argument address for all functions
	for (auto it = m_project->recursive_iterate<Function>(); !it.reached_end(); ++it)
		determine_argument_addresses(*it);


	// Compile all functions
	for (auto it = m_project->recursive_iterate<Function>(); !it.reached_end(); ++it)
	{
		ServerFunctionCompiler compiler(*it);
		compiler.compile();
	}
}


void ServerCompiler::determine_argument_addresses(Function* function) const
{
	u16 offset = 0;
	for (auto arg = function->arguments_begin(); arg != function->arguments_end(); ++arg)
	{
		u32 size = (*arg)->type()->size();
		offset += offset % size;

		(*arg)->address(offset);

		offset += size;
	}
}


void ServerFunctionCompiler::compile()
{
	for (auto arg = m_function->arguments_begin(); arg != m_function->arguments_end(); ++arg)
	{
		u16 arg_offset = push_register(*arg);
		assert(arg_offset == (*arg)->address() && "Miss match between predetermined address offset and pushed register. ");
	}

	compile_block(m_function->block());

	if (m_function->return_type() == m_function->project()->void_t())
		m_opcodes.push_back(Op(OpRETURN_VOID));

	u16 padding = m_stack_uses.size() % 8;
	while (padding != 0)
	{
		m_stack_uses.push_back(false);
		--padding;
	}

	// Modify argument addresses so they get stored at the end of the stack. 
	for (auto it = m_argument_stores.begin(); it != m_argument_stores.end(); ++it)
	{
		Op& op = m_opcodes[it->m_operation_index];
		if (it->m_is_c)
			op.m_c = m_stack_uses.size() + it->m_argument->address();
		else
			op.m_a = m_stack_uses.size() + it->m_argument->address();
	}

	// Store it in a sensible way.
	m_function->m_opcode = OpCodes(m_function, m_stack_uses.size(), m_opcodes, m_constants_data);
}


void ServerFunctionCompiler::compile_block(Block* block)
{
	for (auto stmt = block->stmts_begin(); stmt != block->stmts_end(); ++stmt)
	{
		switch ((*stmt)->node_type())
		{
		case(Node::ControlFlowStmtType):
			compile_control_flow_stmt(Node::FastCast<ControlFlowStmt>(*stmt));
			break;
		case(Node::OperandStmtType):
			compile_operand(Node::FastCast<OperandStmt>(*stmt));
			break;
		case(Node::CallStmtType):
			compile_call(Node::FastCast<CallStmt>(*stmt));
			break;
		default:
			assert(false && "Uncompileable node. ");
		}
	}
}


void ServerFunctionCompiler::compile_control_flow_stmt(ControlFlowStmt* stmt)
{
	
	if (Value* return_value = stmt->value())
	{
		Op op(size_code(m_function->return_type()->size()) + OpxRETURN);
		op.m_a = read_value(return_value, m_function->return_type());
		m_opcodes.push_back(op);
	}
	else
	{
		m_opcodes.push_back(Op(OpRETURN_VOID));
	}
}


void ServerFunctionCompiler::compile_operand(OperandStmt* operand)
{
	Op op;
	Type* term_type = operand->term_type();
	Value* target = operand->target();

	if (operand->is_unary_prefix()) // !, -, ~, ++, --
	{
		Value* rhs = operand->rhs();

		switch (operand->operand().operand())
		{
		case(Token::NotOperand):
			op = Op(size_code(term_type->size()) + OpxNOT);
			op.m_b = read_value(rhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		case(Token::SubOperand):
			op = Op(type_code(term_type) + OpxSUB);
			op.m_b = read_value(Constant::Integer(m_function->block(), 0), term_type);
			op.m_c = read_value(rhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		case(Token::BinaryNotOperand):
			op = Op(size_code(term_type->size()) + OpxBNOT);
			op.m_b = read_value(rhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		case(Token::IncrementOperand):
			op = Op(size_code(term_type->size()) + OpxINC);
			op.m_b = read_value(rhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		case(Token::DecrementOperand):
			op = Op(size_code(term_type->size()) + OpxINC);
			op.m_b = read_value(rhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		default:
			assert(false && "Not a unary prefix operand. ");
		}
	}
	else if (operand->is_unary_postfix()) // ++, --
	{
		Value* lhs = operand->lhs();

		switch (operand->operand().operand())
		{
		case(Token::IncrementOperand):
			op = Op(size_code(term_type->size()) + OpxINC);
			op.m_b = read_value(lhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		case(Token::DecrementOperand):
			op = Op(size_code(term_type->size()) + OpxINC);
			op.m_b = read_value(lhs, term_type);

			m_opcodes.push_back(op);
			m_opcodes.back().m_a = write_value(target, term_type);
			break;
		default:
			assert(false && "Not a unary prefix operand. ");
		}
	}
	else
	{
		Value* lhs = operand->lhs();
		Value* rhs = operand->rhs();

		switch (operand->operand().operand())
		{
		case Token::SetOperand:
			if (target->stored_on_stack())
				op = Op(size_code(lhs->type()->size()) + OpxSET_LOCAL);
			else if (target->stored_on_heap())
				op = Op(size_code(lhs->type()->size()) + OpxSET_LOCAL);
			else
				assert(false && "Can't set a non heap or stack value. ");

			op.m_a = read_value(rhs, lhs->type());

			m_opcodes.push_back(op);
			m_opcodes.back().m_b = write_value(target, lhs->type());
			return;
		case Token::AddOperand:
		case Token::AddSetOperand:
			op = Op(type_code(term_type) + OpxADD);
			break;
		case Token::SubOperand:
		case Token::SubSetOperand:
			op = Op(type_code(term_type) + OpxSUB);
			break;
		case Token::MulOperand:
		case Token::MulSetOperand:
			op = Op(type_code(term_type) + OpxMUL);
			break;
		case Token::DivOperand:
		case Token::DivSetOperand:
			op = Op(type_code(term_type) + OpxDIV);
			break;
		case Token::ModOperand:
		case Token::ModSetOperand:
			op = Op(type_code(term_type) + OpxMOD);
			break;
		case Token::LtOperand:
			op = Op(type_code(term_type) + OpxLT);
			break;
		case Token::GtOperand:
			op = Op(type_code(term_type) + OpxGT);
			break;
		case Token::BinaryAndOperand:
			op = Op(size_code(term_type->size()) + OpxBAND);
			break;
		case Token::BinaryOrOperand:
			op = Op(size_code(term_type->size()) + OpxBOR);
			break;
		case Token::BinaryXorOperand:
			op = Op(size_code(term_type->size()) + OpxBXOR);
			break;
		case Token::EqualsOperand:
			op = Op(size_code(term_type->size()) + OpxEQ);
			break;
		case Token::NotEqualsOperand:
			op = Op(size_code(term_type->size()) + OpxNEQ);
			break;
		case Token::LesserEqualsOperand:
			op = Op(type_code(term_type) + OpxLE);
			break;
		case Token::GreaterEqualsOperand:
			op = Op(type_code(term_type) + OpxGE);
			break;
		case Token::BinaryLShiftOperand:
			op = Op(size_code(term_type->size()) + OpxBLSHIFT);
			break;
		case Token::BinaryRShiftOperand:
			op = Op(size_code(term_type->size()) + OpxBRSHIFT);
			break;
		default:
			assert(false && "Not a valid operand. ");
		}

		op.m_b = read_value(lhs, term_type);
		op.m_c = read_value(rhs, term_type);

		m_opcodes.push_back(op);
		m_opcodes.back().m_a = write_value(target, term_type);
	}
}


void ServerFunctionCompiler::compile_call(CallStmt* stmt)
{
	for (auto it = stmt->arguments_begin(); it != stmt->arguments_end(); ++it)
	{
		Value* argument = **it;


	}

	Op op(OpCALLLocal);
	op.m_b = read_value(stmt->callable(), stmt->callable()->type());

	m_opcodes.push_back(op);
	m_opcodes.back().m_a = write_value(stmt->target(), m_function->project()->i32_t());
}


u16 ServerFunctionCompiler::push_register(Value* value)
{
	u32 type_size = value->type()->size();
	u16 padding = m_stack_uses.size() % type_size;
	
	while (padding != 0)
	{
		m_stack_uses.push_back(false);
		--padding;
	}

	u16 address = m_stack_uses.size();

	while (type_size != 0)
	{
		m_stack_uses.push_back(true);
		--type_size;
	}

	if (value->stored_on_stack())
		value->address(address);

	Register reg;
	reg.m_address = address;
	m_registers[value] = reg;

	return address;
}


u16 ServerFunctionCompiler::read_value(Value* value, Type* target_type)
{
	u16 address = 0;
	if (value->stored_on_stack())
	{
		auto register_iterator = m_registers.find(value);
		if (register_iterator != m_registers.end()) // Already on the stack use that. 
			address = register_iterator->second.m_address;
		else
		{
			// Not found create a new register. 
			address = push_register(value);
		}
	}
	else if (value->stored_on_heap())
	{
		m_opcodes.push_back(Op(size_code(value->type()->size()) + OpxLOAD_STATIC));
		m_opcodes.back().m_a = push_register(value);
		m_opcodes.back().m_bx = value->address();
		address = m_opcodes.back().a();
	}
	else if (value->stored_in_constants())
	{
		m_opcodes.push_back(Op(size_code(value->type()->size()) + OpxLOAD_CONSTANT));
		m_opcodes.back().m_a = push_register(value);
		// Determine constant offset. 
		address = m_opcodes.back().a();
	}
	else
		assert(false && "Value can't be read. ");

	if (value->type() != target_type) // Cast operation
	{
		assert(false && "Cast operation. ");
		return 0; // Return register containing the converted type. 
	}
	else
		return address;
}


u16 ServerFunctionCompiler::write_value(Value* target, Type* value_type)
{
	u16 val_address = 0; ///< Where the value to be written is stored.
	u16 target_address = 0; ///< Where converted value will be stored if types differ.

	if (target->stored_on_stack())
	{
		val_address = read_value(target, target->type());
	}
	else if (target->stored_on_heap())
	{
		assert(false && "TODO");
		// TODO: Find temporary register to be freed at the end of this function. 
	}
	else
		assert(false && "Can't write value. ");

	if (target->type() != value_type)
	{
		assert(false && "Cast operation. ");
	}
	else
	{
		target_address = val_address;
	}

	if (target->stored_on_heap())
	{
		Op op(size_code(target->type()->size()) + OpxSET_STATIC);
		op.m_a = target_address;
		op.m_bx = target->address();
		m_opcodes.push_back(op);
	}
	
	return val_address;
}


OpCode ServerFunctionCompiler::type_code(Type* type)
{
	switch (type->primitive())
	{
	case(Token::BoolPrimitive):
		return U8Codes;

	case(Token::I8Primitive):
		return I8Codes;
	case(Token::U8Primitive):
		return U8Codes;
	
	case(Token::I16Primitive):
		return I16Codes;
	case(Token::U16Primitive):
		return U16Codes;

	case(Token::I32Primitive):
		return I32Codes;
	case(Token::U32Primitive):
		return U32Codes;

	case(Token::I64Primitive):
		return I64Codes;
	case(Token::U64Primitive):
		return U64Codes;

	case(Token::F32Primitive):
		return F32Codes;
	case(Token::F64Primitive):
		return F64Codes;

	default:
		assert(false && "Illegal primitive code. ");
	}
}


OpCode ServerFunctionCompiler::size_code(u32 size)
{
	switch (size)
	{
	case(1):
		return Size8Codes;
	case(2):
		return Size16Codes;
	case(4):
		return Size32Codes;
	case(8):
		return Size64Codes;
	default:
		assert(false && "Illegal size code. ");
	}
}

