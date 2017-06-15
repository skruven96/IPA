#include "links.h"


UnresolvedTypeLink* UnresolvedTypeLink::Create(Module* module, Node* start, const std::vector<Token>& path)
{
	return new UnresolvedTypeLink(module, start, path);
}


void UnresolvedTypeLink::resolve()
{
	for (auto it = m_path.begin() + 1; it != m_path.end(); ++it)
	{
		
	}
}


UnresolvedValueLink* UnresolvedValueLink::Create(Module* module, Node* start, const std::vector<Token>& path)
{
	return new UnresolvedValueLink(module, start, path);
}


void UnresolvedValueLink::resolve()
{
	Node* node = nullptr;
	Node* container = m_start;

	auto it = m_path.begin();

	while (true)
	{
		node = container->child((*it).value(m_module->source()));

		if (node != nullptr) break;

		if (Node* parent = container->parent())
			container = parent;
		else
			break;
	}

	if (node == nullptr)
		throw new LinkerError(m_module->error_context(), m_module, (*it), container);

	Value* value = nullptr;

	switch (node->node_type())
	{
	case(Node::VariableType):
		value = static_cast<Variable*>(node);
		break;
	case(Node::FunctionType):
		value = static_cast<Function*>(node);
		break;
	default:
		throw new LinkerError(m_module->error_context(), m_module, (*it));
	}

	while (users_begin() != users_end())
		**users_begin() = value;
}


void Linker::link_all()
{
	// Resolve imports


	// Resolve links
	for (auto it = m_project->iterate<Module>(); !it.reached_end(); ++it)
		it->resolve_links();


	// Resolve types
	for (auto it = m_project->recursive_iterate<OperandStmt>(); !it.reached_end(); ++it)
	{
		if (it->is_unary_prefix())
			it->m_term_type = it->rhs()->type();
		else if (it->is_unary_prefix())
			it->m_term_type = it->lhs()->type();
		else
		{
			Type* lhs_type = it->lhs()->type();
			Type* rhs_type = it->rhs()->type();

			if (lhs_type == rhs_type)
			{
				it->m_term_type = lhs_type;
				if (it->target()->type() == nullptr)
					it->target()->type(lhs_type);
			}
			else
				assert(false && "Determine term with biggest persision. ");
		}
	}

	for (auto it = m_project->recursive_iterate<CallStmt>(); !it.reached_end(); ++it)
	{
		if (it->target()->type() == nullptr)
			it->target()->type(m_project->i32_t()); // HACK: Take the return type of
	}
}

