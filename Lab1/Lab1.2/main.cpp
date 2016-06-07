/*
==============================
Реализация алгоритма поиска 
подстроки в строке
==============================
*/

/*
Для отладки: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main1.cpp; ./main
Для отладки в потоках: 
clear; g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main1.cpp; valgrind --tool=helgrind --history-level=approx ./main
*/

#include <iostream>
#include <fstream>
#include <thread>
#include <iomanip>
#include <chrono>
#include <mutex>		
#include <string>
#include <vector>

using namespace std;

// One thread search
std::vector<int> find_substrings(string S, string W) {
	std::vector<int> answers;
	unsigned int i, j;
	int number = 0;
	for (i = 0; i < S.length() - W.length() + 1; ++i) {
		j = 0;
		while ((j < W.length()) && (W[j] == S[i + j])) {
			j = j + 1;
		}
		if (j == W.length()) {
			//cout << "Вхождение слова " << W << " , с номера позиции: " << i << endl;
			number++;
			answers.push_back(i); 
		}
	}
	return answers; 
}

// Some thread search
void find_substrings_thread(string& S, string& W, int start, int end, std::vector<int>& answers, mutex &changeMutex) {
	int j;
	for (int i = start; i < end; ++i) {
		if (end - i > W.length())
		{
			j = 0;
			while ((j < (int) W.length()) && (W[j] == S[i + j])) {
				j = j + 1;
			}
			if (j == (int) W.length()) {
				unique_lock<mutex> lock (changeMutex);
				answers.push_back(i); 
				lock.unlock();
			}
		}
	} 
}

// parse all file in string 
string getString (string namefile) {
	string Str;
	std::ifstream ifs;
	ifs.open (namefile, std::ifstream::in);
	while (ifs.good()) {
    	Str += ifs.get();
  	}
  	ifs.close();
	return Str;
}

// multithread search handle
vector<int> handle (string& Str, string Search_key) {
	int maxThread = std::thread::hardware_concurrency(); 
	vector<thread> threads;
	std::vector<int> answers;
	mutex changeMutex; 
	int temp = Str.length() / maxThread, i, j; 

	// Запускаем в потоках
	for (i = 0; i < (maxThread); ++i) {
    	int start = temp * i, end = temp * (i + 1); 
		threads.emplace_back (find_substrings_thread, ref(Str), ref(Search_key), start, end, ref(answers), ref(changeMutex)); 	
 	} 

 	// Подбираем остатки
 	if (Str.length() % maxThread != 0)
 	{
 		int start = temp * maxThread, end = Str.length();
		for (i = start; i < end; ++i) {
			j = 0;
			if (end - i > Search_key.length())
			{
				while ((j < (int) Str.length()) && (Str[j] == Search_key[i + j])) {
				j = j + 1;
				}
				if (j == (int) Str.length()) {
					unique_lock<mutex> lock (changeMutex);
					answers.push_back(i); 
					lock.unlock();
				}	
			}
		} 
 	}

 	// джойним все потоки
 	for (auto& thread : threads) {
    	if (thread.joinable()){
      		thread.join();
      	}
    }

    return answers;
}

int main(int argc, char const *argv[])
{
	// Parse string
	string Str = getString("text.txt");	
	cout << "Длинна строки = " << Str.length() << endl;

	// One thread
	auto start_t = std::chrono::high_resolution_clock::now();
	std::vector<int> v = find_substrings(Str, "later");
	auto end_t = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
   	cout << "Однопоточно = " << time << "мс." << endl; 

	if (v.size() == 0){
		cout << "Массив пуст" << endl;
	} else {
		cout << "Всего " << v.size() << " вхождений(я)" << endl;
		/*for (std::vector<int>::iterator i = v.begin(); i != v.end(); ++i)
	 		cout << *i << endl;*/
	}

	// Clear vetor
	v.clear(); 

	// Some thread
	start_t = std::chrono::high_resolution_clock::now();
	v = handle(Str, "later");
	end_t = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
   	cout << "Многопоточно = " << time << "мс." << endl; 

	if (v.size() == 0){
		cout << "Массив пуст" << endl;
	} else {
		cout << "Всего " << v.size() << " вхождений(я)" << endl;
		/*for (std::vector<int>::iterator i = v.begin(); i != v.end(); ++i)
	 		cout << *i << endl;*/
	}
	return 0;
}

