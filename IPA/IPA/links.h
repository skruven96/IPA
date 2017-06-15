#ifndef LINK_H
#define LINK_H
#include "nodes.h"
#include "stmt.h"


class Module;


class UnresolvedLink
{
public:

	virtual void resolve() = 0;

protected:
	UnresolvedLink(Module* module, Node* start)
		: m_module(module), m_start(start)
	{
		module->m_unresolved_links.push_back(this);
	}
	virtual ~UnresolvedLink() {}

	Module* m_module;
	Node* m_start;

	friend class Module;
};


class UnresolvedTypeLink : public UnresolvedLink, public Type
{
public:
	static UnresolvedTypeLink* Create(Module* module, Node* start, const std::vector<Token>& path);

	void resolve() override;

private:
	UnresolvedTypeLink(Module* module, Node* start, const std::vector<Token>& path)
		: UnresolvedLink(module, start), Type("<unresolved>", start, -1, Token::NoPrimitive, Node::StaticFlag), m_path(path)
	{}

	std::vector<Token> m_path;

	UnresolvedTypeLink(const UnresolvedTypeLink& other) = delete;
	UnresolvedTypeLink& operator=(const UnresolvedTypeLink& rhs) = delete;
};


class UnresolvedValueLink : public UnresolvedLink, public Value
{
public:
	static UnresolvedValueLink* Create(Module* module, Node* start, const std::vector<Token>& path);

	void resolve() override;

private:
	UnresolvedValueLink(Module* module, Node* start, const std::vector<Token>& path)
		: UnresolvedLink(module, start), Value(nullptr, Value::UnkownLocation), m_path(path)
	{}

	std::vector<Token> m_path;

	UnresolvedValueLink(const UnresolvedValueLink& other) = delete;
	UnresolvedValueLink& operator=(const UnresolvedValueLink& rhs) = delete;
};


/* /brief Linker links types and values to each other and also infers types to operands or where types is not specified. 
 * 
 * 
 */
class Linker
{
public:
	Linker(Project* project)
		: m_project(project)
	{}

	void link_all();

private:
	Project* m_project;
};


#endif // LINK_H