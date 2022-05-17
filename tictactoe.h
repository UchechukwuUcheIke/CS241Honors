#include <stddef.h>

typedef struct tictactoe {
    int width;
    int height;
    char **board;
    char player_one_token;
    char player_two_token;
} tictactoe;

tictactoe* tictactoe_create(char player_one_token, char player_two_token);
void place_tictactoe_token(tictactoe* tictactoe, int row, int col, char token);
char get_tictactoe_token(tictactoe* tictactoe, int row, int col);
void clear_tictactoe_board(tictactoe* tictactoe);

void tictactoe_destroy(tictactoe* tictactoe);

int tictactoe_check_for_win(tictactoe* tictactoe, char player_token);
char *get_tictactoe_board_string(tictactoe* tictactoe);
void print_tictactoe_board(tictactoe* board);
