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
#define NUM_THREADS 4
#define NUM_LATECYOP 1000000
#define WORKLOAD 1024*1024*1024

pthread_mutex_t mutex;
long shared_var;
long current_process;
int access_pattern;
int bsize,noperation,current_circle,read_times;
int nthread;
char *temp,*vx,*VA;

void *thoughput(void *args){
    //random access
    long tid=(long) args;
    if(access_pattern==0){
        while(current_circle<noperation){
            pthread_mutex_lock(&mutex);
            vx=(rand()%(WORKLOAD-bsize))+VA;
            memcpy(temp,vx,bsize);
            memset(vx, 'b', bsize);
            current_process++;
            if(current_process>read_times-1){
                current_circle++;
                current_process=0;
                // printf("Thread %ld working on RWR process:%ld in cirlce:%d\n", tid,current_process,current_circle);
                
                vx=VA;
            }
            pthread_mutex_unlock(&mutex);
            
            
        }
    }
    
    //sequence access
    if(access_pattern==1){
        
        while(current_circle<noperation){
            pthread_mutex_lock(&mutex);
            memcpy(temp,vx,bsize);
            memset(vx, 'b', bsize);
            vx=vx+bsize;
            current_process++;
            if(current_process>read_times-1){
                current_circle++;
                current_process=0;
                vx=VA;
            }
            // printf("Thread %ld working on RWS process:%ld in cirlce:%d\n", tid,current_process,current_circle);
            pthread_mutex_unlock(&mutex);
            
            
        }
        
    }
    
    pthread_exit(NULL);
    
}
int main(int argc, char **argv)
{
    clock_t start_t, end_t;
    double total_t;
    char bm_type[10];
    char block_size[10];
    
    //get user input
    // printf("<benchmark_type (RWS RWR)>");
    // scanf("%s",bm_type);
    
    // printf("<block size (1KB, 1MB, 10MB)> in bytes:");
    // scanf("%s",block_size);
    // printf("<number of threads (1,2,4)>");
    // scanf("%d",&nthread);
    
    if (argc != 4)
    {
        printf("You should input 3 arguments:<benchmark_type (RWS RWR LTC_RWS LTC_RWR)>, <block size (1KB, 1MB, 10MB)>, <number of threads (1,2,4)>\n");
        return(1);
    }
    strcpy(bm_type,argv[1]);
    strcpy(block_size,argv[2]);
    nthread = atoi(argv[3]);
    
    //locate 1GB blocks, get user input
    
    vx=VA=malloc(1024*1024*1024);
    memset(VA,'a',1024*1024*1024);
    
    if(strcmp(bm_type,"RWS")==0||strcmp(bm_type,"LTC_RWS")==0){
        access_pattern=1;
    }else if(strcmp(bm_type,"RWR")==0||strcmp(bm_type,"LTC_RWR")==0){
        access_pattern=0;
    }else{
        printf("Could not identify benchmark type");
    }
    
    if(strcmp(block_size,"1KB")==0){
        read_times=1024*1024;
        bsize=1024;
        noperation=100;
        
    }
    else if(strcmp(block_size,"1MB")==0){
        read_times=1024;
        bsize=1024*1024;
        noperation=100;
        
    }
    else if(strcmp(block_size,"10MB")==0){
        read_times=100;
        bsize=1024*1024*10;
        noperation=100;
        
    }
    else if(strcmp(bm_type,"LTC_RWR")==0||strcmp(bm_type,"LTC_RWS")==0){
        read_times=NUM_LATECYOP;
        bsize=1;
        noperation=1;
    }else{
        printf("Could not identify block size type");
    }
    
    temp=malloc(bsize);
    pthread_t threads[NUM_THREADS]; long tid; int rc;
    pthread_mutex_init(&mutex, NULL); shared_var = 0;
    
    //100G data for thoughput
    current_process=0;
    current_circle=0;
    start_t = clock();
    
    for (tid = 0; tid < nthread; tid++) {
        rc = pthread_create(&threads[tid], NULL, thoughput, (void *) tid);
        if (rc) {
            printf("Could not create thread %ld\n", tid);
        }
    }
    for (tid = 0; tid < nthread; tid++) {
        rc = pthread_join(threads[tid], NULL);
        if (rc) {
            printf("Could not join thread %ld\n", tid);
        }
    }
    
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    if(strcmp(bm_type,"LTC_RWR")==0||strcmp(bm_type,"LTC_RWS")==0){
        float thoughput=total_t/100;
        float theoreticalValue=100;
        float efficiency=thoughput/theoreticalValue;
        printf("%s  %d  %d  %.1f us  %.1f us  %.1f ", bm_type,nthread, bsize,thoughput,theoreticalValue, efficiency);
        
    }else{
        float thoughput=100/total_t;
        float theoreticalValue=54.6;
        float efficiency=(thoughput/theoreticalValue)*100;
        printf("%s  %d  %d  %.1f GB/sec  %.1f GB/sec  %.1f ", bm_type,nthread, bsize,thoughput,theoreticalValue, efficiency);
        
    }
    free(VA);
    free(temp);
    pthread_mutex_destroy(&mutex);
    //  pthread_exit(NULL);
    return 0;
    
}





