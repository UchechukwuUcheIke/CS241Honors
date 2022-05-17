/**
 * charming_chatroom
 * CS 241 - Spring 2022
 */

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
#include <semaphore.h>
#include "common.h"

static volatile int serverSocket;
static pthread_t threads[2];

void *write_to_server(void *arg);
void *read_from_server(void *arg);
void close_program(int signal);

sem_t turn_sem; // Semaphore allows writing thread to pass when it is the player's turn

int game_finished = 0;

/**
 * Shuts down connection with 'serverSocket'.
 * Called by close_program upon SIGINT.
 */
void close_server_connection() {
    // Your code here
    shutdown(serverSocket, SHUT_RD); // use this function
    close(serverSocket);
}

/**
 * Sets up a connection to a chatroom server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or exit(1) on failure.
 */
int connect_to_server(const char *host, const char *port) {
    /*QUESTION 1*/
    /*QUESTION 2*/
    /*QUESTION 3*/

    /*QUESTION 4*/
    /*QUESTION 5*/

    /*QUESTION 6*/

    /*QUESTION 7*/

    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET; /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* TCP */

    s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(result);
        exit(1);
    }

    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("connect");
        freeaddrinfo(result);
        exit(1);
    }

    freeaddrinfo(result);
    return sock_fd;
}

typedef struct _thread_cancel_args {
    char **buffer;
    char **msg;
} thread_cancel_args;

/**
 * Cleanup routine in case the thread gets cancelled.
 * Ensure buffers are freed if they point to valid memory.
 */
void thread_cancellation_handler(void *arg) {
    printf("Cancellation handler\n");
    thread_cancel_args *a = (thread_cancel_args *)arg;
    char **msg = a->msg;
    char **buffer = a->buffer;
    if (*buffer) {
        free(*buffer);
        *buffer = NULL;
    }
    if (msg && *msg) {
        free(*msg);
        *msg = NULL;
    }
}

/**
 * Reads bytes from user and writes them to server.
 *
 * arg - void* casting of char* that is the username of client.
 */
void *write_to_server(void *arg) {
    char *name = (char *)arg;
    char *buffer = NULL;
    char *msg = NULL;
    ssize_t retval = 1;

    thread_cancel_args cancel_args;
    cancel_args.buffer = &buffer;
    cancel_args.msg = &msg;
    // Setup thread cancellation handlers.
    // Read up on pthread_cancel, thread cancellation states,
    // pthread_cleanup_push for more!
    pthread_cleanup_push(thread_cancellation_handler, &cancel_args);

    while (retval > 0 && !game_finished) {

        printf("Waiting for turn\n");
        sem_wait(&turn_sem);
        // Read from standard in
        printf("Getting Standard In\n");
        char buffer[100];
        int row = 0;
        int col = 0;
        char token = '\0';

        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d %d %c", &row, &col, &token);
        //scanf("%d %d %s", &row, &col, &token);

        printf("Row Given: %d\n", row);
        printf("Column Given: %d\n", col);
        player_move move;
        move.row = row;
        move.col = col;
        move.token = token;

        printf("Row Sent: %d\n", move.row);
        printf("Column Sent: %d\n", move.col);

        retval = write_all_to_socket(serverSocket, (void *)&move, sizeof(player_move));

    }

    pthread_cleanup_pop(0);
    return 0;
}

/**
 * Reads bytes from the server and prints them to the user.
 *
 * arg - void* requriment for pthread_create function.
 */
void *read_from_server(void *arg) {
    // Silence the unused parameter warning.
    (void)arg;
    ssize_t retval = 1;
    char *buffer = NULL;
    thread_cancel_args cancellation_args;
    cancellation_args.buffer = &buffer;
    cancellation_args.msg = NULL;
    pthread_cleanup_push(thread_cancellation_handler, &cancellation_args);

    while (retval > 0 && !game_finished) {
        size_t message_size;
        retval = read_all_from_socket(serverSocket, (char*)&message_size, sizeof(size_t));
        if (message_size == sizeof(size_t)) { // Server is saying who's turn it is
            
            size_t my_turn;
            read_all_from_socket(serverSocket, (char*)&my_turn, sizeof(size_t));

            if (my_turn) {
                printf("It's my turn");
                sem_post(&turn_sem);
            }
        }else if (message_size == sizeof(game_state)) { // The game may have ended
            
            game_state state;
            retval = read_all_from_socket(serverSocket, (void*) &state, message_size);
            if (state.x_win == 1 || state.o_win == 1) {
                printf("break!\n");
                game_finished = 1;
            }
        } else {
            char buffer[1024];
            printf("Expecting string of size: %ld\n", message_size);
            retval = read_all_from_socket(serverSocket, buffer, message_size);
            printf("Return val was of size: %ld\n", retval);
            printf("%s\n", buffer);
        }


    }

    pthread_cleanup_pop(0);
    close_server_connection();
    return 0;
}

/**
 * Signal handler used to close this client program.
 */
void close_program(int signal) {
    if (signal == SIGINT) {
        pthread_cancel(threads[0]);
        pthread_cancel(threads[1]);

        close_server_connection();
    }
}

int main(int argc, char **argv) {
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Usage: %s <address> <port> <username> [output_file]\n",
                argv[0]);
        exit(1);
    }

    char *output_filename;
    if (argc == 5) {
        output_filename = argv[4];
    } else {
        output_filename = NULL;
    }

    // Setup signal handler.
    signal(SIGINT, close_program);
    //create_windows(output_filename);
    //atexit(destroy_windows);

    sem_init(&turn_sem, 0, 0);

    serverSocket = connect_to_server(argv[1], argv[2]);

    pthread_create(&threads[0], NULL, write_to_server, (void *)argv[3]);
    pthread_create(&threads[1], NULL, read_from_server, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;
}