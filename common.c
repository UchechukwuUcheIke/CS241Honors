/**
 * charming_chatroom
 * CS 241 - Spring 2022
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "common.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

ssize_t read_all_from_socket(int socket, void *buffer, size_t count) { // wrapper function for read
    // Your Code Here
    ssize_t total_bytes_read = 0;
    while (total_bytes_read < (ssize_t) count)
    {  
        ssize_t bytes_read = read(socket, buffer + total_bytes_read, count - total_bytes_read);
        if (bytes_read == 0){
            return total_bytes_read;

        } else if (bytes_read == -1 && errno != EINTR) {
            return -1;
        } else if (bytes_read == -1 && errno == EINTR){
            errno = 0;
            continue;
        }
        total_bytes_read += bytes_read;
    }
    return total_bytes_read;
}

ssize_t write_all_to_socket(int socket, const void *buffer, size_t count) { // wrapper function for write
    // Your Code Here

    // return the min between the count or the length of the buffer
    // if the min is more than write will take, then we call write multiple times
    ssize_t total_bytes_written = 0;
    while (total_bytes_written < (ssize_t)count)
    {
        ssize_t bytes_written = write(socket, buffer + total_bytes_written, count - total_bytes_written);
        if (bytes_written == 0){
            return total_bytes_written;
        } else if (bytes_written == -1 && errno != EINTR) {
            return -1;
        } else if (bytes_written == -1 && errno == EINTR){
            errno = 0;
            continue;
            
        } 
        total_bytes_written += bytes_written;
    }
    return total_bytes_written;
}


char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t message_size;
    ssize_t  bytes_read = read_all_from_socket(socket, (char *)&message_size, MESSAGE_SIZE_DIGITS);
    if (bytes_read == -1 || bytes_read == 0) {
        return bytes_read;
    }
    return (ssize_t)ntohl(message_size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here

    ssize_t message_size = htonl(size);
    ssize_t bytes = write_all_to_socket(socket, (char *)&message_size, MESSAGE_SIZE_DIGITS);
    return bytes;
}

