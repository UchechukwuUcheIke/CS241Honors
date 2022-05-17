#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct player_move {
    char token;
    int row;
    int col;
} player_move;

typedef struct game_state {
    int x_win;
    int o_win;
} game_state;


char *create_message(char *name, char *message);
ssize_t get_message_size(int socket);
ssize_t write_message_size(size_t size, int socket);
ssize_t read_all_from_socket(int socket, void *buffer, size_t count);
ssize_t write_all_to_socket(int socket, const void *buffer, size_t count);