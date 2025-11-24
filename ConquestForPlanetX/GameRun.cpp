#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>

//using namespace std;

#define SIZE 5  //Board size is 5x5

char board[SIZE][SIZE];
int barLength = (SIZE * 5) + 2;
bool game_over = false;

pthread_mutex_t lock;

void print_board() 
{
    std::string repeated(barLength, '-');
    printf("%s\n", repeated.c_str());

    for (int i = 0; i < SIZE; i++)
    {
        printf("|");
        for (int j = 0; j < SIZE; j++)
        {
            printf("  %c  ", board[i][j]);
        }
        printf("|\n");
    }
}

void board_init()
{
    for (int i = 0; i < SIZE; ++i) 
    {
        for (int j = 0; j < SIZE; ++j) {
            board[i][j] = '-';
        }
    }
}

int main() 
{
    srand(time(NULL));

	pthread_t Bugs, Taz, Tweety, Marvin;
	char Bugs = 'b', Taz = 'd', Tweety = 't', Marvin = 'm';  // Player symbols
    char Carrot1 = 'C', Carrot2 = 'C';

	pthread_mutex_init(&lock, NULL);


    board_init();

    //


    print_board();
    print_board();

    return 0;
}