#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "x86intrin.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define N (13421772800)
#define START_N (128)

/*
* Output format: Size in bytes, Avg Time per iteration
* Iterations 10240000
* Working memory: 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, ..., 67108864
* Different working memory sizes are tested in powers of 2
*/

extern uint64_t _access_random_place_to_place_memory_dep(uint64_t *, size_t, size_t);

void place_random_shuffle(uint64_t *p, uint64_t max_offset) {
    uint64_t max_offset_q = max_offset / 8;

    // start by writing for each qword its own offset
    for (uint64_t i = 0; i < max_offset_q; i++) {
        p[i] = 8 * i;
    }

    // then shuffle (Fisher Yates shuffling)
    for (uint64_t i = 0; i < max_offset_q - 1; i++) {
        uint64_t t;
        uint64_t j = (rand() % (max_offset_q - i)) + i;
        t = p[i];
        p[i] = p[j];
        p[j] = t;
    }
}

// Function to create a directory
int createDirectory(const char *path) {
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

// Function to check if a directory exists
int directoryExists(const char *path) {
    struct stat info;
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

int main(int argc, char **argv) {
    unsigned long long n = N;
    unsigned long long int loops = 10240000;

    // create buffer of random memory
    uint64_t *p = malloc(n);
    uint64_t res;

    // result arrays
    double results[64];
    size_t memories[64];
    int i = 0; // Initialize i

    // Check if the output directory exists, create it if not
    const char *outputDir = "./scripts/dataFiles";
    if (!directoryExists(outputDir)) {
        if (createDirectory(outputDir) != 0) {
            fprintf(stderr, "Error creating the output directory.\n");
            return 1; // Return an error code
        }
    }

    // Open the CSV file for writing
    FILE *outputFile = fopen("./scripts/dataFiles/test2.csv", "w");

    // Check if the file is successfully opened
    if (!outputFile) {
        fprintf(stderr, "Error opening the output file.\n");
        return 1; // Return an error code
    }

    for (int working_memory = START_N; working_memory < n && i < 66; working_memory <<= 1) {
        place_random_shuffle(p, working_memory);

        clock_t start_time = clock();
        res = _access_random_place_to_place_memory_dep(p, working_memory, loops);
        clock_t end_time = clock();

        double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

        memories[i] = working_memory;
        results[i] = elapsed_time / loops;

        // Print results to the CSV file
        fprintf(outputFile, "%zu, %.12f\n", memories[i], results[i]);

        i++;
    }

    free(p);

    // Close the CSV file
    fclose(outputFile);

    printf("Results written to ./scripts/dataFiles/test2.csv\n");

    return res;
}
