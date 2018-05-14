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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define NUM_THREADS 128
#define WORKLOAD 1024*1024*1024


pthread_mutex_t mutex;
char bm_type[10];
long current_process;
long operate_times;
int bsize;
int nthread;
struct sockaddr_in ser_add;
struct sockaddr_in cli_add[8];
int cli_sock[8];
int sockaddr_in_size;
int ser_sockfd;


void *udp_recv(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    memset(buffer, '0', bsize);
    
    while(current_process<operate_times){
        recvfrom(cli_sock[(int) args], buffer, bsize, 0, (struct sockaddr *)&cli_add[(int) args], &sockaddr_in_size);
        if(strcmp(bm_type,"LTC_UDP")==0){
            sendto(cli_sock[(int) args], buffer, bsize, 0, (struct sockaddr *)&cli_add[(int) args], sizeof(struct sockaddr));
        }
        current_process++;
    }
    close(cli_sock[(int) args]);
    free(buffer);
    pthread_exit(NULL);
}

void *tcp_recv(void *args){
    char * buffer = (char *)malloc(bsize * sizeof(char));
    memset(buffer, '0', bsize);

    while(current_process<operate_times){
        recv(cli_sock[(int) args], buffer, bsize, 0);
        if(strcmp(bm_type,"LTC_TCP")==0){
            send(cli_sock[(int) args], buffer, bsize, 0);
        }
        current_process++;
    }
    close(cli_sock[(int) args]);
    free(buffer);
    pthread_exit(NULL);
    
}




int main(int argc, char **argv)
{
    
    clock_t start_t, end_t;
    double total_t;
    char block_size[10];
    pthread_t threads[NUM_THREADS];
    long tid;
    int rc;
    
    //get user input
   // printf("<mode (TCP UDP)>");
    //scanf("%s",bm_type);
    
   // printf("<block size (1B, 1KB, 32KB)> in bytes:");
   // scanf("%s",block_size);
   // printf("<number of threads (1,2,4,8)>");
   // scanf("%d",&nthread);
    
    if (argc != 4)
    {
        printf("You should input 3 arguments:<mode (TCP UDP LTC_TCP LTC_UDP)>, <block size (1KB, 32KB)>, <number of threads (1,2,4,8)>\n");
        return(1);
    }
    strcpy(bm_type,argv[1]);
    strcpy(block_size,argv[2]);
    nthread = atoi(argv[3]);

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
    ser_add.sin_addr.s_addr = INADDR_ANY;
    ser_add.sin_port = htons(4999);
    sockaddr_in_size = sizeof(struct sockaddr_in);

    
    if(strcmp(bm_type,"TCP")==0||strcmp(bm_type,"LTC_TCP")==0){
        ser_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        int rt = bind(ser_sockfd, (struct sockaddr *)&ser_add, sizeof(struct sockaddr));
        if(rt == -1)
        {
            printf("bind sockfd failed");
            exit(-1);
        }else {
            printf("bind sockfd success");

        }
        listen(ser_sockfd, 10);

        start_t = clock();
        
        for (tid = 0; tid < nthread; tid++) {
            
            cli_sock[tid] = accept(ser_sockfd, (struct sockaddr *) &cli_add[tid], &sockaddr_in_size);
            if(cli_sock[tid] == -1)
            {
                printf("accept client failed");
                exit(-1);
            }
            rc = pthread_create(&threads[tid], NULL, tcp_recv, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
                exit(-1);
            }
        }
        
    }else if(strcmp(bm_type,"UDP")==0||strcmp(bm_type,"LTC_UDP")==0){
        
        ser_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        int rt = bind(ser_sockfd, (struct sockaddr *)&ser_add, sizeof(struct sockaddr));
        if(rt == -1)
        {
            printf("bind sockfd failed");
            exit(-1);
        }else {
            printf("bind sockfd success");
            
        }
        sockaddr_in_size = sizeof(struct sockaddr_in);
        
        start_t = clock();
        for (tid = 0; tid < nthread; tid++) {
            rc = pthread_create(&threads[tid], NULL, udp_recv, (void *) tid);
            if (rc) {
                printf("Could not create thread %ld\n", tid);
                exit(-1);
            }
        }
        
    close(ser_sockfd);
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    return 0;
    
    }
}





