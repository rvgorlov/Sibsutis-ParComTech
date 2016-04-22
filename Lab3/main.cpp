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

#include <boost/thread/barrier.hpp>

using namespace std;

double randDouble (double fMin, double fMax) {  
   return fMin + (double)rand() / RAND_MAX * (fMax - fMin);
}

int main(int argc, char const *argv[])
{
	vector<thread> threads;
	
	
	return 0;
}