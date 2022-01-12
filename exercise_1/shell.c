#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


#define MAX_LINE 80
#define MAX_ARGS MAX_LINE / 2 + 1

void get_next_argument(char *input, char *args[], char argc) {
    char *buffer;
    buffer = strtok(input, " ");
   
    if (buffer == NULL) {
        args[argc] = NULL;
    } else {
        args[argc] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
        strcpy(args[argc], buffer);
    }
}

void reset_arr(char *arr[]) {
    for(int i = 0; arr[i] != NULL && i < MAX_ARGS; i++) {
        free(arr[i]);
        arr[i] = NULL;
    }
}

void split_user_input(char *input, char *args[]) {
    char *buffer;
    int argc = 0;

    // reset args array, frees all used memory locations
    reset_arr(args);

    // fills args withs args from user
    get_next_argument(input, args, argc);

    while (args[argc] != NULL && argc < MAX_ARGS) {
        get_next_argument(NULL, args, ++argc);
    }

    // removing the '\n' at the last argument
    if (args[--argc] != NULL) {
        args[argc][strlen(args[argc]) - 1] = '\0';
    }
}

void read_user_input(char *args[]) {
    char input[MAX_LINE + 1];

    fgets(input, MAX_LINE, stdin);
    fflush(stdin);

    split_user_input(input, args);
}

int ends_with_ampersand(char *args[]) {
    int i;
    for (i = 0; args[i] != NULL && i < MAX_ARGS; i++);

    if (strcmp(args[--i], "&") == 0) {
        return i;
    }

    return 0;
}

bool is_history_command(char *args[]) {
    return strcmp(args[0], "!") == 0 && strcmp(args[1], "!") == 0 && args[2] == NULL;
}

void print_arr(char *arr[]) {
    printf("osh> ");

    for(int i = 0; arr[i] != NULL && i < MAX_ARGS; i++) {
        printf("%s ", arr[i]);
    }

    printf("\n");
}

void copy_arr(char *dest[], char *src[]) {
    for(int i = 0; src[i] != NULL && i < MAX_ARGS; i++) {
        dest[i] = (char *) malloc((strlen(src[i]) + 1) * sizeof(char));
        strcpy(dest[i], src[i]);
    }
}

void prepare_history_command(char *args[], char *history[]) {
    // prints previous command
    print_arr(history);

    // frees "!" in args
    free(args[0]);
    free(args[1]);
    args[0] = NULL;
    args[1] = NULL;
            
    // copies command in history to args
    copy_arr(args, history);
}

int redirection(char *args[], char *redirect) {
    int i;
    for(i = 0; args[i] != NULL && i < MAX_ARGS; i++) {
        if(strcmp(args[i], redirect) == 0) {
            return i;
        }
    }

    return 0;
}

void shift_args_two_places_left(char *args[], int pos) {
    for(pos; args[pos] != NULL && pos < MAX_ARGS; pos++) {
        args[pos - 2] = (char *) malloc((strlen(args[pos]) + 1) * sizeof(char));
        strcpy(args[pos - 2], args[pos]);
        free(args[pos]);
        args[pos] = NULL;
    }
}

void prepare_redirect(char *args[], int redirect_location, int dup, int flag) {
    int fileDescriptor = open(args[redirect_location + 1], flag, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    dup2(fileDescriptor, dup);
    close(fileDescriptor);
    free(args[redirect_location]);
    free(args[redirect_location + 1]);
    args[redirect_location] = NULL;
    args[redirect_location + 1] = NULL;
    
    shift_args_two_places_left(args, redirect_location + 2);
}

void execute_command_in_child_process(char *args[], char *history[]) {
    if(is_history_command(args)) {
        if(history[0] != NULL) prepare_history_command(args, history);
        else {
            printf("osh> No commands in history\n");
            return;
        }
    }

    int ampersand_location = ends_with_ampersand(args);
    if(ampersand_location) {
        free(args[ampersand_location]);
        args[ampersand_location] = NULL;
    }

    pid_t child_state = fork();

    if (child_state == 0) {
        // child process

        // Checks if ther is output redirection
        int redirect_location = redirection(args, ">");
        if(redirect_location) {
            prepare_redirect(args, redirect_location, STDOUT_FILENO, O_CREAT | O_WRONLY);
        }

        // Checks if there is input redirection
        redirect_location = redirection(args, "<");
        if(redirect_location) {
            if(access(args[redirect_location + 1], F_OK) == 0) {
                // file exists
                prepare_redirect(args, redirect_location, STDIN_FILENO, O_RDONLY);
            } else {
                // file doesn't exist
                printf("osh> Error: The file \"%s\" is not found.\n", args[redirect_location + 1]);
                return;
            }
        }

        execvp(args[0], args);
    } else if (child_state == -1) {
        printf("osh> Child process couldn't be created\n");
    } else {
       // parent process
        if (!ampersand_location) {
            wait(NULL);
        }

        reset_arr(history);
        copy_arr(history, args);

        if(ampersand_location) {
            history[ampersand_location] = (char *) malloc(2 * sizeof(char));
            strcpy(history[ampersand_location], "&");
        }
    }
}

int main(void) {
    char *args[MAX_ARGS] = {NULL};
    char *history[MAX_ARGS] = {NULL};
    int should_run = 1;

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        read_user_input(args);

        if(strcmp(args[0], "exit") == 0) {
            should_run = 0;
            break;
        } 
        
        execute_command_in_child_process(args, history);
    }

    return 0;
}