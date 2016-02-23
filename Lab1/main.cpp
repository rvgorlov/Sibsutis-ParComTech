//clear; g++ -pthread -std=c++11 -Wall -o main main.cpp

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class SLAU
{

private:
	double **_arraySlau;
	unsigned int _size;
public:
	SLAU(unsigned int N) {
		_size = N; 
		_arraySlau = new double*[_size];
		for (unsigned int i = 0; i < _size; i++) {
			
			_arraySlau[i] = new double[_size]; 
			
			for (unsigned int j = 0; j < _size; j++) {
				_arraySlau[i][j] = 0; 
			} 
		}
	}

	~SLAU() {
		delete _arraySlau;
	};

	class randomDouble {
	public:
		randomDouble();
		~randomDouble();

		static double randDouble (double fMin, double fMax) {
			double f = (double)rand() / RAND_MAX;
    		return fMin + f * (fMax - fMin);
		}
	};

	void showSLAU () {
		for (unsigned int i = 0; i < _size; i++) { 
			for (unsigned int j = 0; j < _size; j++) {
				cout << _arraySlau[i][j] << " "; 
			} 
			cout << endl; 
		}
	}

	void randomSLAU () {
		for (unsigned int i = 0; i < _size; i++) { 
			for (unsigned int j = 0; j < _size; j++) {
				_arraySlau[i][j] = randomDouble::randDouble(1.0, 100.0); 
			}  
		}
	}

	unsigned int getSLAUSize () {
		return _size;	
	}
};

class inputUnsInt
{
public:
	inputUnsInt();
	~inputUnsInt();
	
	static unsigned int inputUnsIntCheck () {
	unsigned int size = 0;
	std::ios::iostate state;
    do {
		int temp; 
        std::cout << "Введите N" << std::endl;
        std::cin >> temp;
        if (temp >= 0) {
			state = std::cin.rdstate();
			if (state != std::ios::goodbit) {
				std::cout << "Error" << std::endl;
				std::cin.clear(std::ios::goodbit);
				std::cin.ignore(65535, '\n');
			}
		} else {
			std::cout << "Error" << std::endl;
			std::cin.clear(std::ios::goodbit);
			std::cin.ignore(65535, '\n');
		}
		size = temp;
	} while (state != std::ios::goodbit);
    std::cout << "Вы ввели: " << size << endl;
    return size; 
	};	
};

int main(int argc, char **argv)
{
	

	unsigned int start_time =  clock(); // начальное время
    
	/*somecode*/

	unsigned int size = inputUnsInt::inputUnsIntCheck(); 

	SLAU slau(size);
	slau.randomSLAU();
	slau.showSLAU();

    unsigned int end_time = clock(); // конечное время
    unsigned int search_time = end_time - start_time; // искомое время
	
    cout << "Время выполнения скрипта: " << search_time << "мс." << endl; 

	return 0;
}


