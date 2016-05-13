/*
==============================
Реализация редукции для 
произволных операций над
массивом
==============================
*/

/*
Для отладки: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main1.cpp; ./main
Для отладки в потоках: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main1.cpp; valgrind --tool=helgrind --history-level=approx ./main
*/

#include <vector>
#include <iostream>
#include <iomanip>
#include <future>

using namespace std;

template<typename FuncVec>
int handle(const vector<int>& v, FuncVec f,  int MAX_THREAD)
{
    std::vector<future<int>> threads;
    std::vector<int> answers;
    int mid = v.size() / MAX_THREAD; 
    int start = 0, end = start + mid; 
    for (int i = 0; i < MAX_THREAD; ++i)
    {  
        start = mid * i;
        end = start + mid;
        if (end != (int) v.size() && (int) v.size() - end < mid)
            end = v.size();   
        threads.emplace_back (std::async(std::launch::async, f, v, start, end));     
    }
    for (auto& thread : threads) {
        answers.push_back(thread.get());
    }
    return f(answers, 0, answers.size()); 
}
/*const int minSize = 10000;
template<typename FuncFunc, typename FuncVec>
int handle(const vector<int>& v, int start, int end, FuncVec f, FuncFunc g)
{

    if (end - start < minSize)
    {
        return f(v,start,end);
    }
    int mid = (end+start)/2;
    future<int> f1 = async(handle<FuncFunc,FuncVec>,v,start,mid,f,g);
    future<int> f2 = async(handle<FuncFunc,FuncVec>,v,mid,end,f,g);
    return g(f1.get(),f2.get());
}*/


int sumVec(const vector<int>& v, int start, int end)
{
    int sum = 0;
    for(int i = start; i < end; ++i) sum += v[i];
    return sum;
}
int sumFunc(int v1, int v2) { return v1+v2; }

int maxVec(const vector<int>& v, int start, int end)
{
    int max = v[start];
    for(int i = start + 1; i < end; ++i) if (max < v[i]) max = v[i];
    return max;
}
int maxFunc(int v1, int v2) { return (v1>v2) ? v1 : v2; }

int main(int argc, const char * argv[])
{
    int Num = 10000211; 

    cout << "Создаём " << Num << " элементов." << endl;
    vector<int> v(Num);
    for(int i = 0; i < Num; ++i)
        v.push_back(rand() % 10);

    int MAX_THREAD = std::thread::hardware_concurrency(); 
    cout << "Всего доступно одновременно - " << MAX_THREAD << " потока(ов)"<< endl;
    cout << "...Подсчёт..." << endl;

    auto start_t = std::chrono::high_resolution_clock::now();
    cout << "(Один поток) Сумма = " << sumVec(v, 0, v.size() ) << " ";
    auto end_t = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
    cout << "Время = " << time << "мс." << endl;

    start_t = std::chrono::high_resolution_clock::now();
    cout << "(Многопоточ) Сумма = " << handle(v, sumVec, MAX_THREAD) /*handle(v,0,v.size(),sumVec,sumFunc)*/ << " ";
    end_t = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
    cout << "Время = " << time << "мс." << endl;

    start_t = std::chrono::high_resolution_clock::now();
    cout << "(Один поток) Максимум = " << maxVec(v, 0, v.size() ) << " ";
    end_t = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
    cout << "Время = " << time << "мс." << endl;

    start_t = std::chrono::high_resolution_clock::now();
    cout << "(Многопоточ) Максимум = " << handle(v,maxVec, MAX_THREAD) /*handle(v,0,v.size(),maxVec,maxFunc)*/ << " ";
    end_t = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
    cout << "Время = " << time << "мс." << endl;
}