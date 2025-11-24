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
char winner = ' ';
int MountainX = -1, MountainY = -1;

bool isTweetyDead = false, isTazDead = false, isBugsDead = false;

pthread_mutex_t lock;

std::string decypherChar(char w);
bool greedyMartian();

void print_bar()
{
    int barLength = (SIZE * 5) + 2;
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

    if (i == 'F') 
    {
        MountainX = x;
        MountainY = y;
    }
}

bool didIDie (char player) 
{
    if (player == 't' || player == 'T') 
    {
        return isTweetyDead;
    } 
    else if (player == 'd' || player == 'D') 
    {
        return isTazDead;
    } 
    else if (player == 'b' || player == 'B') 
    {
        return isBugsDead;
    }
    else 
    {
        return false;// Marvin is immortal
    }
}

void* player_thread(void* arg) 
{
    char player = *(char*)arg;
    while (true) 
    {
        pthread_mutex_lock(&lock);

        // If a winner was already found, exit the thread.
        if (winner != ' ' || didIDie(player)) {
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
        std::string whoIsIt = decypherChar(player);
        printf("%s moves!\n", whoIsIt.c_str());

        pthread_mutex_unlock(&lock);
        sleep(2);  // Wait for a second to avoid the threads cheating by skipping turns.
    }

    return NULL;
}

void* martian_thread(void* arg) 
{
    sleep(0.1); // Slight delay to let other players start first
    char player = *(char*)arg;
    bool hasAllCarrots = false;
    int roundCount = 1;
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
            
            tryAgain = (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE);  

        } while (tryAgain || board[nx][ny] == 'm' || board[nx][ny] == 'M');

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
            winner = player;
            board[curx][cury] = '-';
            board[nx][ny] = player;
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        else if (board[nx][ny] == 'b' || board[nx][ny] == 'B' || board[nx][ny] == 'd' || board[nx][ny] == 'D' || board[nx][ny] == 't' || board[nx][ny] == 'T') 
        {
            // Martian eliminates the player
            std::string hadCarrot = "";
            if (isupper(board[nx][ny])) 
            {
                player = toupper(player);
                hadCarrot = " and steals their carrot";
            }
            std::string corpse = decypherChar(board[nx][ny]);
            printf("Marvin the Martian eliminates %s%s!\n", corpse.c_str(), hadCarrot.c_str());
            if (board[nx][ny] == 'b' || board[nx][ny] == 'B') 
            {
                printf("'Ehh, what's up doc?'\n");
                isBugsDead = true;
            }
            else if (board[nx][ny] == 'd' || board[nx][ny] == 'D') 
            {
                printf("'You're gonna regret that!'\n");
                isTazDead = true;
            }
            else if (board[nx][ny] == 't' || board[nx][ny] == 'T') 
            {
                printf("'I tawt I taw a puddy tat!'\n");
                isTweetyDead = true;
            }

            board[curx][cury] = '-';
            board[nx][ny] = player;
        }
        

        print_board();
        print_bar();
        std::string whoIsIt = decypherChar(player);
        printf("%s moves!\n", whoIsIt.c_str());

        print_bar();
        printf("~~~Round %d~~~\n", ++roundCount);

        if(roundCount % 3 == 0) 
        {
            printf("Marvin the Martian activates his space x multi-dimensional time travel machine!\n");
            printf("'Brace yourselves for immediate temporal displacement!'\n");

            int x, y;
            do 
            {
                x = rand() % SIZE;
                y = rand() % SIZE;
            } while (board[x][y] != '-');

            board[x][y] = 'F';
            board[MountainX][MountainY] = '-';
            MountainX = x;
            MountainY = y;
        }

        if (!hasAllCarrots) 
        {
            hasAllCarrots = greedyMartian();
        }

        pthread_mutex_unlock(&lock);
        sleep(2);  // Wait for a second to avoid the threads cheating by skipping turns.
    }

    return NULL;
}

bool greedyMartian()
{
    int CarrotCount = 0;
    bool BugsHasCarrot = false;
    bool TazHasCarrot = false;
    bool TweetyHasCarrot = false;

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == 'C')
            {
                CarrotCount++;
            }
            else if (board[i][j] == 'B' )
            {
                BugsHasCarrot = true;
            }
            else if (board[i][j] == 'D' )
            {
                TazHasCarrot = true;
            }
            else if (board[i][j] == 'T' )
            {
                TweetyHasCarrot = true;
            }
        }

    }

    if (!(CarrotCount == 0) || BugsHasCarrot || TazHasCarrot || TweetyHasCarrot)
    {
        return false;
    }

    printf("Marvin has collected all of the carrots!\n");
    printf("'Hmm, yes very curious. Very interesting. I do so enjoy observing the flora of that tiny planet.'\n");

    return true;
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

    printf("Is Marvin the Martian gonna blow it up (cheat)? (y/n) ");
    char isThereKaboom;
    std::cin >> isThereKaboom;

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
    if (isThereKaboom == 'y' || isThereKaboom == 'Y') 
    {
        printf("Marvin the Martian is feeling greedy today!\n");
        print_bar();
        printf("~~~Round 1~~~\n");
    } 
    else
    {
        printf("Marvin the Martian is feeling merciful today!\n");
    }

    pthread_create(&bugs, NULL, player_thread, &Bugs);
    sleep(0.3);
    pthread_create(&taz, NULL, player_thread, &Taz);
    sleep(0.5);
    pthread_create(&tweety, NULL, player_thread, &Tweety);
    sleep(0.7);
    if (isThereKaboom == 'y' || isThereKaboom == 'Y') 
    {
        pthread_create(&marvin, NULL, martian_thread, &Marvin);
    } 
    else
    {
        pthread_create(&marvin, NULL, player_thread, &Marvin);
    }

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