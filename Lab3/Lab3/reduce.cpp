#include "reduce.h"

void *runTH(void *arg)
{
    // Получили параметры
    task *t = (task *)arg;
    int *array = t->array;
    int size = t->size;
    int (*x)(int, int) = t->x;
    int ressult = t->res;

    for (int i = 0; i < size; i++)
        ressult = x(ressult, array[i]);
    t->res = ressult;
    return t;
}

int handler(char operation, int *array, int size, int MAX_THREAD) {
    int nachal = 0;
    int (*x)(int, int);
    switch (operation)
    {
    case '*':
        x =  [](int a, int b){return a * b;};
        nachal = 1;
        break;
    case '^':
        x =  [](int a, int b){return a ^ b;};
        nachal = 0;
        break;
    case '+':
        x =  [](int a, int b){return a + b;};
        nachal = 0;
        break;
    case '-':
        x =  [](int a, int b){return a - b;};
        nachal = 0;
        break;
    case '<':
        x =  [](int a, int b){return (a < b)? a : b;};
        nachal = array[0];
        break;
    case '>':
        x =  [](int a, int b){return (a > b)? a : b;};
        nachal = array[0];
        break;
    }

    std::vector <pthread_t> ths;
    task *t;
    pthread_t th;
    std::vector <int> result;
    std::vector <task *> tasks;
    int cp = size / MAX_THREAD, temp = 0;
    if(cp == 0)
        cp = 1;
    for (int i = 0; i < MAX_THREAD; ++i) {
        if (i != MAX_THREAD - 1)
        {
            task *t1 = new task(x, array + temp, cp, nachal);
            tasks.push_back(t1);
            pthread_create(&th, NULL, runTH, t1);
            ths.push_back (th);
            temp += cp;
        }
        else
        {
            task *t2 = new task(x, array + temp, size - temp, nachal);
            tasks.push_back(t2);
            pthread_create(&th, NULL, runTH, t2);
            ths.push_back (th);
        }
    }
    void *status;
    for (auto& thread : ths) {
        pthread_join(thread, &status);
        t = (task *)status;
        result.push_back(t->res);
    }
    int res = nachal;
    for (int i = 0; i < (int)result.size(); ++i)
        res = x(res, result[i]);
    return res;
}
