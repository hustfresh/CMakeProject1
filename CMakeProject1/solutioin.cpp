#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include<algorithm>
#include<cfloat>
#include<stdio.h>
#include<omp.h>
using namespace std;
void optimized_pre_phase1(size_t) {}

void optimized_post_phase1() {}

void optimized_pre_phase2(size_t) {}

void optimized_post_phase2() {}

static int cmp(void const* a, void const* b) {
    return *(float*)a < *(float*)b ? -1 : 1;
}
static size_t Partition(float* data, size_t start, size_t end)   //划分数据
{
    float temp = data[start];
    while (start < end) {
        while (start < end && data[end] >= temp)end--;
        data[start] = data[end];
        while (start < end && data[start] <= temp)start++;
        data[end] = data[start];
    }
    data[start] = temp;
    return start;
}
static void quickSort(float* data, size_t start, size_t end) {
    if (start < end) {
        //if (end - start <= 9) { insertionSort(data, start, end); return; }
        size_t pos = Partition(data, start, end);
#pragma omp parallel sections  
        {
#pragma omp section        
            quickSort(data, start, pos - 1);
#pragma omp section      
            quickSort(data, pos + 1, end);
        }
    }
}
void optimized_do_phase1(float* data, size_t size) {
    quickSort(data, 0, size - 1);
}
void optimized_do_phase2(size_t* result, float* data, float* query, size_t size) {
    pair<float, size_t>* q = (pair<float, size_t>*)malloc(sizeof(pair<float, size_t>) * (size + 1));
    size_t l = 0,query_pos=0;
    float query_current;
#pragma omp parallel for
    for (l; l < size; ++l) {
        q[l] = pair<float, size_t>(query[l], l);
    }
    q[size] = pair<float, size_t>(FLT_MIN, 0);
    qsort(q, size, sizeof(q[0]), cmp);
    l = 0; 
    while (query_pos < size) {
        query_current = q[query_pos].first;
        while (data[l] < query_current)
            ++l;
        result[q[query_pos++].second] = l;
        while (q[query_pos].first == query_current) {
            result[q[query_pos++].second] = l;
        }
    }
    free(q);
}