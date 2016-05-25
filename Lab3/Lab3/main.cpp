using namespace std;

#include "reduce.h"

int main()
{
    int N = 100000000;
    int *array = new int[N];

    for (int i = 0; i < N; i++)
        array[i] = rand() % 10 + 1;

    int MAX_THREAD = std::thread::hardware_concurrency();
    cout << "Доступно " << MAX_THREAD << " потоков" << endl;

    // Для сравнения, однопоточная сумма
    auto start_t = std::chrono::high_resolution_clock::now();
    int result  = 0;
    for (int i = 0; i < N; ++i) {
        result  += array[i];
    }
    auto end_t = std::chrono::high_resolution_clock::now();
    cout << "(Однопоточн) сумма: " << result << " "
         << "Время = " << std::chrono::duration<double, std::milli>(end_t-start_t).count()
         << "мс." << endl;

    // редукция для суммы
    start_t = std::chrono::high_resolution_clock::now();
    result = handler('+', array, N, MAX_THREAD);
    end_t = std::chrono::high_resolution_clock::now();
    cout << "(Многопоточ) суммы: " << result << " "
         << "Время = " << std::chrono::duration<double, std::milli>(end_t-start_t).count()
         << "мс." << endl;

    // Редукция для поиска минимального элемента
    start_t = std::chrono::high_resolution_clock::now();
    result = handler('>', array, N, MAX_THREAD);
    end_t = std::chrono::high_resolution_clock::now();
    cout << "(Многопоточ) Другая операция: " << result << " "
         << "Время = " << std::chrono::duration<double, std::milli>(end_t-start_t).count()
         << "мс." << endl;

    return 0;
}


