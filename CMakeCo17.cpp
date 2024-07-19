//
// Created by benny on 2022/3/17.
//
#include <experimental/coroutine>

#include <iostream>
#include "Messageloop.h"
#include "task.hpp"

using namespace cppcoro;
using namespace std;


template<typename OPERATION>
class base_awaitable
{
protected:
	base_awaitable() noexcept
	{}

	~base_awaitable() noexcept
	{
		//协程对象不应该有 base_awaitable 管理，注释部分是错误写法
		//if (m_awaitingcoroutine)
		//{
		//	m_awaitingcoroutine.destroy();
		//}
	}
public:

	bool await_ready() const noexcept { return false; }

	bool await_suspend(std::experimental::coroutine_handle<> awaitingCoroutine)
	{
		m_awaitingCoroutine = awaitingCoroutine;
		return static_cast<OPERATION*>(this)->try_start();
	}

	decltype(auto) await_resume()
	{
		return static_cast<OPERATION*>(this)->get_result();
	}

	void completed() {
		m_awaitingCoroutine.resume();
	}

private:
	std::experimental::coroutine_handle<> m_awaitingCoroutine;

};

class download
	: public base_awaitable<download>
{
public:
	download(std::string url)
		: m_url(std::move(url))
	{}


	bool try_start()
	{
		MessageLoop::Post(1, [this]() {
			completed();
			});

		return true;
	}

	std::string get_result()
	{
		return m_url + "#L#";
	}

private:
	std::string m_url;

};

task<> simple_task() {

	auto result1 = co_await download("www.baidu.com");
	std::cout << result1 << std::endl;

	auto result2 = co_await download(result1);
	std::cout << result2 << std::endl;

	auto result3 = co_await download(result2);
	std::cout << result3 << std::endl;

	auto result4 = co_await download(result3);
	std::cout << result4 << std::endl;

	auto result5 = co_await download(result4);
	std::cout << result5 << std::endl;
}


task<>g_task2;
void doSomthing()
{
	g_task2 = simple_task();
}

task<>g_task;
int main() {

	g_task = simple_task();

	doSomthing();

	MessageLoop::Run();

	return 0;
}

