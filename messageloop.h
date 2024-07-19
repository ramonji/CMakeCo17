#pragma once

#include <functional>
#include <queue>
#include <string>
//#include <iostream>
#include <thread>
#include <mutex>

//template<typename ResultType>
class MessageLoop {

	public:
	//注册回调函数
	static void Post(std::function<void(void)> task) {
		std::unique_lock lock(completion_lock);
		sTaskQueue.push(task);
	}

	static void Post(int nSecond, std::function<void(void)> task) {

		std::thread([nSecond, task]() {
			std::this_thread::sleep_for(std::chrono::seconds(nSecond));
			std::unique_lock lock(completion_lock);
			sTaskQueue.push(task);
		}).detach();

		//std::unique_lock lock(completion_lock);
		//sTaskQueue.push(task);
	}


	//分发回调函数
	static void Run() {
		//std::unique_lock lock(completion_lock);
		while (true)
		{
			if(sTaskQueue.size()>0)
			{
				//取出队列中的回调函数
				std::function<void(void)> task = sTaskQueue.front();

				completion_lock.lock();
				sTaskQueue.pop();
				completion_lock.unlock();

				//执行回调函数
				task();
			}

			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	}

private:
	//等待分发的回调队列
	static std::queue<std::function<void(void)>> sTaskQueue;
	static std::mutex completion_lock;

};

std::queue<std::function<void(void)>> MessageLoop::sTaskQueue;
std::mutex MessageLoop::completion_lock;