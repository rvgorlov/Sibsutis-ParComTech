/*
==============================
Реализация потокобезопасного 
двухсвзяного списка List
==============================
*/

/*
Для отладки: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main.cpp; ./main
Для отоладки в потоках: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main.cpp; valgrind --tool=helgrind --history-level=approx ./main
*/


#include <iostream>
#include <thread>
#include <mutex>

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

public:
   threadList():_structSize(0),Head(NULL),Tail(NULL){}; 
   
   ~threadList() {
   
      while (Head) { 
         Tail = Head->Next;
         delete Head;
         Head = Tail;
      }

}

bool seachNode(T seachX) {
   Node *temp = Head; 
   
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

int main(int argc, char const *argv[]){

threadList<double> lst;

for (int i = 0; i < 33; ++i) 
   lst.Add(randDouble(0.0, 1000.0));   
lst.Add(20.00);

cout << lst.getSize() << endl; 
cout << lst.getHead() << endl;
cout << lst.getLast() << endl;

lst.deleteX(20);

if (lst.seachNode(20)) cout << "Искомый элемент найден" << endl;
else cout << "Искомый элемент не найден" << endl;

lst.Show(); //Отображаем список на экране
   
	return 0;
}