/*
==============================
Реализация потокобезопасного 
двухсвзяного списка List
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

template <typename T>
class threadList
{
private: 
   struct Node {
      T x;
      Node *Prev, *Next; 
   };

   int _structSize; 
   Node *Head, *Tail;
   mutex changeMutex, coutMutex; 

public:
   threadList():_structSize(0),Head(NULL),Tail(NULL){}; 
   ~threadList() {
      unique_lock<mutex> lock (changeMutex);
      while (Head) { 
         Tail = Head->Next;
         delete Head;
         Head = Tail;
      }
}

bool seachNode(T seachX) {
   Node *temp = Head; 
   unique_lock<mutex> lock (changeMutex);
   if (Tail->x == seachX)
      return true;

   while (temp != Tail) {
      if (temp->x == seachX)
         return true; 
      temp = temp->Next; 
   }
   return false; 
}

void deleteX(T deleteX) {
   Node *temp = Head; 
   unique_lock<mutex> lock (changeMutex);
   if (Head != NULL)
   {
      while (temp != Tail) {      
         if    (temp->x == deleteX){
            temp->Prev->Next = temp->Next;
            temp->Next->Prev = temp->Prev;
            --_structSize;
            return;
         }          
         temp = temp->Next; 
      }

      if (Tail->x == deleteX) {
         temp = Tail; 

         Tail = temp->Prev; 
         temp->Prev->Next = NULL; 
         --_structSize;
         delete temp;
         return;
      }
   }
   
}

void Add(T x) {
   unique_lock<mutex> lock (changeMutex);
   ++_structSize; 
   Node *temp = new Node;
   temp->Next = NULL;
   temp->x = x;
   
   if (Head!=NULL)
   {
       temp->Prev = Tail;
       Tail->Next = temp;
       Tail = temp;
   }
   else {
      temp->Prev = NULL;
      Head = Tail = temp;
   }
}

void Show() {
   Node *temp = Head;
   int idx = 0;  
   unique_lock<mutex> lock (changeMutex);
   while (temp != NULL) {
      ++idx;
      if (idx == _structSize)
         cout << temp->x;
      else {
         cout << temp->x << " -> ";
         if (idx % 5 == 0)
               cout << endl;
      }
      temp = temp->Next; 
   }
   cout << endl;
}

T getSize() { return _structSize; }
T getLast() { return Tail->x; }
T getHead() { return Head->x; } 

};

double randDouble (double fMin, double fMax) {  
   return fMin + (double)rand() / RAND_MAX * (fMax - fMin);
}

void threadWork (threadList<double> &lst1, boost::barrier& startBarrier, mutex &coutMutex, int variable) {
   startBarrier.wait();
   auto start_t = std::chrono::high_resolution_clock::now();


/*switch (variable) {
      case 0:
         lst1.deleteX(20);  
         break;
      case 2:
         lst1.Add(999);
         break;
      case 3:
         lst1.seachNode(999);
         break;
      default:
      break;
}*/
   //cout << variable << endl; 

   for (int i = 0; i < 1000000; ++i)
   {
      lst1.Add(variable);
      lst1.seachNode(variable);
      lst1.deleteX(variable);
   }
   
   auto end_t = std::chrono::high_resolution_clock::now();
   auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
   unique_lock<mutex> lck(coutMutex); cout << "Время = " << time << "мс." << endl; 
}

int main(int argc, char const *argv[]){

   threadList<double> lst;
   vector<thread> threads;

   int MAX_THREAD = std::thread::hardware_concurrency();
   boost::barrier startBarrier(MAX_THREAD);
   mutex coutMutex; 

   for (int i = 0; i < 33; ++i) 
      lst.Add(randDouble(0.0, 1000.0));   
   //lst.Add(20.00);
   //lst.Add(999);

   int variable = rand()%3; 
   for (int i = 0; i < MAX_THREAD; ++i) { 
      threads.emplace_back (threadWork, ref(lst), ref(startBarrier), ref(coutMutex), variable);    
   } 

   for (auto& thread : threads) {
      if (thread.joinable()){
            thread.join();
         }
   }

   /*for (int i = 0; i < 33; ++i) 
      lst.Add(randDouble(0.0, 1000.0));   
   lst.Add(20.00);*/

   /*cout << lst.getSize() << endl; 
   cout << lst.getHead() << endl;
   cout << lst.getLast() << endl;*/

   /*lst.deleteX(20);*/

   if (lst.seachNode(20)) cout << "Искомый элемент найден" << endl;
   else cout << "Искомый элемент не найден" << endl;

   lst.Show();
   
	return 0;
}