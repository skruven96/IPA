#ifndef PROJECT_H
#define PROJECT_H

#include <queue>
#include <string>
#include <map>

class Module;
struct Scope;
class Token;
class Source;

struct Settings {
	std::vector<std::string> source_files;
	std::vector<std::string> source_folders;

	bool print_tokens = true;
	bool print_ast    = true;
	bool print_opcodes = false;
};


/*
 */
class Project
{
public:
	typedef std::map<std::string, Module*>::const_iterator modules_iterator;

	Project(int argc, char* argv[]);
	~Project();

	Module* get_or_create_module(const std::string& path);
	Module* get_module(const std::string& path) const;

	modules_iterator modules_begin() const { return m_modules.begin(); }
	modules_iterator modules_end()   const { return m_modules.end();   }

	const Settings& settings() const { return m_settings; }

private:

	bool m_encountered_error = false;

	std::map<std::string, Module*> m_modules;

	Settings m_settings;

	Scope* m_global_scope;
};


/*
 */
class Module
{
public:
	Module(Project* project, const std::string& path);

	Project* project() const { return m_project; }

	const std::string& path() const { return m_path; }

private:

	// Can't copy modules
	Module(const Module&) = delete;
	Module& operator=(const Module&) = delete;

	Project* m_project;
	std::string m_path;
};


#endif // PROJECT_H