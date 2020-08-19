#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

typedef struct {
    char **strings;
    size_t length;
    size_t mem_len;
} current_state;

typedef struct cmds {
    char command;
    int addr1;
    int addr2;
    char **modified_strings;
    int length;
    struct cmds *next;
} commands;

//get input function
char *get_input(size_t *input_length);

//get addresses function
void get_addresses(int *addr1, int *addr2, const char *cmd, size_t cmd_length);

//print command
void print(current_state *state, int addr1, int addr2);

//change command
void
change(current_state *state, int addr1, int addr2, char command, commands **redo, commands **undo);

//delete command
void delete(current_state *state, int addr1, int addr2, char command, commands **redo, commands **undo);

//undo command
void undo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length);

//redo command
void redo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length);

//push into redo/undo struct
void push(commands *state, char *string, int index);

//push
int main() {
    //struct storing current strings and relative length
    current_state *state = (current_state *) calloc(1, sizeof(current_state));
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
    char *ptr; //used only for u/r
    //array length
    size_t cmd_length = 0;
    while (true) {
        //get command from stdin
        cmd = get_input(&cmd_length);
        //get identifier
        c = cmd[cmd_length - 2];
        //get addresses
        if (c == 'd' || c == 'c' || c == 'p') {
            get_addresses(&addr1, &addr2, cmd, cmd_length);
        } else {
            if (c != 'q' && c != '.') {
                addr1 = (int) strtol(cmd, &ptr, 10);
                addr2 = 0;
            }
        }
        //identify command
        switch (c) {
            case ('p'):
                print(state, addr1, addr2);
                break;
            case ('c'):
                change(state, addr1, addr2, c, &redo_state, &undo_state);
                break;
            case ('d'):
                delete(state, addr1, addr2, c, &redo_state, &undo_state);
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
    //input string
    char *input = (char *) calloc(1025, sizeof(char));//TODO EFFICIENCY
    //scan stdin input until it reaches '\n' (included)
    fgets(input, 1025, stdin);
    (*input_length) = strlen(input);
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
    temp[end] = '\0';
    char *ptr = NULL;
    //assign first address
    (*addr1) = (int) strtol(temp, &ptr, 0);
    //reach '\n'
    int j = 0;
    for (int i = end + 1; i < cmd_length; ++i) {
        if (cmd[i] > 96 && cmd[i] < 123) {
            break;
        }
        temp[j] = cmd[i];
        j++;
    }
    temp[j] = '\0';
    //assign second address
    (*addr2) = (int) strtol(temp, &ptr, 0);
    free(temp);
}

/*
 * print command: if state is uninitialized prints only ".\n", if address is included in current state it prints
 * relative string, otherwise it prints ".\n".
 */
void print(current_state *state, int addr1, int addr2) {
    //uninitialized strings (print before change)
    if (state->strings == NULL) {
        for (int i = addr1; i <= addr2; ++i) {
            printf(".\n");
        }
    } else {
        for (int i = addr1; i <= addr2; ++i) {
            //if string isn't included
            if (i == 0 || i > state->length ||
                state->strings[i - 1] == NULL) {
                printf(".\n");
            }
                //if string is included
            else {
                printf("%s", state->strings[i - 1]);
            }
        }
    }
}

/*
 * change command: if state is uninitialized it creates the struct and start filling in, if index i exceeds the length
 * it resizes the struct, otherwise changes the previous string.
 */
void change(current_state *state, int addr1, int addr2, char command, commands **redo, commands **undo) {
    //undo/redo temp nodes
    commands *temp_redo = (commands *) malloc(sizeof(commands));
    commands *temp_undo = (commands *) malloc(sizeof(commands));
    //set their command
    temp_redo->command = command;
    temp_undo->command = command;
    //set their addresses
    temp_undo->addr1 = addr1;
    temp_undo->addr2 = addr2;
    temp_redo->addr1 = addr1;
    temp_redo->addr2 = addr2;
    //add them at the top of the list
    temp_redo->next = (*redo);
    temp_undo->next = (*undo);
    //setting strings to NULL
    temp_redo->modified_strings = NULL;
    temp_undo->modified_strings = NULL;
    //setting length
    temp_redo->length = 0;
    temp_undo->length = 0;
    //string length used for resizing
    size_t c = 0;
    for (int i = addr1; i <= addr2; ++i) {
        //first initialization of state
        if (state->strings == NULL && i == 1) {
            //initialize first node
            char **strings = (char **) malloc(sizeof(char *));
            strings[0] = get_input(&c);
            //resizing to save memory
            strings[0] = (char *) realloc(strings[0], (c + 1) * sizeof(char));
            state->length++;
            state->mem_len++;
            state->strings = strings;
            //fill undo/redo
            push(temp_redo, state->strings[0], i - addr1);
            push(temp_undo, NULL, i - addr1);
        } else {
            //index out of bound, more memory allocated needed
            if (i > state->mem_len) {
                char *temp = get_input(&c);
                //resizing to save memory
                temp = (char *) realloc(temp, (c + 1) * sizeof(char));
                state->mem_len = (state->mem_len + 1) * 2;
                state->strings = (char **) realloc(state->strings, state->mem_len * sizeof(char *));
                state->strings[i - 1] = temp;
                state->length++;
                //fill undo/redo
                push(temp_redo, state->strings[i - 1], i - addr1);
                push(temp_undo, NULL, i - addr1);
            } //modify already existing string
            else {
                char *temp = get_input(&c);
                temp = (char *) realloc(temp, (c + 1) * sizeof(char));
                if (i > state->length) {
                    push(temp_undo, NULL, i - addr1);
                    state->strings[i - 1] = (char *) malloc((c + 1) * sizeof(char));
                    state->length++;
                } else {
                    push(temp_undo, state->strings[i - 1], i - addr1);
                    state->strings[i - 1] = (char *) realloc(state->strings[i - 1], (c + 1) * sizeof(char));//todo PORBLEMONE
                }
                state->strings[i - 1] = temp;

                //fill redo
                push(temp_redo, state->strings[i - 1], i - addr1);
            }
        }
    }
    (*undo) = temp_undo;
    (*redo) = temp_redo;
}

//delete current state string
void delete(current_state *state, int addr1, int addr2, char command, commands **redo, commands **undo) {
    //undo/redo temp nodes
    commands *temp_redo = (commands *) malloc(sizeof(commands));
    commands *temp_undo = (commands *) malloc(sizeof(commands));
    //set their command
    temp_redo->command = command;
    temp_undo->command = command;
    //set their addresses
    temp_undo->addr1 = addr1;
    temp_undo->addr2 = addr2;
    temp_redo->addr1 = addr1;
    temp_redo->addr2 = addr2;
    //add them at the top of the list
    temp_redo->next = (*redo);
    temp_undo->next = (*undo);
    //setting strings to NULL
    temp_redo->modified_strings = NULL;
    temp_undo->modified_strings = NULL;
    //setting length
    temp_redo->length = 0;
    temp_undo->length = 0;
    //invalid delete
    if (addr1 > state->length) {
        return;
    } else {
        if (addr2 >= state->length) {
            for (int i = addr1 - 1; i < (int) state->length - 1; ++i) {
                if (i == -1) {
                    break;
                }
                push(temp_undo, state->strings[i], i - addr1 + 1);
                free(state->strings[i]);
                state->strings[i] = NULL;
            }
            state->length = addr1 - 1;
            state->mem_len = addr1 - 1;
        } else {
            for (int i = addr1; i <= addr2; ++i) {
                if (i == 0) {
                    i++;
                }
                push(temp_undo, state->strings[i - 1], i - addr1);
                state->strings[i - 1] = NULL;
            }
            int k = addr1 - 1;
            int i = 0;
            for (i = addr2; i < state->length; ++i) {
                if (state->strings[i] != NULL) {
                    char *temp = state->strings[i];
                    state->strings[k] = temp;
                    state->strings[i] = NULL;
                    k++;
                } else break;
            }
            state->length = i;
            state->mem_len = i;
        }
    }
    (*undo) = temp_undo;
    (*redo) = temp_redo;
}

//undo command
void undo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length) {}

//redo command
void redo(current_state *state, int addr1, int addr2, char *cmd, size_t cmd_length) {}

/*
 * push function: given redo/undo fill relative modified_strings with provided string, if this one is empty it fill with
 * ".\n"
 */
void push(commands *state, char *string, int index) {//index i-addr1
    //modified strings uninitialized
    if (state->modified_strings == NULL) {
        state->modified_strings = (char **) calloc(1, sizeof(char *));//TODO NOT SURE VALGRIND ROMPE
        state->modified_strings[0] = string;
        state->length = 1;
    } else {
        //modified strings resizing
        if (index >= state->length) {
            state->modified_strings = (char **) realloc(state->modified_strings,
                                                           (state->length +1) * sizeof(char *));
            if (string == NULL) {
                state->modified_strings[index] = ".\n";
            } else {
                state->modified_strings[index] = string;
            }
            state->length = state->length+1;
        } else //filling strings
        {
            if (string == NULL) {
                state->modified_strings[index] = ".\n";
            } else {
                state->modified_strings[index] = string;
            }
        }
    }
}