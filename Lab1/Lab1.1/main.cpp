//clear; valgrind --tool=helgrind --history-level=approx ./main
//valgrind -v --leak-check=full --show-leak-kinds=all ./main
// g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main.cpp; valgrind --tool=helgrind --history-level=approx ./main
// g++ -pthread -Wl,--no-as-needed -lboost_system -lboost_thread -std=c++11 -Wall -o main main.cpp; 


#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <iomanip>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <chrono>
#include <mutex>

using namespace std;

double **_arraySlau;
int _size, MAX_THREAD;

double randDouble (double fMin, double fMax) {	
 	return fMin + (double)rand() / RAND_MAX * (fMax - fMin);
}

void printArr (double **arr, int size ) {
	if (size < 17) { 
		for (int i = 0; i < size + 1; ++i) 
			cout << "========";
	 	cout << endl; 
		cout.setf(ios::fixed);
		for (int i = 0; i < size; ++i)
		{	
			cout << setw(2) << left << i << ") "; 
			for (int j = 0; j < size + 1; ++j)
			{	
				if (j == size) cout << "|"; 
				if ((arr[i][j] < 0 && arr[i][j] > -0.001) || 
					(arr[i][j] > 0 && arr[i][j] < 0.0001))
				{
					cout << setw(8) << left << setprecision(1) << 0;
				}else
					cout << setw(8) << left << setprecision(1) << arr[i][j]; 
			}
		cout << endl; 
		}	
	}
}

void writeToFile (double **arraySlau, int size) {
	ofstream fout("arraySlau.txt");
	for (int i = 0; i < size; i++) { 
		for (int j = 0; j < size + 1; j++) {
			fout << arraySlau[i][j] << " ";
		} 
	fout << endl; 
	}
	fout.close();
}

vector<double> getAnswers (double **arraySlau, int size) {

	cout << "Метод Гауса - без thread:" << endl; 
	auto start_t = std::chrono::high_resolution_clock::now();

	double **arr;
	arr = new double*[size];  
	for (int i = 0; i < size; i++) { 
		arr[i] = new double[size + 1]; 
		for (int j = 0; j < size + 1; j++) {
			arr[i][j] = arraySlau[i][j];
		}  
	}

	double m;

    // прямой ход - исключение неизвестных
    for (int k = 1; k < size; k++) {
 		for (int j = k; j < size; j++) {
 			m = arr[j][k - 1] / arr[k - 1][k - 1];
 			for (int i = 0; i < size + 1; i++) {
 				arr[j][i] = arr[j][i] - m * arr[k - 1][i];	
 			}
 		}
 	}

 	arr[size - 1][size] /= arr[size - 1][size - 1]; 
 	for (int i = size - 2; i >= 0; i--) {
		for (int j = i + 1; j < size; j++)
 			arr[i][size] -= arr[i][j] * arr[j][size]; 
 		arr[i][size] /= arr[i][i];
 	}
 	//printArr(arr, size);
 	
	vector<double> _answers;	
	for (int i = 0; i < size; ++i) 
		_answers.push_back(arr[i][size]); 
	
    auto end_t = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
   	cout << "Время = " << time << "мс." << endl; 

    for (int i = 0; i < size; ++i)
    	delete[] arr[i]; 
    delete[] arr; 
	
	return _answers;  
}

void threadCalculateString (double **arr, int size, int start, int end, boost::barrier& cur_barier, boost::barrier& phase1) {
	double m;
	for (int k = 1; k < size; ++k) {	
		for (int j = k; j < end; ++j) {	
			if ((j >= start) && (j < end)) {
				m = arr[j][k - 1] / arr[k - 1][k - 1];
 				for (int i = 0; i < size + 1; ++i)
 					arr[j][i] = arr[j][i] - m * arr[k - 1][i];
			}
		}
 		cur_barier.wait();
	}

	phase1.wait(); 

	for (int i = size - 2; i >= 0; i--) {
		if ((i >= start) && (i < end)) {
 			for (int j = i + 1; j < size; j++)
 				arr[i][size] -= arr[i][j] * arr[j][size]; 
 			arr[i][size] /= arr[i][i];
		}
		cur_barier.wait();
 	}
 }

vector<double> getAnswers (double **arraySlau, int size, int MAX_THREAD) {

	cout << "Метод Гауса - с использованием thread:" << endl; 
	auto start_t = std::chrono::high_resolution_clock::now();
	vector<double> _answers;
	vector<thread> threads;
    boost::barrier bar(MAX_THREAD), phase1(MAX_THREAD);

	double **arr = new double*[size];  
	for (int i = 0; i < size; i++) { 
		arr[i] = new double[size + 1]; 
		for (int j = 0; j < size + 1; j++) {
			arr[i][j] = arraySlau[i][j];
		}  
	}

    int temp = size / (MAX_THREAD - 1);  			//для определения интервалов для потоков

 	for (int i = 0; i < (MAX_THREAD - 1); ++i) {
    	int start = temp * i, end = temp * (i + 1); 
		threads.emplace_back (threadCalculateString, ref(arr), size, start, end, boost::ref(bar), boost::ref(phase1)); 	
 	} 
 	
 	int start, end;

 	if (size % MAX_THREAD - 1 != 0)
 	{
 		double m;
 		start = temp * (MAX_THREAD - 1); 
 		end = size; 
 		for (int k = 1; k < size; ++k)
		{	

			for (int j = k; j < end; ++j)
			{
				if ((j >= start) && (j < end))
				{	
					m = arr[j][k - 1] / arr[k - 1][k - 1];
 					for (int i = 0; i < size + 1; ++i) {
 						  arr[j][i] = arr[j][i] - m * arr[k - 1][i];
 					}
				}
			}
			bar.wait();
		}	
 	}  

 	arr[size - 1][size] /= arr[size - 1][size - 1]; 

 	phase1.wait();
 	
 	if (size % MAX_THREAD - 1 != 0)
 	{
 		start = temp * (MAX_THREAD - 1);
 		end = size;

		for (int i = size - 2; i >= 0; i--) {
			if ((i >= start) && (i < end)) {
 				for (int j = i + 1; j < size; j++)
 					arr[i][size] -= arr[i][j] * arr[j][size]; 
 				arr[i][size] /= arr[i][i];
			}
			bar.wait();
 		}
 	} 

 	for (auto& thread : threads) {
    	if (thread.joinable()){
      		thread.join();
      	}
    } 	
	
	for (int i = 0; i < size; ++i) 
		_answers.push_back(arr[i][size]); 

    auto end_t = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(end_t-start_t).count();
   	cout << "Время = " << time << "мс." << endl; 

    for (int i = 0; i < size; ++i)
    	delete[] arr[i]; 
    delete[] arr; 
	
	return _answers;  
}

bool is_equalDouble(double x, double y) {
    return std::fabs(x - y) < std::numeric_limits<double>::epsilon();
}

int main(int argc, char const *argv[]){
	
	bool yesOrNot = false;	
	int _size = 15;					// Размер матрицы size*size. 
	_arraySlau = new double*[_size];
	
	for (int i = 0; i < _size; i++) {
		_arraySlau[i] = new double[_size + 1]; 
		for (int j = 0; j < _size + 1; j++) {
			_arraySlau[i][j] = randDouble(0.0, 1000.0);  
		} 
	}

	//writeToFile(_arraySlau, _size); 
	printArr(_arraySlau, _size); 

	cout << "Размер матрицы " << _size << " X " << _size + 1 << endl;
	MAX_THREAD = std::thread::hardware_concurrency(); 
	cout << "Всего доступно одновременно - " << MAX_THREAD << " потока(ов)"<< endl;   

	vector<double> answer1 = getAnswers(_arraySlau, _size);					// Последовательный алгоритм
	vector<double> answer2 = getAnswers(_arraySlau, _size, MAX_THREAD);		// Паралельный алгоритм 

	cout << "Ответы:|без threads | c threads" << "| Всего ответов - " << answer1.size() << endl;
	std::vector<double>::iterator ii = answer2.begin();
	int idx = 0; 
	 
	for (std::vector<double>::iterator i = answer1.begin(); i != answer1.end(); ++i, ++ii) {	
		if (idx < 16) {
			cout << setw(1) << left << "X" 
			<< setw(2) << left << idx + 1 << " =  |" 
			<< setw(12) << left << setprecision(3) << *i << "| "
			<< setw(8) << left << setprecision(3) << *ii  << " |"<< endl;
			idx++;
		}
		if (is_equalDouble(round(*i * 100) /100, round(*ii * 100) / 100) != true) {
			yesOrNot = true; 
		}
	}

	if (yesOrNot)
		cout << "Проверка не пройдена" << endl;	
	else cout << "\n   === Проверка пройдена \n"<< endl;

	for (int i = 0; i < _size; ++i)
    	delete[] _arraySlau[i]; 
    delete[] _arraySlau;
	
	return 0;
}
