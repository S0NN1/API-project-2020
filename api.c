#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct {
    char **strings;
    size_t length;
} current_state;

typedef struct cmds {
    char *command;
    char **modified_strings;
    int length;
    struct cmds *next;
    struct cmds *prev;
} commands;

//get input function
char *get_input(size_t *input_length);

//get addresses function
void get_addresses(int *addr1, int *addr2, const char *cmd, size_t cmd_length);

//print command
void print(current_state *state, int addr1, int addr2);

//change command
void
change(current_state **state, int addr1, int addr2, char *cmd, commands **redo, commands **undo);

//delete command
void delete(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length);

//undo command
void undo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length);

//redo command
void redo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length);

//push into redo/undo struct
void push(commands **state, char *string, int index);

//push
int main() {
    //struct storing current strings and relative length
    current_state *state = NULL;

    //struct storing redo and relative strings
    commands *redo_state = NULL;

    //struct storing undo and relative strings
    commands *undo_state = NULL;
    //command range
    int addr1 = 0, addr2 = 0;

    //command identifier 'p', 'c', 'd', 'r', 'u', 'q'
    char c;

    //command string
    char *cmd;

    //array length
    size_t cmd_length = 0;

    while (true) {

        //get command from stdin
        cmd = get_input(&cmd_length);

        //get identifier
        c = cmd[cmd_length - 2];

        //get addresses
        get_addresses(&addr1, &addr2, cmd, cmd_length);

        //identify command
        switch (c) {
            case ('p'):
                print(state, addr1, addr2);
                break;
            case ('c'):
                change(&state, addr1, addr2, cmd, &redo_state, &undo_state);
                break;
            case ('d'):
                delete(state, addr1, addr2, cmd, cmd_length);
                break;
            case ('u'):
                undo(state, addr1, addr2, cmd, cmd_length);
                break;
            case ('r'):
                redo(state, addr1, addr2, cmd, cmd_length);
                break;
            case ('q'):
                return 0;
            case ('.'):
                break;
            default:
                printf("ARE YOU WINNING SON???");
                break;
        }
    }
}

char *get_input(size_t *input_length) {
    //char for input
    char c;

    //input string
    char *input = (char *) malloc(1025 * sizeof(char));//TODO EFFICIENCY

    //index of the string
    int i = 0;

    //string length
    size_t length = 0;

    //scan stdin input until it reaches '\n' (included)
    do {
        c = getchar_unlocked();
        input[i] = c;
        length++;
        i++;
    } while (c != '\n');
    (*input_length) = length;
    return input;
}


void get_addresses(int *addr1, int *addr2, const char *cmd, size_t cmd_length) {
    //index of ',' char
    int end = 0;

    //temp string
    char *temp = (char *) malloc(10 * sizeof(char));

    //reach ','
    for (int i = 0; i < cmd_length; ++i) {
        if (cmd[i] == ',') {
            end = i;
            break;
        }
        temp[i] = cmd[i];
    }
    //assign first address
    (*addr1) = atoi(temp);

    //reach '\n'
    int j = 0;
    for (int i = end + 1; i < cmd_length; ++i) {
        if (cmd[i] > 96 && cmd[i] < 123) {
            end = i;
            break;
        }
        temp[j] = cmd[i];
        j++;
    }
    (*addr2) = atoi(temp);
    free(temp);
}

/*
 * print command: if state is uninitialized prints only ".\n", if address is included in current state it prints
 * relative string, otherwise it prints ".\n".
 */
void print(current_state *state, int addr1, int addr2) {
    current_state *temp = state;
    if (state == NULL) {
        for (int i = addr1; i <= addr2; ++i) {
            printf(".\n");
        }
    } else {
        for (int i = addr1; i <= addr2; ++i) {
            if (i == 0 || i > temp->length) {
                printf(".\n");
            } else {
                printf("%s", temp->strings[i - 1]);
            }
        }
    }
}

/*
 * change command: if state is uninitialized it creates the struct and start filling in, if index i exceeds the length
 * it resizes the struct, otherwise changes the previous string.
 */
void
change(current_state **state, int addr1, int addr2, char *cmd, commands **redo, commands **undo) {
    //undo/redo temp nodes
    commands *temp_redo = (commands *) malloc(sizeof(commands));
    commands *temp_undo = (commands *) malloc(sizeof(commands));

    //set them first
    temp_redo->prev = NULL;
    temp_undo->prev = NULL;

    //set their command
    temp_redo->command = cmd;
    temp_undo->command = cmd;

    //add them at the top of the list
    temp_redo->next = (*redo);
    temp_undo->next = (*undo);

    //setting strings to NULL
    temp_redo->modified_strings = NULL;
    temp_undo->modified_strings = NULL;

    //setting length
    temp_redo->length = 0;
    temp_undo->length = 0;

    //string length (not used in this context)
    size_t c = 0;
    for (int i = addr1; i <= addr2; ++i) {
        //first initialization of state
        if ((*state) == NULL && i == 1) {
            current_state *temp_state = (current_state *) malloc(sizeof(current_state));
            temp_state->strings = (char **) malloc(sizeof(char *));
            temp_state->length = 1;
            temp_state->strings[i - 1] = get_input(&c);
            (*state) = temp_state;
            push(&temp_redo, temp_state->strings[i - 1], i - addr1);
            push(&temp_undo, ".\n", i - addr1);
        } else {
            //add strings to state
            if (i >= (*state)->length) {
                char *temp = get_input(&c);
                if (temp[0] == '.' && temp[1] == '\n') break;//TODO DA TESTARE
                (*state)->strings = (char **) realloc((*state)->strings, ((*state)->length * 2) * sizeof(char *));
                (*state)->strings[i - 1] = temp;
                (*state)->length = (*state)->length * 2;
                push(&temp_redo, (*state)->strings[i - 1], i - addr1);
                push(&temp_undo, ".\n", i - addr1);
            } //modify already existing string
            else {
                char *temp = get_input(&c);
                if (temp[0] == '.' && temp[1] == '\n') break;//TODO DA TESTARE
                push(&temp_undo, (*state)->strings[i - 1], i - addr1);
                (*state)->strings[i - 1] = (char *) realloc((*state)->strings[i - 1], c * sizeof(char));
                (*state)->strings[i - 1] = temp;
                push(&temp_redo, (*state)->strings[i - 1], i - addr1);
            }
        }
    }
    (*undo) = temp_undo;
    (*redo) = temp_redo;
}

//delete current state string
void delete(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length) {}

//undo command
void undo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length) {}

//redo command
void redo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length) {}

//push to redo/undo
void push(commands **state, char *string, int index) {//index i-addr1
    //modified strings uninitialized
    if ((*state)->modified_strings == NULL) {
        (*state)->modified_strings = (char **) malloc(sizeof(char));
        (*state)->modified_strings[0] = string;
        (*state)->length = 1;
    } else {
        //modified strings resizing
        if (index > (*state)->length) {
            (*state)->modified_strings = (char **) realloc((*state)->modified_strings,
                                                           ((*state)->length * 2) * sizeof(char *));
            if (string == NULL) {
                (*state)->modified_strings[index] = ".\n";
            } else {
                (*state)->modified_strings[index] = string;
            }
            (*state)->length = (*state)->length * 2;
        } else //filling strings
        {
            if (string == NULL) {
                (*state)->modified_strings[index] = ".\n";
            } else {
                (*state)->modified_strings[index] = string;
            }
        }
    }
}
