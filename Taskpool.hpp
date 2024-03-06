#pragma once
#include <queue>
#include <mutex>

#include "./Task.hpp"

class Taskpool
{
public:
	inline bool isEmpty()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_tasks.empty();
	}

	TaskSharePtr pop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_tasks.empty())
			return nullptr;

		TaskSharePtr task = m_tasks.front();
		m_tasks.pop();

		return task;
	}

	inline void push(TaskSharePtr task)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_tasks.push(task);
	}

	size_t size()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_tasks.size();
	}
private:
	std::queue<TaskSharePtr> m_tasks;
	std::mutex m_mutex;
};
