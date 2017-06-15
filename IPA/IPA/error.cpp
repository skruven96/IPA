#include "error.h"
#include "nodes.h"


Error::Error(ErrorContext* context)
	: m_context(context)
{
	context->m_errors.push_back(this);
	context->project()->thrown_error();
}


std::string SyntaxError::stringify() const
{
	std::string str("Line ");
	str += std::to_string(m_token.start().row(m_module->source()));
	str += ": Unexpected ";
	str += m_token.type();
	str += " ";
	str += m_token.value(m_module->source());
	str += ".";
	return str;
}


std::string LinkerError::stringify() const
{
	
	std::string str("Line ");
	str += std::to_string(m_name.start().row(m_module->source()));
	str += ": ";
	
	if (m_container == nullptr)
	{
		str += "Couldn't find " + m_name.value(m_module->source());
	}
	else
	{
		str += m_container->name() + " don't contain any element called " + m_name.value(m_module->source());
	}

	str += ".";
	return str;
}

