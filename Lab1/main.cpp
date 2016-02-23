//clear; g++ -pthread -std=c++11 -Wall -o main main.cpp

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class SLAU
{
public:
	SLAU(unsigned int N) {

	}
	~SLAU();


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

    unsigned int end_time = clock(); // конечное время
    unsigned int search_time = end_time - start_time; // искомое время
	
    cout << "Время выполнения скрипта: " << search_time << "мс." << endl; 

	return 0;
}


