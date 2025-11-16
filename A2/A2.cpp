#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <sstream>

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

void insertion_sort(std::vector<int>& v) {
    if (v.size() <= 1) return;
    for (auto next = v.begin() + 1; next != v.end(); next++) {
        auto cur = next;
        while (cur != v.begin()) {
            auto cur2 = cur--;
            if (*cur > *cur2) std::swap(*cur, *cur2);
        }
    }
}

std::vector<int>& merge(std::vector<int>& target, const std::vector<int>& first, const std::vector<int>& second) {
    auto b = target.begin();
    auto b1 = first.begin(), b2 = second.begin();
    while (b != target.end()) {
        if (b1 != first.end()) {
            if (b2 != second.end()) {
                if (*b1 < *b2) *(b++) = *(b1++);
                else *(b++) = *(b2++);
            } else *(b++) = *(b1++);
        } else *(b++) = *(b2++);
    }
    return target;
}

void merge_sort(std::vector<int>& v, size_t& breakpoint) {
    if (v.size() <= breakpoint) {
        insertion_sort(v);
        return;
    }
    std::vector l(v.begin(), v.begin() + v.size() / 2), r(v.begin() + v.size() / 2, v.end()); 
    merge_sort(l, breakpoint);
    merge_sort(r, breakpoint);
    merge(v, l, r);
}

int main(int argv, char* argc[]) {
    std::ofstream table;
    std::string filename(argc[1]);
    filename += ".csv";
    size_t breakpoint = std::atol(argc[2]);
    ArrayGenerator generator(123, 100000, 0, 6000);
    table.open(filename);
    table << "Size;Random Array Sorting Time;Reverse Array Sorting Time;Nearly Sorted Array Sorting Time\n";
    for (int i = 500; i < 100000; i += 100) {
        std::vector<int> randomArray = generator.getRandomArray(i);
        std::vector<int> reverseArray = generator.getReverseArray(i);
        std::vector<int> nearlySortedArray = generator.getNearlySortedArray(i);
        auto start = std::chrono::high_resolution_clock::now();
        merge_sort(randomArray, breakpoint);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long msec_random = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        start = std::chrono::high_resolution_clock::now();
        merge_sort(reverseArray, breakpoint);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        long long msec_reverse = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        start = std::chrono::high_resolution_clock::now();
        merge_sort(nearlySortedArray, breakpoint);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        long long msec_nearly_sorted = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        table << i << ";" << msec_random << ";" << msec_reverse << ";" << msec_nearly_sorted << "\n";
    }
    table.close();
    return 0;
}