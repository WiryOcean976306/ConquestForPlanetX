#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define SIZE 3  //Board size is 3x3

char board[SIZE][SIZE];

pthread_mutex_t lock;

bool game_over = false;

/*                                          // Print command for the old board
void print_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c", board[i][j]);
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) printf("-+-+-\n");
    }
}
*/


void print_board() {

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf(" %c ", board[i][j]);
			if (j < SIZE - 1) printf("|");
		}
		printf("\n");
		if (i < SIZE - 1) printf("---+---+---\n");
	}
	printf("\n");
}


bool check_winner(char player) {

	// Check rows and columns
	for (int i = 0; i < SIZE; i++) {
		if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
		        (board[0][i] == player && board[1][i] == player && board[2][i] == player))
			return true;
	}

	// Check diagonals
	if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
	        (board[0][2] == player && board[1][1] == player && board[2][0] == player))
		return true;

	return false;
}


bool board_full() {

	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (board[i][j] == ' ')
				return false;
	return true;
}

void* player_thread(void* arg) {

	char player = *(char*)arg;

	while (!game_over) {

		pthread_mutex_lock(&lock);

		int x, y;

		// Choose a move
		do {
			x = rand() % SIZE;
			y = rand() % SIZE;
		} while (board[x][y] != ' ');

		// Make the move
		board[x][y] = player;

		print_board();

		// Check for win or draw
		if (check_winner(player)) {
			printf("Player %c wins!", player);
			game_over = true;
		} else if (board_full()) {
			printf("It's a draw!");
			game_over = true;
		}

		pthread_mutex_unlock(&lock);
		sleep(1);  // Wait for a second to avoid the threads cheating by skipping turns.
	}

	return NULL;
}

int main() {
	srand(time(NULL));

	pthread_t x_thread, o_thread;
	char x = 'X', o = 'O';  // Player symbols

	pthread_mutex_init(&lock, NULL);

	// Initialize board with empty spaces
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			board[i][j] = ' ';
	print_board();

	pthread_create(&x_thread, NULL, player_thread, &x);
	pthread_create(&o_thread, NULL, player_thread, &o);

	pthread_join(x_thread, NULL);
	pthread_join(o_thread, NULL);

	pthread_mutex_destroy(&lock);

	return 0;
}