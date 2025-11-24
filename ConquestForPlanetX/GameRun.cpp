#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <cctype>
#include <string>

#define SIZE 5  //Board size is 5x5 : 3 is the minimum size to fit all characters.

char board[SIZE][SIZE];
int barLength = (SIZE * 5) + 2;
char winner = ' ';

pthread_mutex_t lock;

void print_bar()
{
    std::string repeated(barLength, '-');
    printf("%s\n", repeated.c_str());
}

void print_board() 
{
    print_bar();

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

void character_init(char i)
{
    int x, y;
    do 
    {
        x = rand() % SIZE;
        y = rand() % SIZE;
    } while (board[x][y] != '-');

    board[x][y] = i;
}

void* player_thread(void* arg) 
{
    char player = *(char*)arg;
    while (true) 
    {
        pthread_mutex_lock(&lock);

        // If a winner was already found, exit the thread.
        if (winner != ' ') {
            pthread_mutex_unlock(&lock);
            break;
        }

        int curx = -1, cury = -1;
        // Find the player's current position on the board
        for (int i = 0; i < SIZE && curx == -1; ++i) 
        {
            for (int j = 0; j < SIZE; ++j) 
            {
                if (board[i][j] == player) 
                {
                    curx = i;
                    cury = j;
                    break;
                }
            }
        }

        if (curx == -1) 
        {
            // Player not found on board; nothing to do this turn
            pthread_mutex_unlock(&lock);
            sleep(1);
            continue;
        }

        bool tryAgain = false;
        int nx, ny;
        int attempts = 0;
        // Choose a neighbouring move (including staying still) but prefer empty cells
        do 
        {
            nx = curx + (rand() % 3) - 1;
            ny = cury + (rand() % 3) - 1;
            
            // safety: if too many attempts, skip turn
            ++attempts;
            if (attempts > 50) 
            {
                break;
            }

            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) 
            {
                tryAgain = true;
            }

        } while (tryAgain || (board[nx][ny] != '-' && board[nx][ny] != 'C' && (board[nx][ny] != 'F' && isupper(player))));

        if (board[nx][ny] == '-') {
            board[curx][cury] = '-';
            board[nx][ny] = player;
        }
        else if (board[nx][ny] == 'C' && !(isupper(player))) 
        {
            player = toupper(player);
            board[curx][cury] = '-';
            board[nx][ny] = player;
        }
        else if (board[nx][ny] == 'F' && isupper(player)) 
        {
            // Declare winner and exit
            winner = player;
            board[curx][cury] = '-';
            board[nx][ny] = player;
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        

        print_board();
        print_bar();
        printf("Player %c moves!\n", player);

        pthread_mutex_unlock(&lock);
        sleep(1);  // Wait for a second to avoid the threads cheating by skipping turns.
    }

    return NULL;
}

std::string decypherChar(char w) {
    switch (w) {
        case 'b':
        case 'B': return "Bugs Bunny";
        case 'd':
        case 'D': return "Taz";
        case 't':
        case 'T': return "Tweety";
        case 'm':
        case 'M': return "Marvin the Martian";
        default: return "Unknown";
    }
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
    character_init(Bugs);
    character_init(Taz);
    character_init(Tweety);
    character_init(Marvin);
    character_init(Mountain);
    character_init(Carrot1);
    character_init(Carrot2);

    //Game start
    pthread_create(&bugs, NULL, player_thread, &Bugs);
    pthread_create(&taz, NULL, player_thread, &Taz);
    pthread_create(&tweety, NULL, player_thread, &Tweety);
    pthread_create(&marvin, NULL, player_thread, &Marvin);

    pthread_join(bugs, NULL);
    pthread_join(taz, NULL);
    pthread_join(tweety, NULL);
    pthread_join(marvin, NULL);

    pthread_mutex_destroy(&lock);


    print_board();
    print_bar();

    std::string winnerName = decypherChar(winner);
    printf("Congratulations to %s for winning the game!\n", winnerName.c_str());

    print_bar();
    
    return 0;
}