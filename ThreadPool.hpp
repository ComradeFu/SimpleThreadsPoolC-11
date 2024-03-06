#pragma once
#include <thread>
#include <iostream>
#include <unordered_map>
#include <atomic>

#include "./Worker.hpp"

class ThreadPool
{
public:
	enum State
	{
		NONE = 0,
		IDLE,
		BUSY,
	};
public:
	ThreadPool(int minWorkerCount = 2, int maxWorkerCount = 10)
		: m_minWorkerCount(minWorkerCount),
		m_maxWorkerCount(maxWorkerCount)
	{
		for (int i = 0; i < minWorkerCount; ++i)
			IncWorker();
	}

	void Run()
	{
		m_mainThread = std::thread([this]
			{
				while (true)
				{
					// 等待队列执行完
					if (m_isQuit && m_taskpool.isEmpty())
						break;

					if (m_isQuit)
						continue;

					// 如果队列等待超过一倍，开多点线程
					if (m_taskpool.size() > m_currWorkerCount * 2 && m_currWorkerCount < m_maxWorkerCount)
						IncWorker();

					// 如果队列空闲太多，少点线程
					if (m_taskpool.size() == 0 && m_currWorkerCount > m_minWorkerCount)
						DecWorker();
				}
			});
	}

	void addTask(TaskSharePtr task)
	{
		if (m_isQuit)
			return;

		m_taskpool.push(task);
	}

	void quit()
	{
		m_isQuit = true;
		printf("thread pool quit. \n");
	}

	~ThreadPool() noexcept
	{
		if(m_mainThread.joinable())
			m_mainThread.join();

		for (auto keypair : m_workers)
			keypair.second->exit();
	}
private:
	// 增加一个线程
	void IncWorker()
	{
		WorkerSharePtr worker = std::make_shared<Worker>(nexttid(), m_taskpool);
		m_workers[worker->gettid()] = worker;

		++m_currWorkerCount;
	}
	// 减少一个线程
	void DecWorker()
	{
		for(const auto &keypair: m_workers)
			if (keypair.second->getState() == Worker::Status::IDLE)
			{
				keypair.second->exit();
				m_workers.erase(keypair.first);

				--m_currWorkerCount;
				break;
			}
	}
	
	int nexttid()
	{
		return ++m_tidHelper;
	}

private:
	int m_minWorkerCount;
	int m_maxWorkerCount;
	int m_currWorkerCount = 0;
	int m_tidHelper = 0;

	std::atomic_bool m_isQuit = false;

	std::thread m_mainThread;
	std::unordered_map<std::thread::id, WorkerSharePtr> m_workers;
	Taskpool m_taskpool;
};
