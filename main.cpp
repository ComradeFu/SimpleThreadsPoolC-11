#include <iostream>
#include <functional>
#include <chrono>

#include "./Task.hpp"
#include "./ThreadPool.hpp"

int main()
{
	ThreadPool pool;

	for (int id = 0; id < 10; ++id)
	{
		// ������ɧ����
		TaskSharePtr task = std::make_shared<MyTask<void()>>(id, [id]()
			{
				std::this_thread::sleep_for(std::chrono::seconds(id));
			});
		pool.addTask(task);
	}

	pool.Run();

	{
		// ������Բ����ӳ٣�����Ϊ 0s��5s ���в�ͬ����
		std::this_thread::sleep_for(std::chrono::seconds(30));
	}

	pool.quit();

	return 0;
}
