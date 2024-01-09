#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>

/*
 * Output format: index, time taken for accessing elements of the array, time taken for accessing random elements of the array
 */

const int N = 1000000;       // Adjust the size of the array to suit your needs
const int EXPERIMENTS = 100; // Number of experiments

// Function to measure access time for different scenarios
void measureAccessTime(int arr[N], std::vector<int> &indices, std::vector<long long> &elapsed_times)
{
    int sum = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
    {
        sum += arr[i];
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    elapsed_times.push_back(elapsed_time);

    start_time = std::chrono::high_resolution_clock::now();
    for (int i : indices)
    {
        sum += arr[i];
    }
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    elapsed_times.push_back(elapsed_time);
}

// Function to create a directory if it doesn't exist
bool createDirectory(const char *path)
{
#ifdef _WIN32
    struct stat info;
    if (stat(path, &info) != 0 || !(info.st_mode & S_IFDIR))
        return _mkdir(path) == 0;
#else
    struct stat info;
    if (stat(path, &info) != 0 || !(info.st_mode & S_IFDIR))
        return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
#endif

    return true; // Return true if the directory already exists
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // Create the output directory if it doesn't exist
    if (!createDirectory("./scripts/dataFiles"))
    {
        std::cerr << "Error creating the output directory.\n";
        return 1; // Return an error code
    }

    // Create an array
    int arr[N];

    // Generate a vector of random indices
    std::vector<int> indices;
    for (int i = 0; i < EXPERIMENTS; ++i)
    {
        indices.push_back(rand() % N);
    }

    // Open the CSV file for writing
    std::ofstream outputFile("./scripts/dataFiles/test.csv");

    // Check if the file is successfully opened
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening the output file.\n";
        return 1; // Return an error code
    }

    // Perform multiple experiments
    std::vector<std::vector<long long>> results;

    for (int exp = 0; exp < EXPERIMENTS; ++exp)
    {
        std::vector<long long> elapsed_times;

        // Start up cache by accessing the array once
        int sum = 0;
        for (int i = 0; i < N; ++i)
        {
            sum += arr[i];
        }

        // Measure access time for different scenarios
        measureAccessTime(arr, indices, elapsed_times);

        // Store results for this experiment
        results.push_back(elapsed_times);
    }

    // Print results to the CSV file
    for (int exp = 0; exp < EXPERIMENTS; ++exp)
    {
        outputFile << exp << ",";
        for (size_t i = 0; i < results[exp].size(); ++i)
        {
            outputFile << results[exp][i];
            if (i < results[exp].size() - 1)
            {
                outputFile << ",";
            }
        }
        outputFile << "\n";
    }

    // Close the CSV file
    outputFile.close();

    std::cout << "Results written to ./scripts/dataFiles/test.csv\n";

    return 0;
}
