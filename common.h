#if !defined(_COMMON_H_)
#define _COMMON_H_

#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <time.h> 
#include <ncurses.h>
#include <pthread.h>


struct data_t 
{
    sem_t cs;
    int id;
    char map[5][5];
    char direction;
    int game_round;
    unsigned int x;
    unsigned int y;
    unsigned int camp_x;
    unsigned int camp_y;
    unsigned int coins;
    unsigned int brought;
    unsigned int deaths;
    
};

struct connection_t
{
    sem_t cs;
    int id;
    int number;
    char type[6];
    int sid;
};

static void err(int c, const char* msg) 
{
    if (!c)
        return;
    perror(msg);
    exit(1);
}   

#endif