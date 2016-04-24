/*
==============================
Реализация потокобезопасной 
двухсвзяной очереди
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
#include <string>

#include <boost/thread/barrier.hpp>

using namespace std;

template <class T> 
class threadDeque
{
private:

   struct Node
   {
      Node* Prev, *Next; 
      T* dequeue;

      Node(Node* Pr, Node* Ne, int Mx) {
         Prev = Pr;
         Next = Ne;
         dequeue = new T[Mx]();
      }

      void print(int Max, int first, int last) {
         for(int i = 0; i < Max; ++i){
            if (i > first && i < last)
            {
               cout << dequeue[i] << " ";
            }
         }
      cout << endl;
      }
   };

   mutex changeMutex, coutMutex; 
   int _maxSize, _realSize, _firstIdx, _lastIdx; 
   Node *_first, *_last; 

   void add_first () {        // добавляет новый узел в начало списка
      Node* temp = new Node(nullptr, _first, _maxSize);
      _first->Prev = temp;
      _first = temp;
      _firstIdx = _maxSize - 1;
   }

   void add_last () {      // добавляет новый узел в конец списка
      Node* temp = new Node(_last, nullptr, _maxSize);
      _last -> Next = temp;
      _last = temp;
      _lastIdx = 0;
   }

   void delete_first () {     // удаляет первый узел списка
      Node* temp = _first -> Next;
      delete _first;
      _first = temp;
      _first->Prev = nullptr;
      _firstIdx = 0;
   }
   
   void delete_last () {      // удаляет последний узел списка
      Node* temp = _last -> Prev;
      delete _last;
      _last = temp;
      _last->Next = nullptr;
      _lastIdx = _maxSize - 1;
   }

public:
   threadDeque() {
      _realSize = 0;
      _maxSize = 1000; 
      _lastIdx = _maxSize/2;
      _firstIdx = _maxSize/2 - 1; 
      Node * temp = new Node(nullptr, nullptr, _maxSize);
      _first = temp; 
      _last = temp;
   };
   ~threadDeque() {};

   void push_front (int temp) {
      unique_lock<mutex> lock (changeMutex);
      (_last -> dequeue)[_firstIdx] = temp;
      _firstIdx--;
      _realSize++;
      if (_firstIdx == -1)      // если первый дек переполнен
         add_first();   // добавляем в начало списка новый дек
   }

   void push_back (int temp) {
      unique_lock<mutex> lock (changeMutex);
      (_last -> dequeue)[_lastIdx] = temp;
      _lastIdx++;
      _realSize++;
      if (_lastIdx == _maxSize)     // если последний дек переполнен
         add_last();    // добавляем в конец списка новый дек
   }

   T pop_front () {
      unique_lock<mutex> lock (changeMutex);
      _realSize--;
      if (_firstIdx == _maxSize - 1)
         delete_first();
      else
         _firstIdx++;
      return (_last->dequeue)[_firstIdx];
   }
   
   T pop_back () {
      unique_lock<mutex> lock (changeMutex);
      _realSize--;
      if (_lastIdx == 0)
         delete_last();
      else
         _lastIdx--;
      return ( _last->dequeue )[_lastIdx];
   }

   bool empty() { return (_realSize == 0); }
   int size() { return _realSize; }

   void clear() {
      unique_lock<mutex> lock (changeMutex);
      Node* temp = _last;
      while (temp) {
         Node* M = temp->Next;
         delete temp->Prev;
         delete temp;
         temp = M;
      }
      temp = new Node(nullptr, nullptr, _maxSize);
      _last = temp;
      _last = temp;
      _realSize = 0;
      _firstIdx = _maxSize / 2 - 1;
      _lastIdx = _maxSize / 2;
   }

   void print() {
      unique_lock<mutex> lock (coutMutex);
      Node* temp = _last;
      while (temp) {
         temp->print(_maxSize, _firstIdx, _lastIdx);
         temp = temp->Next;
      }
   }

};

double randDouble (double fMin, double fMax) {  
   return fMin + (double)rand() / RAND_MAX * (fMax - fMin);
}

void threadWork (threadDeque<double> *dequeue, boost::barrier& startBarrier, mutex &coutMutex) {
   //startBarrier.wait();
   auto start_t = std::chrono::high_resolution_clock::now();

   for (int i = 0; i < 10; ++i)
   {
      dequeue->push_back(randDouble(0.0, 1000.0));
      dequeue->push_front(randDouble(0.0, 1000.0));   
   }
   unique_lock<mutex> lck(coutMutex);
   cout << dequeue->pop_back() << " ";
   cout << dequeue->pop_front() << endl; 
   
   auto end_t = std::chrono::high_resolution_clock::now();
   auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
   cout << "Время = " << time << "мс." << endl;
}

int main(int argc, char const *argv[]){

   threadDeque<double> *dequeue = new threadDeque<double>();

   if (!dequeue->empty())
         cout << "Очередь содержит" << dequeue->size() << "элементов" << endl;
   else cout << "Очередь пуста" << endl; 

   for (int i = 0; i < 10; ++i)
   {
      dequeue->push_back(randDouble(0.0, 1000.0));
      dequeue->push_front(randDouble(0.0, 1000.0));
      //cout << dequeue.pop_back() << " ";
      //cout << dequeue.pop_front() << endl; 
   }
   dequeue->print();
   //dequeue.clear();

   if (!dequeue->empty())
         cout << "Очередь содержит " << dequeue->size() << " элементов" << endl;
   else cout << "Очередь пуста" << endl; 

   vector<thread> threads;

   int MAX_THREAD = std::thread::hardware_concurrency();
   boost::barrier startBarrier(MAX_THREAD);  
   mutex coutMutex; 

   for (int i = 0; i < MAX_THREAD - 1; ++i) { 
      threads.emplace_back (threadWork, ref(dequeue), ref(startBarrier), ref(coutMutex));    
   } 

   for (auto& thread : threads) {
      if (thread.joinable()){
            thread.join();
         }
   }

   if (!dequeue->empty())
         cout << "Очередь содержит " << dequeue->size() << " элементов" << endl;
   else cout << "Очередь пуста" << endl; 
   
	return 0;
}