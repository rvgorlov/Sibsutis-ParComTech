#ifndef REDUCE_H
#define REDUCE_H

#include <iostream>
#include <thread>
#include <pthread.h>
#include <vector>

struct task {
    int (*x)(int, int), *array, size, res;
    task(int (*xx)(int, int), int *m, int c, int r): x(xx), array(m), size(c), res(r){}
};

void *runTH(void *);
int handler(char, int *, int, int);

#endif // REDUCE_H
