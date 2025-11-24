#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "Player.h"

#include <iostream>
#include <string>

//using namespace std;

#define SIZE 5  //Board size is 5x5 : 3 is the minimum size to fit all characters.

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

void character_init(std::string s, char i)
{
    int x, y;
    do 
    {
        x = rand() % SIZE;
        y = rand() % SIZE;
    } while (board[x][y] != '-');

    // Create a Player object for this character and place it on the board.
    // Use a simple generated name based on the icon.
    std::string name = s;
    ConquestForPlanetX::Player p(name, i, ConquestForPlanetX::Player::Position{x, y});

    board[x][y] = i;
}

void* player_thread(void* arg) 
{
    char player = *(char*)arg;

    while (!game_over) 
    {
        pthread_mutex_lock(&lock);

        int x, y;

        // Choose a move
        do 
        {
            x = rand() % SIZE;
            y = rand() % SIZE;
        } while (board[x][y] != '-');

        // Make the move
        board[x][y] = player;

        print_board();

        // Check for win or draw
        // (Win/draw logic to be implemented)

        pthread_mutex_unlock(&lock);
        sleep(1);  // Wait for a second to avoid the threads cheating by skipping turns.
    }

    return NULL;
}

int main() 
{
    srand(time(NULL));

    if (SIZE < 3) {
        printf ( "Board size must be at least 3 to fit all characters. \n");
        return 1;
    }

	pthread_t bugs, taz, tweety, marvin;
	char Bugs = 'b', Taz = 'd', Tweety = 't', Marvin = 'm';  // Player symbols
    char Mountain = 'F', Carrot1 = 'C', Carrot2 = 'C';  // Objective symbols

	pthread_mutex_init(&lock, NULL);


    board_init();
    character_init("Bugs Bunny",Bugs);
    character_init("Tazmanian Devil", Taz);
    character_init("Tweety Bird",Tweety);
    character_init("Marvin the Martion",Marvin);
    character_init("Mountain", Mountain);
    character_init("Golden Carrot", Carrot1);
    character_init("Golden Carrot", Carrot2);

    //Game start
    pthread_create(&bugs, NULL, player_thread, &Bugs);
    pthread_create(&taz, NULL, player_thread, &Taz);
    pthread_create(&tweety, NULL, player_thread, &tweety);
    pthread_create(&marvin, NULL, player_thread, &marvin);

    pthread_join(bugs, NULL);
    pthread_join(taz, NULL);
    pthread_join(tweety, NULL);
    pthread_join(marvin, NULL);

    pthread_mutex_destroy(&lock);
    
    return 0;
}