//clear; g++ -pthread -Wl,--no-as-needed -std=c++11 -Wall -o main main1.cpp


#include <iostream>
//#include <pthread.h>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <iomanip>
#include <atomic>
#include <condition_variable>

using namespace std;

class barrier {
	const unsigned int threadCount;
	std::atomic<unsigned int>threadsWaiting; 
	bool isNotWaiting;
	std::condition_variable waitVariable;
	std::mutex mutex;

public:
	barrier(unsigned int n) : threadCount(n) {
		threadsWaiting = 0;
  		isNotWaiting = false;
	}

	barrier(const barrier &) = delete;
 	
 	void wait() {
  		if (threadsWaiting.fetch_add(1) >= threadCount - 1) {
			isNotWaiting = true;
   			waitVariable.notify_all();
   			threadsWaiting.store(0);
 		} else {
  			std::unique_lock<std::mutex> lock(mutex);
  			waitVariable.wait(lock,[&]{ return isNotWaiting;});
 		}
	}
};

barrier *rowBarrier; 
mutex coutMutex; 


double **_arraySlau;
int _size;


double randDouble (double fMin, double fMax) {
	double f = (double)rand() / RAND_MAX;
 	return fMin + f * (fMax - fMin);
}

void printArr (double **arr, int size ) {
	if (size < 17) { 
		for (int i = 0; i < size + 1; ++i) cout << "========";
	 	cout << endl; 
		cout.setf(ios::fixed);
		for (int i = 0; i < size; ++i)
		{
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
	/*double arr[size][size + 1]; 

	for (int i = 0; i < size; i++) { 
		for (int j = 0; j < size + 1; j++) {
			arr[i][j] = arraySlau[i][j];
			//cout << arr[i][j] << " "; 
		} 
		//cout << endl; 
	}*/
	double **arr;
	arr = new double*[size];  
	for (int i = 0; i < size; i++) { 
		arr[i] = new double[size + 1]; 
		for (int j = 0; j < size + 1; j++) {
			arr[i][j] = arraySlau[i][j];
		}  
	}

	unsigned int start_time =  clock();
	double m;
	double x[size];

	for (int i = 0; i < size; i++) {
        x[i] = arr[i][size];
    }
    
    // прямой ход - исключение неизвестных
    for (int k = 1; k < size; k++) {
 		for (int j = k; j < size; j++) {
 			m = arr[j][k - 1] / arr[k - 1][k - 1];
 			//cout << m << endl;
 			for (int i = 0; i < size; i++) {
 				arr[j][i] = arr[j][i] - m * arr[k - 1][i];	
 			}
        	x[j] = x[j] - m * x[k - 1];
 		}
 		//printArr(arr, size);
 	}

 	// обратный ход - подставляем и считаем
 	for (int i = size - 1; i >= 0; i--) {
 		for (int j = i + 1; j < size; j++) x[i] -= arr[i][j] * x[j];
 		x[i] = x[i] / arr[i][i];
 	}

 	unsigned int end_time = clock();

	vector<double> _answers;	
	for (int i = 0; i < size; ++i) _answers.push_back(x[i]); 

	
    unsigned int search_time = end_time - start_time; 
	
    cout << "Время = " << search_time << "мс." << endl; 

	return _answers;  
}

void threadCalculateString (double **arr, double *x, int size, int k, int j) {
	//rowBarrier->wait();
	double m = arr[j][k - 1] / arr[k - 1][k - 1];
	//cout << m << endl;
 	for (int i = 0; i < size; i++) {
 		//rowBarrier->wait();

 		/*coutMutex.lock();*/ arr[j][i] = arr[j][i] - m * arr[k - 1][i];/*coutMutex.unlock();*/
 		// coutMutex.lock();cout << "1" << endl;coutMutex.unlock();
 		//rowBarrier->wait(); 	
 		// coutMutex.lock();cout << "2" << endl; coutMutex.unlock();
 	}
    /*coutMutex.lock();*/ x[j] = x[j] - m * x[k - 1]; /*coutMutex.unlock();*/
    //rowBarrier->wait();
	return; 
}

vector<double> getAnswersThread (double **arraySlau, int size) {

	cout << "Метод Гауса - с использованием thread:" << endl; 
	cout << "Всего доступно одновременно - " << std::thread::hardware_concurrency() << " потока"<< endl;   
	
	double **arr;
	arr = new double*[size];  
	for (int i = 0; i < size; i++) { 
		arr[i] = new double[size + 1]; 
		for (int j = 0; j < size + 1; j++) {
			arr[i][j] = arraySlau[i][j];
		}  
	}

	unsigned int start_time =  clock();
	
	double *x = new double[size];
	for (int i = 0; i < size; i++) {
        x[i] = arr[i][size];
    }
    
    vector<thread> threads;
    //thread threads[size];
	// прямой ход - исключение неизвестных
    /*for (int k = 1; k < size; k++) {
    	cout << "Запускаем поток №" << k << endl; 
 		thread thr(threadCalculateString, ref(arr), ref(x), size, k);
 		threads[k - 1] = move(thr); 	
 	}*/

 	for (int k = 1; k < size; ++k) {
 		//rowBarrier = new barrier((unsigned int) size - k);
 		for (int j = k; j < size; ++j) {
 			//cout << "Запускаем поток №" << j << endl; 
 			thread thr(threadCalculateString, ref(arr), ref(x), size, k, j);
 			//threads[j - k] = move(thr);
			threads.push_back(move(thr));  		
 		}

 		try {
        	//for (int i = 0; i < size - k; ++i) threads[i].join();
        	/*for (std::vector<thread>::iterator i = threads.begin(); i != threads.end(); ++i)
        	{
        		i->join(); 
        	}*/
        	for (auto& t : threads) {
  		
    			if (t.joinable())
      				t.join();
      		}
    	} catch(const std::system_error& e) {
        	std::cout << "Caught system_error with code in thread " << e.code() 
              		  << " meaning " << e.what() << '\n';
    	}
        //printArr(arr, size);
    	delete rowBarrier;
 	}

 	/*try {
        for (int i = 0; i < size - 1; ++i) threads[i].join();
    } catch(const std::system_error& e) {
        std::cout << "Caught system_error with code in thread " << e.code() 
                  << " meaning " << e.what() << '\n';
    }*/

    // обратный ход - подставляем и считаем
 	for (int i = size - 1; i >= 0; i--) {
 		for (int j = i + 1; j < size; j++) x[i] -= arr[i][j] * x[j];
 		x[i] = x[i] / arr[i][i];
 	}

    unsigned int end_time = clock();

	vector<double> _answers;	
	for (int i = 0; i < size; ++i) _answers.push_back(x[i]); 

    unsigned int search_time = end_time - start_time; 
    cout << "Время = " << search_time << "мс." << endl; 

	return _answers;  
}

class inputUnsInt
{
public:
	inputUnsInt();
	~inputUnsInt();
	
	static int inputUnsIntCheck () {
	unsigned int size = 0;
	std::ios::iostate state;
    do {
		int temp; 
        std::cout << "Введите колличество коэфициентов: " << std::endl;
        std::cin >> temp;
        if (temp >= 0) {
			state = std::cin.rdstate();
			if (state != std::ios::goodbit) {
				std::cout << "Ошибка" << std::endl;
				std::cin.clear(std::ios::goodbit);
				std::cin.ignore(65535, '\n');
			}
		} else {
			std::cout << "Ошибка" << std::endl;
			std::cin.clear(std::ios::goodbit);
			std::cin.ignore(65535, '\n');
		}
		size = temp;
	} while (state != std::ios::goodbit);
    std::cout << "Вы ввели: " << size << endl;
    return (int) size; 
	};	
};

int main(int argc, char const *argv[])
{
	//int _size = inputUnsInt::inputUnsIntCheck(); 
	int _size = 200; 
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

	vector<double> answer1 = getAnswers(_arraySlau, _size);
	vector<double> answer2 = getAnswersThread(_arraySlau, _size);
	cout << "Ответы: без threads, c threads" << endl;

	std::vector<double>::iterator ii = answer2.begin();
	//int idx = 0; 
	bool yesOrNot = false; 
	for (std::vector<double>::iterator i = answer1.begin(); i != answer1.end(); ++i, ++ii)
	{
		/*cout << "X" << idx + 1 << " = " << *i << "  " << *ii << endl;
		idx++;*/

		if (*i != *ii)
		{
			/*cout << "всё верно" << endl;*/
			yesOrNot = true;  
		} /*else cout << "Ошибка" << endl;*/ 
	}

	if (yesOrNot)
	{	
		cout << "Проверка не пройдена" << endl;
		
	} else cout << "Проверка пройдена" << endl;

	return 0;
}