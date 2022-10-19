#include <iostream>
#include <list>
#include <iterator>

std::list<long> list;

bool is_prime(long n) {
    if (n <= 1)
        return false;

    for (long i = 2; i < n; i++)
        if (n % i == 0) return false;

    return true;
}

int check(long data) {
    for (long i = 1; i <= (data / 2) + 1; ++i) {
        if (data % i == 0) {
            list.push_back(i);
        }
    }

    for (long item : list)
        if (!is_prime((data / item) + item)) return 0;

    return 1;
}

int main(int argc, char *argv[]) {
    long input = std::stol(argv[1]);
    struct timespec start{}, end{};
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    for (; !check(input); input++)
        list.clear();
    std::cout << input << std::endl;

    copy(list.begin(), list.end(), std::ostream_iterator<long>(std::cout, " "));
    std::cout << std::endl;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    float time = (((end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3) / 1000.0) / 1000.0;
    std::cerr << time << std::endl;

    return 0;
}