#include "tictactoe.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int tictactoe_create_and_destroy();
int tictactoe_place_token_and_check();
int tictactoe_print_board_simple();
int tictactoe_print_board_tokens_placed();
int tictactoe_check_win_condition_no_win();
int tictactoe_check_win_condition_horizontal_win();
int tictactoe_check_win_condition_vertical_win();
int tictactoe_check_win_condition_diagonal_win();
int tictactoe_check_win_condition_wrong_token();
int main() {
    return tictactoe_print_board_simple() && tictactoe_print_board_tokens_placed();
}

int tictactoe_create_and_destroy() {
    printf("\nTest TicTacToe Create and Destroy\n");

    tictactoe *board = tictactoe_create('X', 'O');

    tictactoe_destroy(board);

    printf("Test Passed!\n");
    return 1;
}

int tictactoe_place_token_and_check() {
    printf("\nTest TicTacToe place token and check\n");

    tictactoe *board = tictactoe_create('X', 'O');
    place_tictactoe_token(board, 0, 0, 'X');
    assert(get_tictactoe_token(board, 0, 0) == 'X');
    tictactoe_destroy(board);
    printf("Test Passed!\n");
    return 1;
}

int tictactoe_print_board_simple() {
    printf("\nTest TicTacToe Print Board Simple\n");
    tictactoe *board = tictactoe_create('X', 'O');

    char* board_string = get_tictactoe_board_string(board);

    char* cross_reference = "      \n      \n      \n";

    int i = 0;
    for (; i < strlen(cross_reference); i++) {
        assert(board_string[i] == cross_reference[i]);
    }

    free(board_string);
    return 1;
}

int tictactoe_print_board_tokens_placed() {
    printf("\nTest TicTacToe Print Board Complex\n");
    tictactoe *board = tictactoe_create('X', 'O');

    
    place_tictactoe_token(board, 0, 0, 'X');
    place_tictactoe_token(board, 2, 0, 'X');
    place_tictactoe_token(board, 2, 2, 'X');
    place_tictactoe_token(board, 0, 2, 'O');
    place_tictactoe_token(board, 1, 2, 'O');
    char* board_string = get_tictactoe_board_string(board);
    
    char* cross_reference = "X   O \n    O \nX   X \n";

    int i = 0;
    for (; i < strlen(cross_reference); i++) {
        assert(board_string[i] == cross_reference[i]);
    }

    printf("%s\n", board_string);
    printf("Strlen(board_string): %ld\n", strlen(board_string));

    free(board_string);
    tictactoe_destroy(board);
    return 1;
}

int tictactoe_check_win_condition_no_win() {
    printf("\ntictactoe_check_win_condition_no_win()\n");
    tictactoe *board = tictactoe_create('X', 'O');

    
    place_tictactoe_token(board, 0, 0, 'X');
    place_tictactoe_token(board, 0, 1, 'X');
    place_tictactoe_token(board, 2, 2, 'X');
    place_tictactoe_token(board, 0, 2, 'O');

    assert(tictactoe_check_for_win(board, 'X') == 0);
    tictactoe_destroy(board);

    return 1;
}

int tictactoe_check_win_condition_horizontal_win() {
    printf("\nTest TicTacToe Horizontal Win\n");
    tictactoe *board = tictactoe_create('X', 'O');

    
    place_tictactoe_token(board, 0, 0, 'X');
    place_tictactoe_token(board, 0, 1, 'X');
    place_tictactoe_token(board, 2, 2, 'O');
    place_tictactoe_token(board, 0, 2, 'X');

    assert(tictactoe_check_for_win(board, 'X') == 1);
    tictactoe_destroy(board);

    return 1;
}


int tictactoe_check_win_condition_vertical_win() {
    printf("\nTest TicTacToe Vertical Win\n");
    tictactoe *board = tictactoe_create('X', 'O');

    
    place_tictactoe_token(board, 0, 0, 'X');
    place_tictactoe_token(board, 1, 0, 'X');
    place_tictactoe_token(board, 2, 0, 'X');
    place_tictactoe_token(board, 0, 2, 'O');

    assert(tictactoe_check_for_win(board, 'X') == 1);
    tictactoe_destroy(board);

    return 1;
}

int tictactoe_check_win_condition_diagonal_win() {
    printf("\nTest TicTacToe Diagonal Win\n");
    tictactoe *board = tictactoe_create('X', 'O');

    
    place_tictactoe_token(board, 0, 0, 'X');
    place_tictactoe_token(board, 1, 1, 'X');
    place_tictactoe_token(board, 2, 2, 'X');
    place_tictactoe_token(board, 0, 2, 'O');

    assert(tictactoe_check_for_win(board, 'X') == 1);
    tictactoe_destroy(board);

    return 1;
}

int tictactoe_check_win_condition_wrong_token() {
    printf("\nTest TicTacToe Diagonal Win\n");
    tictactoe *board = tictactoe_create('X', 'O');

    
    place_tictactoe_token(board, 0, 0, 'X');
    place_tictactoe_token(board, 1, 1, 'X');
    place_tictactoe_token(board, 2, 2, 'X');
    place_tictactoe_token(board, 0, 2, 'O');

    assert(tictactoe_check_for_win(board, 'O') == 0);
    tictactoe_destroy(board);

    return 1;
}
