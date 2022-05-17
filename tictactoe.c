#include <stddef.h>
#include <stdlib.h>
#include "tictactoe.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int BOARD_STRING_LEN = 21;

tictactoe* tictactoe_create(char player_one_token, char player_two_token) {
    tictactoe *new_tictactoe = malloc(sizeof(tictactoe));
    new_tictactoe -> player_one_token = player_one_token;
    new_tictactoe -> player_two_token = player_two_token;
    new_tictactoe -> width = 3;
    new_tictactoe -> height = 3;
    new_tictactoe -> board = calloc(sizeof(char*) * new_tictactoe -> height, sizeof(char));

    int i = 0;
    for (; i < new_tictactoe -> height; i++) {
        new_tictactoe -> board[i] = calloc(sizeof(char) * new_tictactoe -> width, sizeof(char));
    }

    return new_tictactoe;
}

void place_tictactoe_token(tictactoe* tictactoe, int row, int col, char token) {
    tictactoe -> board[row][col] = token;
}

char get_tictactoe_token(tictactoe* tictactoe, int row, int col) {
    return tictactoe -> board[row][col];
}

void clear_tictactoe_board(tictactoe* tictactoe) {
    int width = tictactoe -> width;
    int height = tictactoe -> height;

    int row = 0;
    int col = 0;
    for (; row < height; row++) {
        for(; col < width; col++) {
            tictactoe -> board[row][col] = '\0';
        }
    }
    
}

void tictactoe_destroy(tictactoe* tictactoe) {
    int row = 0;
    int height = tictactoe -> height;
    for (; row < height; row++) {
        free(tictactoe -> board[row]);
    }
    free(tictactoe -> board);

    free(tictactoe);
    tictactoe = NULL;
}

int tictactoe_check_for_win(tictactoe* tictactoe, char player_token) {
    int width = tictactoe -> width;
    int height = tictactoe -> height;
    
    int horizontal_win = ((tictactoe -> board[0][0] == player_token) && (tictactoe -> board[0][0] == tictactoe -> board[0][1] && tictactoe -> board[0][1] == tictactoe -> board[0][2])) ||
                        ((tictactoe -> board[1][0] == player_token) && (tictactoe -> board[1][0] == tictactoe -> board[1][1] && tictactoe -> board[1][1] == tictactoe -> board[1][2])) ||
                        ((tictactoe -> board[2][0] == player_token) &&(tictactoe -> board[2][0] == tictactoe -> board[2][1] && tictactoe -> board[2][1] == tictactoe -> board[2][2]));
    
    int vertical_win = ((tictactoe -> board[0][0] == player_token) && (tictactoe -> board[0][0] == tictactoe -> board[1][0] && tictactoe -> board[1][0] == tictactoe -> board[2][0])) ||
                        ((tictactoe -> board[0][1] == player_token) && (tictactoe -> board[0][1] == tictactoe -> board[1][1] && tictactoe -> board[1][1] == tictactoe -> board[2][1]))  ||
                        ((tictactoe -> board[0][2] == player_token) && (tictactoe -> board[0][2] == tictactoe -> board[1][2] && tictactoe -> board[1][2] == tictactoe -> board[2][2]));
    
    int diagonal_win = ((tictactoe -> board[0][0] == player_token) && (tictactoe -> board[0][0] == tictactoe -> board[1][1] && tictactoe -> board[1][1] == tictactoe -> board[2][2])) ||
                        ((tictactoe -> board[2][0] == player_token) && (tictactoe -> board[2][0] == tictactoe -> board[1][1] && tictactoe -> board[1][1] == tictactoe -> board[0][2]));

    return horizontal_win || vertical_win || diagonal_win;
}

char *get_tictactoe_board_string(tictactoe * board) {
    int width = board -> width;
    int height = board -> height;
    char *output = calloc(width * height * 2 + height + 1, sizeof(char));

    int row = 0;
    
    int str_idx = 0;

    printf("Width: %d\n", width);
    printf("Height: %d\n", height);
    for (; row < height; row++) {
        int col = 0;
        for(; col < width; col++) {
            char token = get_tictactoe_token(board, row, col);
            if (token != '\0') {
                output[str_idx++] = token;
                output[str_idx++] = ' ';
            } else {
                output[str_idx++] = ' ';
                output[str_idx++] = ' ';
            }
        }
        output[str_idx++] = '\n';
    }
    return output;
}
void print_tictactoe_board(tictactoe* board) {

}
