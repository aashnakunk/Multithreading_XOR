#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include"process_file.h"
#include "helper.h"
#include <math.h>
#include <sys/stat.h>

struct ThreadData {
    char *filename;
    long block_size;
    long start_offset;
    long block_count;
    long end_offset;
    unsigned int xor_value;
    double elapsed_time;
};

unsigned int calculate_xor(char *filename, off_t start_offset, off_t end_offset, long block_size) {
    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    lseek(file_descriptor, start_offset, SEEK_SET);
    unsigned int xor_value = 0;

    long buffer_size = end_offset - start_offset;
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    long bytes_read;
    while ((bytes_read = read(file_descriptor, buffer, buffer_size)) > 0) {
        for (long i = 0; i < bytes_read; i++) {
            xor_value ^= buffer[i];
        }
        if (lseek(file_descriptor, 0, SEEK_CUR) >= end_offset|| bytes_read < buffer_size) {
            break;
        }
    }

    close(file_descriptor);

    return xor_value;
}

void *read_file(void *data) {
    struct ThreadData *thread_data = (struct ThreadData *)data;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    thread_data->xor_value = calculate_xor(thread_data->filename, thread_data->start_offset, thread_data->end_offset, thread_data->block_size);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    thread_data->elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    double throughput = ((thread_data->end_offset - thread_data->start_offset) / thread_data->elapsed_time) / (1024*1024);

    // printf(" Thread ID %ld: Elapsed Time: %.6f seconds, Throughput: %.6f MiBs/s Intermediate XOR Value: %02X\n", pthread_self(), thread_data->elapsed_time, throughput,thread_data->xor_value);

    pthread_exit(NULL);
}

unsigned int calculate_final_xor(struct ThreadData *thread_data, int num_threads) {
    unsigned int final_xor = 0;
    for (int i = 0; i < num_threads; i++) {
        final_xor ^= thread_data[i].xor_value;
    }
    return final_xor;
}

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    float maxmib=0; 
    int max_blocksize=0;
    for (int i = 2; i <= 10000; i *= 2) {
        process_file("test6.txt", "-w", i, 1, false);
        float bytes_per_sec = (i / measure_time("test6.txt", 1, i, 0));
        float mib = bytes_per_sec / pow(2, 20);
        if(mib > maxmib){
            maxmib = mib;
            max_blocksize = i;
        }
        // printf("Block size: %d %.2f, ", i, mib);
        // printf("\n");
    }

    struct stat st;
    if (stat(filename, &st) == -1) {
        
        perror("Error getting file size");
        exit(EXIT_FAILURE);
    }

    long file_size = (long long)st.st_size;
    // printf("The file size is %lld bytes \n",(long long)file_size);
    int block_count = (file_size + max_blocksize - 1) / max_blocksize; // Ceiling division
    int NUM_THREADS;
    if(block_count > 10){
        NUM_THREADS = 10;
    }else{
        NUM_THREADS = block_count;
    }
    // printf("Spawning %d threads\n", NUM_THREADS);
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].filename = filename;
        thread_data[i].block_size = max_blocksize;

        thread_data[i].start_offset = i * ((long long)file_size / NUM_THREADS);

        thread_data[i].end_offset = (i + 1) * (file_size / NUM_THREADS);
        if (i == NUM_THREADS - 1) {
            thread_data[i].end_offset = file_size;
        }
  

        thread_data[i].xor_value = 0;
    }

    system("sudo sh -c '/usr/bin/echo 3 > /proc/sys/vm/drop_caches'");
    // printf("doing non-cached\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, read_file, (void *)&thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    unsigned int final_xor = calculate_final_xor(thread_data, NUM_THREADS);
    printf("Final XOR value: %08X\n", final_xor);

    // printf("doing cached\n");

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, read_file, (void *)&thread_data[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    final_xor = calculate_final_xor(thread_data, NUM_THREADS);

    printf("Final XOR value: %08X\n", final_xor);

    return 0;
}