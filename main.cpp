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
		// 下面是骚操作
		TaskSharePtr task = std::make_shared<MyTask<void()>>(id, [id]()
			{
				std::this_thread::sleep_for(std::chrono::seconds(id));
			});
		pool.addTask(task);
	}

	pool.Run();

	{
		// 这里可以测试延迟，设置为 0s、5s 各有不同表现
		std::this_thread::sleep_for(std::chrono::seconds(30));
	}

	pool.quit();

	return 0;
}
