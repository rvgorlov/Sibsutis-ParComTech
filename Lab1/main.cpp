//clear; g++ -pthread -std=c++11 -Wall -o main main.cpp

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

class SLAU
{

private:
	double **_arraySlau;
	unsigned int _size;
	std::vector<double> _answers;
public:
	SLAU(unsigned int N) {
		_size = N; 
		_arraySlau = new double*[_size];
		for (unsigned int i = 0; i < _size; i++) {
			
			_arraySlau[i] = new double[_size + 1]; 
			
			for (unsigned int j = 0; j < _size + 1; j++) {
				_arraySlau[i][j] = 0; 
			} 
		}
	}

	~SLAU() {
		delete _arraySlau;
	};

	double randDouble (double fMin, double fMax) {
			double f = (double)rand() / RAND_MAX;
    		return fMin + f * (fMax - fMin);
	}

	void showSLAU () {
		cout << "Массив коэффициентов" << endl;
		for (unsigned int i = 0; i < _size; i++) { 
			for (unsigned int j = 0; j < _size + 1; j++) {
				cout << _arraySlau[i][j] << " "; 
			} 
			cout << endl; 
		}
	}

	void randomSLAU () {
		for (unsigned int i = 0; i < _size; i++) { 
			for (unsigned int j = 0; j < _size + 1; j++) {
				_arraySlau[i][j] = randDouble(1.0, 100.0); 
			}  
		}
	}

	unsigned int getSLAUSize () {
		return _size;	
	}

	vector<double> getAnswers () {

		double x[_size];
		for (unsigned int i = 0; i < _size; i++) {
            x[i] = _arraySlau[i][_size];
        }

        double m;

        for (unsigned int k = 1; k < _size; k++) {
            for (unsigned int j = k; j < _size; j++) {
                m = _arraySlau[j][k - 1] / _arraySlau[k - 1][k - 1];
                for (unsigned int i = 0; i < _size + 1; i++) {
                    _arraySlau[j][i] = _arraySlau[j][i] - m * _arraySlau[k - 1][i];
                }
                x[j] = x[j] - m * x[k - 1];
            }
        }

        for (int i = _size - 1; i >= 0; i--) {
            for (int j = i + 1; j < _size; j++) 
            	x[i] -= _arraySlau[i][j] * x[j];
            x[i] = x[i] / _arraySlau[i][i];
        }

        for (unsigned int i = 0; i < _size; ++i) { _answers.push_back(x[i]); }

		return _answers; 
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
	unsigned int size = inputUnsInt::inputUnsIntCheck(); 

	SLAU slau(size);
	slau.randomSLAU();
	slau.showSLAU();
	std::vector<double> v = slau.getAnswers(); 
	
	for (std::vector<double>::iterator i = v.begin(); i != v.end(); ++i)
	{
		cout << *i << endl;
	}

    unsigned int end_time = clock(); // конечное время
    unsigned int search_time = end_time - start_time; // искомое время
	
    cout << "Время выполнения скрипта: " << search_time << "мс." << endl; 

	return 0;
}


