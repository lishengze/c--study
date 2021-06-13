/*
 * thread_func.cpp
 *
 *  Created on: Sep 4, 2018
 *      Author: lsz
 */
#include "thread_func.h"
#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <string>
#include <chrono>
#include <future>
using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::thread;
using std::string;
using std::chrono::system_clock;
using std::chrono::seconds;
using std::chrono::duration_cast;
using std::launch;
using std::future;
using std::async;
using std::mutex;
using std::lock_guard;

mutex g_mutex;

void basic_thread_callback() 
{
	// cout << "basic_thread_callback" << endl;
	lock_guard<mutex> lock(g_mutex);
	cout << "thread id: " << std::this_thread::get_id() << endl;
}

void test_create_thread() 
{
	cout << "main thread start" << endl;

	int thread_numb = 5;
	list<thread*> threadList;
	for (int i = 0 ; i < thread_numb; ++i) {
		thread* tmpThread = new thread(basic_thread_callback);
		threadList.push_front(tmpThread);
	}

	for (thread* tmpThread:threadList) {
		tmpThread->join();
	}

	cout << "\nmain thread over" << endl;

	for (list<thread*>::iterator it = threadList.begin(); it != threadList.end(); ++it) {
		delete *it;
		*it = NULL;
	}
}

// class Wallet 
// {
// 	public:
// 		Wallet():m_money(0) {

// 		}

// 		void addMoney(int money) {
// 			// std::lock_guard<std::mutex> lockGurad(m_mutex);
// 			m_money += money;
// 		}

// 		int getMoney() {
// 			return m_money;
// 		}

// 	private:
// 		int m_money;	
// 		std::mutex m_mutex;	
// };

// int  getWalletMoney(int threadNumb, int addedMonedyOnce) {
// 	Wallet walletObj;

// 	list<thread> threadList;
// 	for (int i = 0; i < threadNumb; ++i) {
// 		threadList.push_back(thread(&Wallet::addMoney, &walletObj, addedMonedyOnce));
// 	}
// 	for (list<thread>::iterator it= threadList.begin(); it != threadList.end(); ++it) {
// 		it->join();
// 	}

// 	return walletObj.getMoney();
// }

// void test_mutex() {
// 	int threadNumb = 5;
// 	int addedMonedyOnce = 1000;	
// 	int testNumb = 10;

// 	for (int i = 0; i < testNumb; ++i) {
// 		int val = getWalletMoney(threadNumb, addedMonedyOnce);
// 		if (val != threadNumb * addedMonedyOnce) {
// 			cout << "Error at count = " << i << " money in wallet" << val << endl;
// 		} else {
// 			cout << "Correct numb i: " << i << ", val: " << val << endl;
// 		}
// 	}
// }

string fetchDataFromDB(string recvData) 
{
  	//确保函数要5秒才能执行完成
  	std::this_thread::sleep_for(std::chrono::seconds(5));
	cout << "thread id: " << std::this_thread::get_id() << endl;
 	 //处理创建数据库连接、获取数据等事情
  	return "DB_" + recvData;
}
 
// string fetchDataFromFile(string recvData) {
//   //确保函数要5秒才能执行完成
//   std::this_thread::sleep_for(std::chrono::seconds(5));
 
//   //处理获取文件数据
//   return "File_" + recvData;
// }

void test_async() {
	//获取开始时间
	system_clock::time_point start = system_clock::now();

	int taskNumb = 5;
	list<future<string>> futureList;
	for (int i = 0; i < taskNumb; ++i) {
		// futureList.push_back(async(launch::async | launch::deferred, fetchDataFromDB, "Data"));
		futureList.push_back(async(launch::async, fetchDataFromDB, "Data"));
	}

	for (list<future<string>>::iterator it = futureList.begin(); it != futureList.end(); ++it) {
		it->get();
	}

	// future<string> resultFromDB_A = std::async(std::launch::async, fetchDataFromDB, "Data");
	// future<string> resultFromDB_B = std::async(std::launch::async, fetchDataFromDB, "Data");
	// //从文件获取数据
	// string fileData = fetchDataFromFile("Data");

	// //从DB获取数据
	// //数据在future<std::string>对象中可获取之前，将一直阻塞
	// string dbData_A = resultFromDB_A.get();
	// string dbData_B = resultFromDB_B.get();

	//获取结束时间
	auto end = system_clock::now();

	auto diff = duration_cast<seconds>(end - start).count();
	cout << "Total Time taken = " << diff << " seconds" << endl;

	//   //组装数据
	//   string data = dbData +  " :: " + fileData;

	//   //输出组装的数据
	//   cout << "Data = " << data << endl;
 
}

string getDataFromDB(std::string token) 
{
  //获取数据
  string data = "Data fetched from DB by Filter :: " + token;
  return data;
}

void test_packaged_task() {
  std::packaged_task<std::string(std::string)> task(getDataFromDB);
 
  //从packaged_task<>获取相关的future<>
  std::future<std::string> result = task.get_future();
 
  //将packaged_task传递给线程以异步运行
  std::thread th(std::move(task), "Arg");
 
  //join线程，阻塞直到线程完成时返回
  th.join();
 
  //获取packaged_task<>的结果，即getDataFromDB()的返回值
  std::string data = result.get();
 
  std::cout << data << std::endl;

}

// void test_condition_variable()
// {

// }

// void test_thread_func_main() {
// 	// test_create_thread();
// 	// test_mutex();
// 	// test_async();
// 	// test_packaged_task();
// }
