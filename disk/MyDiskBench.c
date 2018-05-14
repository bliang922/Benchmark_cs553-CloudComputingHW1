//
//  main.c
//  cs553 memorybenchmark
//
//  Created by liangbo on 05/03/2018.
//  Copyright © 2018 liangbo. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#define NUM_THREADS 128
#define WORKLOAD 1024*1024*1024


pthread_mutex_t mutex;
long current_process;
long operate_times;
int bsize,current_circle,read_times;
int nthread,nthread_latency;
FILE *f;

void *write_sequence(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    memset(buffer, '0', bsize);
    
    while(current_process<operate_times){
        // write to disk sequentially
        fwrite(buffer, 1, bsize, f);
        current_process++;
    }
    
    free(buffer);
    pthread_exit(NULL);
}

void *read_sequence(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    
    while(current_process<operate_times){
        // write to disk sequentially
        fread(buffer, 1, bsize, f);
        current_process++;
    }
    free(buffer);
    pthread_exit(NULL);
    
}

void *read_random(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    long fpos=0;
    
    while(current_process<operate_times){
        // write to disk sequentially
        fpos = (rand() % ((long int) WORKLOAD / (long int)bsize)) * bsize;
        fseek(f, fpos, SEEK_SET);
        fread(buffer, 1, bsize, f);
        current_process++;
    }
    free(buffer);
    pthread_exit(NULL);
    
}

void *write_random(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    long fpos=0;
    memset(buffer, '0', bsize);
    
    while(current_process<operate_times){
        // write to disk sequentially
        fpos = (rand() % ((long int) WORKLOAD / (long int)bsize)) * bsize;
        fseek(f, fpos, SEEK_SET);
        fwrite(buffer, 1, bsize, f);
        current_process++;
    }
    
    free(buffer);
    pthread_exit(NULL);
}



int main(int argc, char **argv)
{
    
    
    //open a binary file for writhing in binary mode, if file exists, rewrite it; else create new file.
    
    
    clock_t start_t, end_t;
    double total_t;
    char bm_type[10];
    char block_size[10];
    pthread_t threads[NUM_THREADS]; long tid; int rc;
    
    //get user input
    //  printf("<mode (RS WS RR WR LTC_RR LTC_WR)>");
    //  scanf("%s",bm_type);
    
    //  printf("<block size (1KB, 1000KB, 10000KB,100000KB)> in bytes:");
    //  scanf("%s",block_size);
    //  printf("<number of threads (1,2,4)>");
    //  scanf("%d",&nthread);
    //  printf("<number of threads (1,2,4,8,16,32,64,128)>");
    //  scanf("%d",&nthread_latency);
    
    if (argc != 4)
    {
        printf("You should input 4 arguments:<benchmark_type (RS WS RR WR LTC_RR LTC_WR)>, <block size (<block size (1KB, 1000KB, 10000KB,100000KB)> in bytes:)>, <number of threads (1,2,4)>,<number of threads for latency (1,2,4,8,16,32,64,128)>\n");
        return(1);
    }
    strcpy(bm_type,argv[1]);
    strcpy(block_size,argv[2]);
    nthread = atoi(argv[3]);
    if(strcmp(bm_type,"LTC_RR")==0||strcmp(bm_type,"LTC_WR")==0) nthread_latency=nthread;

    if(strcmp(block_size,"1KB")==0||strcmp(block_size,"1")==0)            bsize=1024;
    else if(strcmp(block_size,"1000")==0||strcmp(block_size,"1000KB")==0)  bsize=1024*1024;
    else if(strcmp(block_size,"10000")==0||strcmp(block_size,"10000KB")==0)bsize=1024*1024*10;
    else if(strcmp(block_size,"100000")==0||strcmp(block_size,"100000KB")==0)bsize=1024*1024*100;
    else printf("Could not identify block size type");
    operate_times=(long int)WORKLOAD/(long int)bsize;
    current_process=0;
    
    if(strcmp(bm_type,"RS")==0){
        f = fopen("new_file.bin", "r");
        if (f == NULL) {
            perror("Error");
        }
        //disable the buffer so that following process are executed directly to disk
        setbuf(f, NULL);
        fseek(f, 0, SEEK_SET);
        
        start_t = clock();
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, read_sequence, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
        
    }else if(strcmp(bm_type,"WS")==0){
        f = fopen("new_file.bin", "w");
        if (f == NULL) {
            perror("Error");
        }
        //disable the buffer so that following process are executed directly to disk
        setbuf(f, NULL);
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, write_sequence, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
    }else if(strcmp(bm_type,"RR")==0){
        f = fopen("new_file.bin", "r");
        if (f == NULL) {
            perror("Error");
        }
        //disable the buffer so that following process are executed directly to disk
        setbuf(f, NULL);
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, read_random, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
    }else if(strcmp(bm_type,"WR")==0){
        f = fopen("new_file.bin", "w");
        if (f == NULL) {
            perror("Error");
        }
        //disable the buffer so that following process are executed directly to disk
        setbuf(f, NULL);
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, write_random, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
    }else if(strcmp(bm_type,"LTC_WR")==0){
        f = fopen("new_file.bin", "r");
        if (f == NULL) {
            perror("Error");
        }
        //disable the buffer so that following process are executed directly to disk
        setbuf(f, NULL);
        bsize=1024;
        operate_times=(long int)WORKLOAD/((long int)bsize*10);
        
        start_t = clock();
        
        for (tid = 0; tid < nthread_latency; tid++) {
            rc = pthread_create(&threads[tid], NULL, write_random, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
    }else if(strcmp(bm_type,"LTC_RR")==0){
        f = fopen("new_file.bin", "r");
        if (f == NULL) {
            perror("Error");
        }
        //disable the buffer so that following process are executed directly to disk
        setbuf(f, NULL);
        bsize=1024;
        operate_times=(long int)WORKLOAD/(long int)bsize;
        
        start_t = clock();
        
        for (tid = 0; tid < nthread_latency; tid++) {
            rc = pthread_create(&threads[tid], NULL, read_random, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
    }else{
        printf("Could not identify benchmark type");
        return 0;
    }
    for (tid = 0; tid < nthread; tid++) {
        rc = pthread_join(threads[tid], NULL);
        if (rc) {
            printf("Could not join thread %ld\n", tid);
        }
    }
    
    end_t = clock();
    fclose(f);
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    if(strcmp(bm_type,"LTC_WR")==0||strcmp(bm_type,"LTC_RR")==0){
        float thoughput=total_t/102;
        float theoreticalValue=50;
        float efficiency=thoughput/theoreticalValue;
        printf("%s  %d  %d  %.2f ms  %.1f ms  %.2f ", bm_type,nthread, bsize,thoughput,theoreticalValue, efficiency);
        
    }else{
        float thoughput=1024/total_t;
        float theoreticalValue=540;
        float efficiency=(thoughput/theoreticalValue)*100;
        printf("%s  %d  %d  %.1f MB/sec  %.1f MB/sec  %.1f ", bm_type,nthread, bsize,thoughput,theoreticalValue, efficiency);
        
    }
    return 0;
    
}





