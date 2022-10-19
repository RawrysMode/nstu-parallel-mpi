#include <ctime>
#include <cstdio>
#include <string>
#include <list>
#include <iostream>
#include <mpi.h>
#include <iterator>

int world_rank;
int complete;
std::list<long> list;
MPI_Request request;

bool is_prime(long n) {
    if (n <= 1) return false;

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

    for (long item : list) {
        MPI_Test(&request, &complete, MPI_STATUS_IGNORE);

        if (complete) return 0;
        if (!is_prime((data / item) + item)) return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    int world_size;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc < 2) {
        std::cout << "not enough arguments" << std::endl;
        return 1;
    }

    if (world_rank == 0) {
        struct timespec start{}, end{};
        long input = std::stol(argv[1]);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

        for (int i = 1; i < world_size; i++) {
            long in = input;
            MPI_Send(&in, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
        }

        long result;
        MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%ld\n", result);

        copy(list.begin(), list.end(), std::ostream_iterator<long>(std::cout," "));

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        float time = (((end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3) / 1000.0) / 1000.0;
        std::cerr << time << std::endl;

        for (int i = 1; i < world_size; i++) {
            MPI_Send(&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

    } else {
        long current = 0;
        MPI_Recv(&current, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        current += world_rank - 1;

        int buffer;
        MPI_Irecv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &complete, MPI_STATUS_IGNORE);

        for (; !complete && !check(current); current += (world_size - 1)) list.clear();

        if (complete) goto end;

        MPI_Send(&current, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
    }
    end:
    MPI_Finalize();
}