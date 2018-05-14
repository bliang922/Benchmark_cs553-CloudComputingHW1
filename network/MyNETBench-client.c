//
//  main.c
//  cs553 memorybenchmark 104.194.115.42
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define NUM_THREADS 128
#define WORKLOAD 1024*1024*1024


pthread_mutex_t mutex;
char bm_type[10];
long current_process;
long operate_times;
int bsize;
int nthread;
struct sockaddr_in ser_add;
int sockaddr_in_size = sizeof(struct sockaddr_in);

void *udp_client(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    memset(buffer, '0', bsize);
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    while(current_process<operate_times){
        sendto(sockfd, buffer, bsize, 0, (struct sockaddr *)&ser_add, sizeof(struct sockaddr));
        if(strcmp(bm_type,"LTC_UDP")==0){
            recvfrom(sockfd, buffer, bsize, 0, (struct sockaddr *)&ser_add, &sockaddr_in_size);
        }
        current_process++;

    }
    close(sockfd);
    free(buffer);
    pthread_exit(NULL);
}

void *tcp_client(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    memset(buffer, '0', bsize);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int rt = connect(sockfd, (struct sockaddr *) &ser_add, sizeof(struct sockaddr));
    if(rt == -1)
    {
        printf("thread %ld set connection failed\n",(long)args);
        exit(-1);
    }else{
        printf("thread %ld set connection success\n",(long)args);

    }
    while(current_process<operate_times){
        send(sockfd, buffer, bsize, 0);
        if(strcmp(bm_type,"LTC_TCP")==0){
            recv(sockfd, buffer, bsize, 0);
        }
        current_process++;
    }
    close(sockfd);
    free(buffer);
    pthread_exit(NULL);
    
}

int main(int argc, char **argv)
{
    
    clock_t start_t, end_t;
    double total_t;
    char ser_ip[20];
    char block_size[10];
    pthread_t threads[NUM_THREADS]; long tid; int rc;
    
    //get user input
   // printf("<mode (TCP UDP LTC_TCP LTC_UDP)>");
    //scanf("%s",bm_type);
    
  // printf("<block size (1B, 1KB, 32KB)> in bytes:");
   // scanf("%s",block_size);
   // printf("<number of threads (1,2,4,8)>");
   // scanf("%d",&nthread);
   // printf("<server IP address>");
    //scanf("%s",ser_ip);
    if (argc != 5)
    {
        printf("You should input 4 arguments:<mode (TCP UDP LTC_TCP LTC_UDP)>, <block size (1KB, 32KB)>, <number of threads (1,2,4,8)>,<server IP address>\n");
        return(1);
    }
    strcpy(bm_type,argv[1]);
    strcpy(block_size,argv[2]);
    nthread = atoi(argv[3]);
    strcpy(ser_ip,argv[4]);

    printf("server ip is : %s",ser_ip);
    if(strcmp(block_size,"1000")==0||strcmp(block_size,"1KB")==0)  bsize=1024;
    else if(strcmp(block_size,"32KB")==0||strcmp(block_size,"32")==0)bsize=32*1024;
    else printf("Could not identify block size type");
    operate_times=((long int)WORKLOAD/(long int)bsize)*100;
    if(strcmp(bm_type,"LTC_TCP")==0||strcmp(bm_type,"LTC_UDP")==0){
        bsize=1;
        operate_times=1000000;
    }
    current_process=0;
  
    bzero(&ser_add, sizeof(ser_add));
    ser_add.sin_family = AF_INET;
    ser_add.sin_addr.s_addr = inet_addr(ser_ip);
    ser_add.sin_port = htons(4999);
    
    
    if(strcmp(bm_type,"TCP")==0||strcmp(bm_type,"LTC_TCP")==0){
        
        start_t = clock();
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, tcp_client, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
        
    }else if(strcmp(bm_type,"UDP")==0||strcmp(bm_type,"LTC_UDP")==0){
        
        start_t = clock();
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, udp_client, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
            }
        }
    }
    else{
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
    
    if(strcmp(bm_type,"LTC_TCP")==0||strcmp(bm_type,"LTC_UDP")==0){
        float thoughput=total_t/1024;
        float theoreticalValue=100;
        float efficiency=thoughput/theoreticalValue;
        printf("%s  %d  %d  %.1f ms  %.1f ms  %.1f \n", bm_type,nthread, bsize,thoughput,theoreticalValue, efficiency);
        
    }else{
        float thoughput=100*1024/total_t;
        float theoreticalValue=50;
        float efficiency=thoughput/theoreticalValue;
        printf("%s  %d  %d  %.1f MB/sec  %.1f MB/sec  %.1f \n", bm_type,nthread, bsize,thoughput,theoreticalValue, efficiency);
        
    }
    return 0;
    
}





