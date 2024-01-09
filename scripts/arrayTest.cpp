#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Output format: time taken by the loop
 * runs many times for the same array size to see cache effects
 * Uses same array size each time and runs experiments many times
 */

const int arraySize = 1000000;
const int numTests = 100;

// Function to create a directory if it doesn't exist
bool createDirectory(const char *path)
{
#ifdef _WIN32
    return _mkdir(path) == 0;
#else
    struct stat info;
    if (stat(path, &info) != 0)
    {
        // Directory doesn't exist, create it
        return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
    }
    else if (info.st_mode & S_IFDIR)
    {
        // Directory already exists
        return true;
    }
    else
    {
        // Path exists but is not a directory
        return false;
    }
#endif
}

int main()
{
    int *data = new int[arraySize];

    // Initialize the array
    for (int i = 0; i < arraySize; ++i)
    {
        data[i] = i;
    }

    std::srand(42); // Seed for number generation

    // Print the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        std::cout << "Current working directory: " << cwd << std::endl;
    }
    else
    {
        perror("getcwd() error");
        return 1;
    }

    // Create the output directory if it doesn't exist
    if (!createDirectory("./scripts/dataFiles"))
    {
        std::cerr << "Error creating the output directory.\n";
        return 1; // Return an error code
    }

    // Open the CSV file for writing
    std::ofstream outputFile("./scripts/dataFiles/arrayTest.csv");

    // Check if the file is successfully opened
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening the output file.\n";
        return 1; // Return an error code
    }

    for (int test = 0; test < numTests; ++test)
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        // Access the array elements with a more random pattern
        long long sum = 0;
        for (int i = 0; i < arraySize; ++i)
        {
            int index = std::rand() % arraySize;
            sum += data[index];
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

        // Print results to the console
        std::cout << "Test " << test + 1 << ": ";
        std::cout << "Sum: " << sum << ", ";
        std::cout << "Time taken by the loop: " << duration.count() << " microseconds" << std::endl;

        // Print results to the CSV file
        outputFile << test + 1 << "," << duration.count()  << "," << sum  << "\n";
    }

    // Close the CSV file
    outputFile.close();

    std::cout << "Results written to ./scripts/dataFiles/arrayTest.csv\n";

    delete[] data;
    return 0;
}
