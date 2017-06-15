#ifndef ERROR_H
#define ERROR_H
#include "tokenizer.h"

#include <string>
#include <vector>

class Node;
class Project;
class Module;

class Error;
class ErrorContext;


class Error
{
public:
	Error(ErrorContext* context);

	ErrorContext* context() const { return m_context; }

	virtual std::string stringify() const = 0;

protected:

	virtual ~Error()
	{}


	ErrorContext* m_context;


	Error(const Error& other) = delete;
	Error& operator=(const Error& other) = delete;

	friend class ErrorContext;
};


class SyntaxError : public Error
{
public:
	SyntaxError(ErrorContext* context, Module* module, const Token& token)
		: Error(context), m_module(module), m_token(token)
	{}	

	virtual std::string stringify() const override;

private:
	virtual ~SyntaxError() {}

	Token m_token;
	Module* m_module;
};


class LinkerError : public Error
{
public:
	LinkerError(ErrorContext* context, Module* module, const Token& name, Node* container = nullptr)
		: Error(context), m_module(module), m_name(name), m_container(container)
	{}

	virtual std::string stringify() const override;

private:
	virtual ~LinkerError() {}

	Token m_name;
	Node* m_container;
	Module* m_module;
};


class ErrorContext
{
public:
	typedef std::vector<Error*>::const_iterator error_iterator;

	ErrorContext(Project* project)
		: m_project(project)
	{}

	error_iterator begin() const { return m_errors.begin(); }
	error_iterator end() const { return m_errors.end(); }

	bool no_errors() const { return m_errors.size() == 0; }

	Project* project() const { return m_project; }

private:
	std::vector<Error*> m_errors;
	Project* m_project;

	friend class Error;
};


#endif // ERROR_H