#ifndef HANDLES_H
#define HANDLES_H
#include "types.h"

#include <vector>


template<typename T>
class Handle
{
public:
	Handle()
		: m_ptr(nullptr)
	{}

	Handle(T* ptr)
		: m_ptr(ptr)
	{
		if (m_ptr != nullptr)
			m_ptr->m_visible_uses.push_back(this);
	}

	Handle(const Handle<T>& other)
		: m_ptr(other.m_ptr)
	{
		if (m_ptr != nullptr)
			m_ptr->m_visible_uses.push_back(this);
	}

	void operator=(T* ptr)
	{
		clear();
		m_ptr = ptr;
		if (m_ptr != nullptr)
			m_ptr->m_visible_uses.push_back(this);
	}

	void operator=(const Handle<T>& other)
	{
		clear();
		m_ptr = other.m_ptr;
		if (m_ptr != nullptr)
			m_ptr->m_visible_uses.push_back(this);
	}

	void clear()
	{
		if (m_ptr != nullptr)
		{
			auto it = std::find(m_ptr->users_begin(), m_ptr->users_end(), this);
			m_ptr->m_visible_uses.erase(it);
			m_ptr = nullptr;
		}
	}

	T* operator*() const { return m_ptr; }
	T* operator->() const { return m_ptr; }

private:
	T* m_ptr;
};


template<typename T>
class Usable
{
public:
	typedef typename std::vector<Handle<T>*>::iterator user_iterator;

	user_iterator users_begin() { return m_visible_uses.begin(); }
	user_iterator users_end() { return m_visible_uses.end(); }
	u32 num_uses() const { return users_end() - users_begin(); }

protected:
	virtual ~Usable() {}

private:
	std::vector<Handle<T>*> m_visible_uses;

	friend class Handle<T>;
};



#endif // HANDLES_H