# MULTITHREADING: XOR file contents

This project is designed to measure the performance of file I/O operations using multiple threads. The program calculates the XOR of file contents and measures the throughput of reading the file with different block sizes. It also includes functionalities to write to, read from, and seek within files to facilitate performance measurements.

## Directory Structure

- `main.c`: The main program file that orchestrates the file I/O operations, measures performance, and spawns threads for concurrent execution.
- `process_file.c`: Contains functions to process the file in different modes: writing, reading, and seeking.
- `helper.c`: Contains helper functions, including a function to measure the time taken for file operations.
- `process_file.h`: Header file for `process_file.c`.
- `helper.h`: Header file for `helper.c`.

## Compilation and Execution

### Prerequisites

- GCC compiler
- pthread library

### Compilation

To compile the project, use the following command:

```sh
gcc -o file_performance main.c process_file.c helper.c -lpthread -lm
```

### Execution

To execute the program, use the following command:

```sh
./file_performance <filename>
```

Here, `<filename>` is the name of the file you want to process.

### Example

```sh
./file_performance testfile.txt
```

## Project Components

### Main Program (`main.c`)

The main program performs the following tasks:

1. Parses the command-line arguments to get the filename.
2. Determines the optimal block size for reading the file by measuring the throughput for different block sizes.
3. Spawns multiple threads to read the file concurrently and calculate the XOR of the file contents.
4. Measures and prints the final XOR value and performance metrics (elapsed time and throughput).

### File Processing (`process_file.c`)

This module contains functions to process the file in different modes:

- `-w`: Write mode, which writes data to the file.
- `-r`: Read mode, which reads data from the file.
- `-l`: Seek mode, which uses `lseek` to read data from different positions in the file.

### Helper Functions (`helper.c`)

This module contains helper functions, including:

- `measure_time`: Measures the time taken to process the file in the specified mode (read or seek).

## Code Explanation

### Main Program

- **`calculate_xor`**: Calculates the XOR value of the file contents between specified offsets.
- **`read_file`**: Function executed by each thread to read a portion of the file and calculate the XOR value.
- **`calculate_final_xor`**: Combines the XOR values calculated by each thread.
- **`main`**: Orchestrates the entire process, including determining the optimal block size, spawning threads, and calculating the final XOR value.

### File Processing

- **`process_file`**: Processes the file in the specified mode (`-w`, `-r`, `-l`).

### Helper Functions

- **`measure_time`**: Measures the time taken for file operations in read or seek mode.

## Performance Measurement

The performance measurement includes:

1. **Throughput Calculation**: Measures the throughput of reading the file with different block sizes.
2. **Non-cached and Cached Operations**: Performs file operations with and without caching to measure performance under different conditions.

## Conclusion

This project provides a framework for measuring the performance of file I/O operations using multi-threading. It can be extended to include additional file operations and performance metrics as needed.


For any questions or issues: aashnakunk@gmail.com.
