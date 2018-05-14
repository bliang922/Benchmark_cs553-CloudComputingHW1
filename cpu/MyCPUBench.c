//
//  main.c
//  cpuBenchMark
//
//  Created by liangbo on 26/03/2018.
//  Copyright © 2018 liangbo. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>

#define NUM_THREADS 4
#define WORKLOAD 1024*1024


pthread_mutex_t mutex;
char bm_type[10];
long current_process;
long operate_times;
long circle;
int nthread;

void *QP(void *args){
    char ch1='a';
    char ch2='b';
    while(circle<50000){
        while(current_process<WORKLOAD)
        {
            if(ch1<ch2) current_process++;
            else current_process++;
        }
        circle++;
        current_process=0;
        
    }
    
    pthread_exit(NULL);
}

void *HP(void *args){
    short sht1=1;
    short sht2=2;
    while(circle<50000){
        while(current_process<WORKLOAD)
        {
            if(sht1<sht2) current_process++;
            else current_process++;
        }
        circle++;
        current_process=0;
        
    }
    pthread_exit(NULL);
    
}
void *SP(void *args){
    int int1=10000;
    int int2=20000;
    while(circle<50000){
        while(current_process<WORKLOAD)
        {
            if(int1<int2) current_process++;
            else current_process++;
        }
        circle++;
        current_process=0;
        
    }
    pthread_exit(NULL);
    
}void *DP(void *args){
    double db1=10000000000.0;
    double db2=20000000000.0;
    while(circle<50000){
        while(current_process<WORKLOAD)
        {
            if(db1<db2) current_process++;
            else current_process++;
        }
        circle++;
        current_process=0;
        
    }
    pthread_exit(NULL);
    
}



int main(int argc, char **argv)
{
    
    clock_t start_t, end_t;
    double total_t;
    pthread_t threads[NUM_THREADS];
    long tid;
    int rc;
    
    
   if (argc != 3)
   {
        printf("You should input 4 arguments:<mode (QP HP SP DP)>, <number of threads (1,2,4)>\n");
        return(1);
   }
   strcpy(bm_type,argv[1]);
    nthread = atoi(argv[2]);
    
    
    //get user input
     // printf("<benchmark_type (QP HP SP DP)>");
     // scanf("%s",bm_type);
    
     // printf("<number of threads (1,2,4)>");
     //  scanf("%d",&nthread);
    operate_times=(long int)WORKLOAD;
    circle=0;
    current_process=0;
    
    if(strcmp(bm_type,"QP")==0){
        
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            
            rc = pthread_create(&threads[tid], NULL, QP, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
                exit(-1);
            }
        }
        
    }else if(strcmp(bm_type,"HP")==0){
        
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            
            rc = pthread_create(&threads[tid], NULL, HP, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
                exit(-1);
            }
        }
        
    }else if(strcmp(bm_type,"SP")==0){
        
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            
            rc = pthread_create(&threads[tid], NULL, SP, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
                exit(-1);
            }
        }
        
    }else if(strcmp(bm_type,"DP")==0){
        
        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            
            rc = pthread_create(&threads[tid], NULL, DP, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
                exit(-1);
            }
        }
        
    }  else{
        printf("Could not identify bm type");
        return -1;
    }
    
    for (tid = 0; tid < nthread; tid++) {
        rc = pthread_join(threads[tid], NULL);
        if (rc) {
            printf("Could not join thread %ld\n", tid);
        }
    }
    
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    float thoughput=50/total_t;
    float theoreticalValue=2355;
    float efficiency=(thoughput/theoreticalValue)*100;
    printf("%s  %d  %.2f GigaOPS/sec  %.2f GigaOPS/sec  %.2f ", bm_type,nthread,thoughput,theoreticalValue, efficiency);
    
}







