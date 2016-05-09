/*
==============================
Реализация редукции для 
произволных операций над
массивом
==============================
*/

/*
Для отладки: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main.cpp; ./main
Для отладки в потоках: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main.cpp; valgrind --tool=helgrind --history-level=approx ./main
*/

#include <iomanip>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstdlib>

#include <boost/thread/barrier.hpp>

using namespace std;

template <typename T> 
class functor
{
public:
	T operator() () { 
		
	}
};

int main(int argc, char const *argv[])
{
	int arraySize, *arr;
	if (argc < 2) {
		cout << "Ошибка, задайте параметры программы" << endl;
		return 0;
	}
	arraySize = atoi(argv[1]);
	if (arraySize < 1) { 
		cout << "Ошибка заданы не правильные параметры" << endl;
		return 0;
	}
	arr = new int[arraySize];
	for (int i = 0; i < arraySize; ++i)
	 	arr[i] = rand() % 100;
	cout << "Создан массив с " << arraySize << " символами" << endl;

	
	return 0;
}