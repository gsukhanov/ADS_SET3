#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>

std::mt19937 gg(57);
std::uniform_int_distribution<int> rand_int(0, 1e6);

class ArrayGenerator {
private:
    std::vector<int> randomArray;
    std::vector<int> forwardArray;
    std::vector<int> reverseArray;
    std::mt19937 g;
    std::uniform_int_distribution<int> dist;
public:
    ArrayGenerator(int n, size_t size, int min, int max) : g(n), dist(min, max),
                            randomArray(size), forwardArray(size), reverseArray(size) {
        for (size_t i = 0; i < size; i++) {
            randomArray[i] = dist(g);
            forwardArray[i] = i;
            reverseArray[i] = size - i;
        }
    }
    const std::vector<int>& getRandomArray(size_t size) {
        static std::vector<int> array;
        array.assign(randomArray.begin(), randomArray.begin() + size);
        return array;
    }
    const std::vector<int>& getReverseArray(size_t size) {
        static std::vector<int> array;
        array.assign(reverseArray.begin(), reverseArray.begin() + size);
        return array;
    }
    const std::vector<int>& getNearlySortedArray(size_t size) {
        static std::vector<int> array;
        array.assign(forwardArray.begin(), forwardArray.begin() + size);
        for (size_t i = 0; i < size / 100; i++) std::swap(array[dist(g) % size], array[dist(g) % size]);
        return array;
    }
};

void insertion_sort(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
    if (end - begin < 2) return;
    for (auto next = begin + 1; next != end; next++) {
        auto cur = next;
        while (cur != begin) {
            auto cur2 = cur--;
            if (*cur > *cur2) std::swap(*cur, *cur2);
        }
    }
}

void heapify(std::vector<int>::iterator begin, size_t i, size_t n) {
    size_t l;
    while ((l = 2 * i + 1) < n) {
        if (l < n - 1 && begin[l] < begin[l + 1]) ++l;
        if (begin[i] >= begin[l]) break;
        std::swap(begin[i], begin[l]);
        i = l;
    }
}

void buildMaxHeap(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
    size_t size = end - begin;
    for (size_t i = size / 2; i > 0; --i) heapify(begin, i - 1, size);
}

void heapSort(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
    size_t size = end - begin;
    buildMaxHeap(begin, end);
    for (size_t i = size; i > 0; --i) {
        std::swap(begin[i - 1], begin[0]);
        heapify(begin, 0, i - 1);
    }
}

void quickSort(std::vector<int>::iterator begin, std::vector<int>::iterator end, int depth, int lim, int breakpoint) {
    size_t size = end - begin;
    if (size <= breakpoint) {
        insertion_sort(begin, end);
        return;
    }
    if (depth >= lim) {
        heapSort(begin, end);
        return;
    }
    int pivot = rand_int(gg) % (end - begin);
    std::swap(begin[pivot], *(end - 1));
    int pivot_value = *(end - 1);
    
    auto i = begin;
    for (auto j = begin; j < end - 1; ++j) {
        if (*j <= pivot_value) {
            std::swap(*i, *j);
            ++i;
        }
    }
    std::swap(*i, *(end - 1));
    
    quickSort(begin, i, depth + 1, lim, breakpoint);
    quickSort(i + 1, end, depth + 1, lim, breakpoint); ;
}

class SortTester {
public:
    static long long test(std::vector<int> array, int lim, int breakpoint){
        auto start = std::chrono::high_resolution_clock::now();
        quickSort(array.begin(), array.end(), 0, lim ,breakpoint);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }
};

int main(int argv, char* argc[]) {
    std::ofstream table;
    std::string filename(argc[1]);
    filename += ".csv";
    size_t breakpoint = std::atol(argc[2]);
    size_t upper = std::atol(argc[3]);
    ArrayGenerator generator(123, upper, 0, 6000);
    table.open(filename);
    table << "Size;Random Array Sorting Time;Reverse Array Sorting Time;Nearly Sorted Array Sorting Time\n";
    for (int i = 500; i < upper; i += 100) {
        int lim = 2 * static_cast<int>(log2(i));
        if (breakpoint == 1) lim = __INT_MAX__;
        std::vector<int> randomArray = generator.getRandomArray(i);
        std::vector<int> reverseArray = generator.getReverseArray(i);
        std::vector<int> nearlySortedArray = generator.getNearlySortedArray(i);
        long long msec_random = SortTester::test(randomArray, breakpoint, lim);
        long long msec_reverse = SortTester::test(reverseArray, breakpoint, lim);
        long long msec_nearly_sorted = SortTester::test(nearlySortedArray, breakpoint, lim);
        table << i << ";" << msec_random << ";" << msec_reverse << ";" << msec_nearly_sorted << "\n";
    }
    table.close();
    return 0;
}
