#pragma once
#include <memory>
#include <functional>

class Task
{
public:
	Task(int id = 0): m_id(id){}
	virtual void Run() = 0; // 运行入口

	inline int getID() { return m_id; }
private:
	int m_id = 0;
};

template<typename Func>
class MyTask : public Task
{
public:
	MyTask(int id, std::function<void()> func) :Task(id), m_func(func) {}

	void Run() override
	{
		m_func();
	}
private:
	std::function<void()> m_func;
};

using TaskSharePtr = std::shared_ptr<Task>;
