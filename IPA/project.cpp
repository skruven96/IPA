#include "project.h"

#include "compiler.h"

#include <iostream>


Project::Project(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		m_settings.source_files.push_back(argv[i]);
	}


	for (auto path : m_settings.source_files) {
		get_or_create_module(path);
	}
}

Project::~Project() {}

Module* Project::get_or_create_module(const std::string& name) {
	Module* result;
	auto it = m_modules.find(name);
	if (it == m_modules.end()) {
		result = new Module(this, name);
		m_modules[name] = result;
	} else {
		result = it->second;
	}

	return result;
}



// =========================================================================================================
// Module
// =========================================================================================================

Module::Module(Project* project, const std::string& path)
	:  m_project(project), m_path(path) {

}
