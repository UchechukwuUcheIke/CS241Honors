/**
 * charming_chatroom
 * CS 241 - Spring 2022
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "common.h"
#include "tictactoe.h"

#define MAX_CLIENTS 2

void *process_client(void *p);

static volatile int serverSocket;
static volatile int endSession;

static volatile int clientsCount;
static volatile int clients[MAX_CLIENTS];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
tictactoe* board;

static pthread_barrier_t player_barrier; // Blocks until Max clients players are ready to be processed
static pthread_barrier_t waiting_barrier; // Blocks until both players have either made a move or waited in a "game cycle"
int current_plr = -1;

int game_finished = 0;

/**
 * Signal handler for SIGINT.
 * Used to set flag to end server.
 */
void close_server() {
    endSession = 1;
    // add any additional flags here you want.
}

/**
 * Cleanup function called in main after `run_server` exits.
 * Server ending clean up (such as shutting down clients) should be handled
 * here.
 */
void cleanup() {
    if (shutdown(serverSocket, SHUT_RDWR) != 0) {
        perror("shutdown():");
    }
    close(serverSocket);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            if (shutdown(clients[i], SHUT_RDWR) != 0) {
                perror("shutdown(): ");
            }
            close(clients[i]);
        }
    }
}

/**
 * Sets up a server connection.
 * Does not accept more than MAX_CLIENTS connections.  If more than MAX_CLIENTS
 * clients attempts to connects, simply shuts down
 * the new client and continues accepting.
 * Per client, a thread should be created and 'process_client' should handle
 * that client.
 * Makes use of 'endSession', 'clientsCount', 'client', and 'mutex'.
 *
 * port - port server will run on.
 *
 * If any networking call fails, the appropriate error is printed and the
 * function calls exit(1):
 *    - fprtinf to stderr for getaddrinfo
 *    - perror() for any other call
 */
void run_server(char *port) {
    /*QUESTION 1*/
    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt_val1 = 1;
    int opt_val2 = 1;

    if (sock_fd < 0) {
        perror("socket() failed\n");
        exit(1);
    }

    int set_sock1 = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val1, sizeof(opt_val1));
    int set_sock2 = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt_val2, sizeof(opt_val2));
    
    if (set_sock1 != 0 || set_sock2 != 0) {
        perror("setsockopt() failed\n");
        exit(1);
    }

    /*QUESTION 2*/
    /*QUESTION 3*/
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(result);
        exit(1);
    }

    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        freeaddrinfo(result);
        exit(1);
    }

    if (listen(sock_fd, MAX_CLIENTS + 1) != 0) {
        perror("listen()");
        freeaddrinfo(result);
        exit(1);
    }
    

    freeaddrinfo(result);
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    while(endSession == 0 && !game_finished) {
        struct sockaddr_storage client_address;
        socklen_t client_addr_size = sizeof(client_address);
        memset(&client_address, 0, sizeof(struct sockaddr_storage));
        int client = accept(sock_fd, (struct sockaddr *)&client_address, &client_addr_size);
       
        if (current_plr == -1) {
            current_plr = 0;
        }
        if (client == -1) {
            perror("accept() failed\n");
            exit(1);
        }

        pthread_mutex_lock(&mutex);
        clientsCount++;

        intptr_t client_id = -1;
        if (clientsCount > MAX_CLIENTS) {
            clientsCount--;
            close(client);
            pthread_mutex_unlock(&mutex);
            continue;
        } else {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == -1) {
                    clients[i] = client;
                    client_id = i;
                    break;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        pthread_t id;
        pthread_create(&id, NULL, process_client, (void *)client_id);
    }
    shutdown(sock_fd, SHUT_RD);
    close(sock_fd);
    close_server();
}

/**
 * Broadcasts the message to all connected clients.
 *
 * message  - the message to send to all clients.
 * size     - length in bytes of message to send.
 */
void write_to_clients() {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            char *board_string = get_tictactoe_board_string(board);
            ssize_t retval = strlen(board_string) + 1;
            printf("Retval: %ld\n", retval);
            if (retval > 0) {
                size_t message_size = strlen(board_string);
                retval = write_all_to_socket(clients[i], (char*)&message_size, sizeof(size_t));
                retval = write_all_to_socket(clients[i], board_string, message_size);
            }
            if (retval == -1) {
                perror("write(): ");
            }
            free(board_string);
        }
    }
    
    pthread_mutex_unlock(&mutex);
}

/**
 * Handles the reading to and writing from clients.
 *
 * p  - (void*)intptr_t index where clients[(intptr_t)p] is the file descriptor
 * for this client.
 *
 * Return value not used.
 */
void *process_client(void *p) {
    pthread_detach(pthread_self());
    intptr_t clientId = (intptr_t)p;
    ssize_t retval = 1;
    char *buffer = NULL;

    current_plr = clientId;
    pthread_barrier_wait(&player_barrier);
    printf("Players Accepted\n");

    while (retval > 0 && endSession == 0 && game_finished == 0) {
        /**
        retval = get_message_size(clients[clientId]);
        if (retval > 0) {
            buffer = calloc(1, retval);
            retval = read_all_from_socket(clients[clientId], buffer, retval);
        }
        if (retval > 0)
            write_to_clients(buffer, retval);

        free(buffer);
        buffer = NULL;
        **/

        if (retval > 0 && current_plr == clientId) { // && current_plr == clientId
            player_move move;
            printf("Player: %d's turn!\n", current_plr);
            size_t can_play = 1;
            size_t message_size = sizeof(size_t);
            retval = write_all_to_socket(clients[clientId], (char*)&message_size, sizeof(size_t));
            retval = write_all_to_socket(clients[clientId], (void *)&can_play, sizeof(size_t));

            retval = read_all_from_socket(clients[clientId], (void *)&move, sizeof(player_move));
            printf("Received info\n");
            
            char token = move.token;
            int row = move.row;
            int col = move.col;
            printf("Row Received: %d\n", row);
            printf("Column Received: %d\n", col);
            place_tictactoe_token(board, row, col, token);

            if (current_plr == 0) {
                current_plr = 1;
            } else {
                current_plr = 0;
            }

        } else {
            size_t can_play = 0;
            size_t message_size = sizeof(size_t);
            retval = write_all_to_socket(clients[clientId], (char*)&message_size, sizeof(size_t));
            retval = write_all_to_socket(clients[clientId], (void *)&can_play, sizeof(size_t));
        }

        printf("At barrier\n");
        pthread_barrier_wait(&waiting_barrier);
        printf("Past Barrier\n");
        if (retval > 0) {
            write_to_clients();
        } else {
            printf("Oops\n");
        }

        if (tictactoe_check_for_win(board, 'X') == 1) {
            printf("X Wins\n");
            game_state state;
            state.x_win = 1;
            state.o_win = 0;
            size_t message_size = sizeof(game_state);
            retval = write_all_to_socket(clients[clientId], (char*)&message_size, sizeof(size_t));
            retval = write_all_to_socket(clients[clientId], (void*)&state, message_size);
            
            game_finished = 1;
        } else if(tictactoe_check_for_win(board, 'O') == 1) {
            printf("O Wins\n");
            game_state state;
            state.x_win = 0;
            state.o_win = 1;
            size_t message_size = sizeof(game_state);
            retval = write_all_to_socket(clients[clientId], (char*)&message_size, sizeof(size_t));
            retval = write_all_to_socket(clients[clientId], (void*)&state, message_size);

            game_finished = 1;
        }
    }

    printf("User %d left\n", (int)clientId);
    close(clients[clientId]);

    pthread_mutex_lock(&mutex);
    clients[clientId] = -1;
    clientsCount--;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "%s <port>\n", argv[0]);
        return -1;
    }

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    pthread_barrier_init(&player_barrier,
        NULL, MAX_CLIENTS);
    pthread_barrier_init(&waiting_barrier,
        NULL, MAX_CLIENTS);
    

    board = tictactoe_create('X', 'O');
    run_server(argv[1]);
    cleanup();
    pthread_exit(NULL);
}


/**
 *         player_move move;
        printf("Here also\n");
        read_all_from_socket(client_fd, &move, sizeof(player_move));
        printf("Received info\n");
        char token = move.token;
        int row = move.row;
        int col = move.col;
        place_tictactoe_token(board, row, col, token);
        print_tictactoe_board(board);
    **/