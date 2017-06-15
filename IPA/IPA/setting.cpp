#include "setting.h"
#include "project.h"

#include <assert.h>


Setting* Setting::Create(Project* project, const std::string& name, const std::string& initial)
{
	auto pos = project->m_settings.find(name);
	assert(pos == project->m_settings.end() && "Settings name is already taken. ");

	Setting* setting = new Setting(name, initial);
	project->m_settings[name] = setting;
	return setting;
}


Setting* Setting::Create(Project* project, const std::string& name, const std::vector<std::string>& initial)
{
	auto pos = project->m_settings.find(name);
	assert(pos == project->m_settings.end() && "Settings name is already taken. ");

	Setting* setting = new Setting(name, initial);
	project->m_settings[name] = setting;
	return setting;
}


Setting* Setting::Get(Project* project, const std::string& name)
{
	auto pos = project->m_settings.find(name);

	assert(pos != project->m_settings.end() && "Could no find a setting. ");

	return pos->second;
}


