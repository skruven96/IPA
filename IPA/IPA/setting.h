#ifndef SETTING_H
#define SETTING_H
#include <string>
#include <vector>


class Project;


class Setting
{
public:
	static Setting* Create(Project* project, const std::string& name, const std::string& initial = "");
	static Setting* Create(Project* project, const std::string& name, const std::vector<std::string>& initial);
	static Setting* Get(Project* project, const std::string& name);

	const std::string& name() const { return m_name; }

	

private:
	Setting(const std::string& name, const std::string& initial)
		: m_name(name)
	{
		if (!initial.empty())
			m_values.push_back(initial);
	}

	Setting(const std::string& name, const std::vector<std::string>& initial)
		: m_name(name), m_values(initial)
	{}

	// This class should not be copied
	Setting(const Setting&) = delete;
	Setting& operator=(const Setting&) = delete;

	std::string m_name;
	std::vector<std::string> m_values;
};


#endif // SETTING_H
