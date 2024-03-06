#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include <atomic>

#include "./Taskpool.hpp"
#include "./Task.hpp"

class Worker
{
public:
	enum Status
	{
		NONE = 0, IDLE, BUSY, EXIT
	};
public:
	Worker(int id, Taskpool &taskpool): m_id(id)
	{
		m_threadLoop = std::thread([this, &taskpool]()
			{
				while (true)
				{
					if (m_isExited)
						break;

					m_state = IDLE;
					while (taskpool.isEmpty())
					{
						if (m_isExited)
							break;

						std::this_thread::sleep_for(std::chrono::microseconds(100));
					}

					TaskSharePtr task = taskpool.pop();
					if (task == nullptr)
						continue;

					m_state = BUSY;

					printf("Worker %d scheduling task %d \n", m_id, task->getID());
					// std::cout << "Worker " << m_id << " scheduling task " << task->getID() << std::endl;
					task->Run();
					printf("Worker %d finish task %d \n", m_id, task->getID());
					// std::cout << "Worker " << m_id << " finish task " << task->getID() << std::endl;
				}

				m_state = EXIT;
			});
		printf("Worker %d started. \n", m_id);
		// std::cout << "Worker " << id << " started." << std::endl;
	}

	inline void exit()
	{
		m_isExited = true;
	}

	std::thread::id gettid()
	{
		return m_threadLoop.get_id();
	}

	Status getState()
	{
		return m_state;
	}

	~Worker() noexcept
	{
		printf("Worker %d exit.", m_id);
		// std::cout << "Worker " << m_id << " exit." << std::endl;
		if (m_threadLoop.joinable())
			m_threadLoop.join();
	}

private:
	std::atomic<Status> m_state = IDLE;
	int m_id = 0;
	std::atomic_bool m_isExited = false;
	std::thread m_threadLoop;
};

using WorkerSharePtr = std::shared_ptr<Worker>;
