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
char *get_input();

//get addresses function
void get_addresses(int *addr1, int *addr2, const char *cmd, size_t cmd_length);

//print command
void print(current_state *state, int addr1, int addr2);

//change command
void
change(current_state *state, int addr1, int addr2, commands **undo,  commands**redo, bool is_redo);

//delete command
void
delete(current_state *state, int addr1, int addr2, commands **undo,  commands**redo, bool is_redo);

//undo command
void undo(current_state *state, int addr1, char next_command, commands **undo, commands **redo);

//redo command
void redo(current_state *state, int addr1, commands **undo, commands **redo);

//push into redo/undo struct
void push(commands *state, char *string, int index);

void undo_change(current_state *state, commands *undo, commands **redo);

void undo_delete(current_state *state, commands *undo, commands **redo);

//free nodes of undo/redo struct
commands *empty(commands *state);

//push
int main() {
    //struct storing current strings and relative length
    current_state *state = (current_state *) calloc(1, sizeof(current_state));
    //struct storing redo and relative strings
    commands *redo_state = NULL;
    //struct storing undo and relative strings
    commands *undo_state = NULL;
    //command range
    int addr1 = 0, addr2 = 0, read = 1;
    //command identifier 'p', 'c', 'd', 'r', 'u', 'q'
    char c, d, temp;
    //command string
    char *cmd;
    char *next_cmd = NULL;
    char *ptr; //used only for u/r
    //array length
    size_t cmd_length = 0;
    size_t next_cmd_length = 0;
    while (true) {
        if (read == 1) {
            //get command from stdin
            cmd = get_input();

            cmd_length = strlen(cmd);
            //get identifier
            c = cmd[cmd_length - 2];
        }

       if (c=='r'|| c=='u'){
           addr1 = (int) strtol(cmd, &ptr, 10);
       } else            get_addresses(&addr1, &addr2, cmd, cmd_length);
        //identify command
        switch (c) {
            case ('p'):
                print(state, addr1, addr2);
                read = 1;
                break;
            case ('c'):
                change(state, addr1, addr2,&undo_state, &redo_state, false);
                read = 1;
                break;
            case ('d'):
                delete(state, addr1, addr2, &undo_state, &redo_state, false);
                read = 1;
                break;
            case ('u'):
                undo(state, addr1, d, &undo_state, &redo_state);
                cmd = next_cmd;
                cmd_length = next_cmd_length;
                c = d;
                next_cmd = NULL;
                break;
            case ('r'):
                redo(state, addr1, &undo_state, &redo_state);
                cmd = next_cmd;
                cmd_length = next_cmd_length;
                c = d;
                next_cmd = NULL;
                break;
            case ('q'):
                return 0;
            case ('.'):
                break;
            default:
                // printf("ARE YOU WINNING SON???");
                c = temp;
                read = 0;
                break;
        }
    }
}

char *get_input() {
    //input string
    char input[1025];
    //scan stdin input until it reaches '\n' (included)
    fgets(input, 1024, stdin);
    char *ptr = malloc((strlen(input) + 1) * sizeof(char));
    strcpy(ptr, input);
    return ptr;
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
void
change(current_state *state, int addr1, int addr2, commands **undo, commands**redo, bool is_redo) {
    commands *temp_undo = (commands *) malloc(sizeof(commands));
    //undo/redo temp nodes
        temp_undo->command = 'c';
        temp_undo->addr1 = addr1;
        temp_undo->addr2 = addr2;
        temp_undo->next = (*undo);
        temp_undo->modified_strings = NULL;
        temp_undo->length = 0;
    //string length used for resizing
    size_t c = 0;
    for (int i = addr1; i <= addr2; ++i) {
        //first initialization of state
        if (state->strings == NULL && i == 1) {
            //initialize first node
            state->strings = (char **) malloc(sizeof(char *));
            if (is_redo){
                state->strings[0] = (*redo)->modified_strings[0];
            } else{
                state->strings[0] = get_input();
            }
            state->length++;
            state->mem_len++;
            //fill undo/redo
                push(temp_undo, NULL, i - addr1);
        } else {
            //index out of bound, more memory allocated needed
            if (i > state->mem_len) {
                char *temp;
                if(is_redo){
                    temp=(*redo)->modified_strings[i-addr1];
                } else{
                    temp = get_input();
                }
                state->mem_len = state->mem_len + 2;
                state->strings = (char **) realloc(state->strings, state->mem_len * sizeof(char *));
                state->strings[i - 1] = temp;
                state->length++;
                //fill undo/redo
                    push(temp_undo, NULL, i - addr1);
            } //modify already existing string
            else {
                char *temp;
                if(is_redo){
                    temp=(*redo)->modified_strings[i-addr1];
                } else{
                    temp = get_input();
                }
                if (i > state->length) {
                        push(temp_undo, NULL, i - addr1);
                    state->length++;
                } else {
                        push(temp_undo, state->strings[i - 1], i - addr1);
                    //todo realloc scomparsa funzionerà?
                }
                state->strings[i - 1] = temp;
            }
        }
    }
        (*undo) = temp_undo;
}

//delete current state string
void
delete(current_state *state, int addr1, int addr2, commands **undo, commands**redo, bool is_redo) {
    commands *temp_undo = (commands *) malloc(sizeof(commands));

    //undo/redo temp nodes
        temp_undo->command = 'd';
        temp_undo->addr1 = addr1;
        temp_undo->addr2 = addr2;
        temp_undo->next = (*undo);
        temp_undo->modified_strings = NULL;
        temp_undo->length = 0;
    if (addr1 > state->length) {
        return;
    } else {
        if (addr2 >= state->length) {
            int i;
            for (i = addr1 - 1; i < state->length; ++i) {
                if (i < 0) {
                    break;
                }
                    if (state->length == 0) {
                        push(temp_undo, NULL, i - addr1 + 1);
                    }
                    push(temp_undo, state->strings[i], i - addr1 + 1);
                state->strings[i] = NULL;
                state->length--;
            }
            while (i < addr2) {
                push(temp_undo, NULL, i - addr1 + 1);
                i++;
            }
        } else {
            for (int i = addr1; i <= addr2; ++i) {
                if (i == 0) {
                    i++;
                } else {
                        push(temp_undo, state->strings[i - 1], i - addr1);
                    state->strings[i - 1] = NULL;
                }
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
            state->length = i - 1;
        }
            (*undo) = temp_undo;
    }

}

//undo command
void undo(current_state *state, int addr1, char next_command, commands **undo, commands **redo) {
    if (state == NULL || (*undo) == NULL || addr1 == 0) {
        return;
    } else {
        int i = 0;
        commands *temp;
        while ((*undo) != NULL) {
            if (i != addr1-1) {
                if ((*undo)->command == 'c') {
                    undo_change(state, (*undo), redo);
                } else {
                    if ((*undo)->modified_strings != NULL) {
                        undo_delete(state, (*undo), redo);
                    }
                }
                temp = (*undo);
                (*undo) = (*undo)->next;
                free(temp);
                i++;
            } else break;
        }
        if (next_command == 'c' || next_command == 'd') {
            (*redo) = empty((*redo));
        }
    }
}

//redo command
void redo(current_state *state, int addr1, commands **undo, commands **redo) {
    if ((*redo) == NULL || addr1 == 0) {
        return;
    } else {
        int i = 0;
        commands *temp;
        while ((*redo) != NULL) {
            if (i != addr1-1) {
                if ((*redo)->command == 'c') {
                    change(state, (*redo)->addr1, (*redo)->addr2,undo, redo,true);
                } else {
                    delete(state, (*redo)->addr1, (*redo)->addr2, undo, redo, true);
                }
                temp = (*redo);
                (*redo) = (*redo)->next;
                free(temp);
                i++;
            } else break;
        }
    }
}

/*
 * push function: given redo/undo struct, fill relative modified_strings with provided string, if this one is empty it
 * fill with ".\n"
 */
void push(commands *state, char *string, int index) {//index i-addr1
    //modified strings uninitialized
    if (state->modified_strings == NULL) {
        state->modified_strings = (char **) malloc(1 * sizeof(char *));
        if (string == NULL) {
            state->modified_strings[0] = ".\n";
        } else {
            state->modified_strings[0] = string;
        }
        state->length = 1;
    } else {
        //modified strings resizing
        if (index >= state->length) {
            state->modified_strings = (char **) realloc(state->modified_strings,
                                                        (state->length + 1) * sizeof(char *));
            if (string == NULL) {
                state->modified_strings[index] = ".\n";
            } else {
                state->modified_strings[index] = string;
            }
            state->length = state->length + 1;
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

/*
 * empty function: given redo/undo struct, empty provided one.
 */
commands *empty(commands *state) {
    while (state != NULL) {
        commands *temp = state->next;
        for (int i = 1; i < state->length; ++i) {
            free(state->modified_strings[i - 1]);
        }
        free(state->modified_strings);
        free(state);
        state = temp;
    }
    return state;
}

void undo_change(current_state *state, commands *undo, commands **redo) {
    commands *temp_redo = (commands *) malloc(sizeof(commands));
    temp_redo->command = 'c';
    temp_redo->addr1 = undo->addr1;
    temp_redo->addr2 = undo->addr2;
    temp_redo->next = (*redo);
    temp_redo->length = 0;
    temp_redo->modified_strings = NULL;
    for (int i = undo->addr1; i <= undo->addr2; ++i) {
        if (undo->modified_strings[i - 1][0] == '.' && undo->modified_strings[i - 1][1] == '\n') {
            push(temp_redo, state->strings[i - 1], i - undo->addr1);
            state->strings[i - 1] = NULL;
            state->length--;
        } else {
            if (state->strings == NULL || i > state->length) {
                push(temp_redo, NULL, i - undo->addr1);
                state->length++;
                state->strings = realloc(state->strings, state->length * sizeof(char *));
                state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
            } else {
                push(temp_redo, state->strings[i - 1], i - undo->addr1);
                state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
            }
        }
    }
    (*redo) = temp_redo;
}

void undo_delete(current_state *state, commands *undo, commands **redo) {
    commands *temp_redo = (commands *) malloc(sizeof(commands));
    temp_redo->command = 'd';
    temp_redo->addr1 = undo->addr1;
    temp_redo->addr2 = undo->addr2;
    temp_redo->next = (*redo);
    temp_redo->length = 0;
    temp_redo->modified_strings = NULL;
    if (state->strings[undo->addr1 - 1] != NULL) {
        for (int i = undo->addr1; i <= undo->addr2; ++i) {
            char *temp = state->strings[i - 1];
            state->strings = (char **) realloc(state->strings, (state->length + 1) * sizeof(char *));
            state->length++;
            state->strings[state->length - 1] = temp;
            state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
        }
    } else {
        for (int i = undo->addr1; i < undo->addr2; ++i) {
            state->strings = (char **) realloc(state->strings, (state->length + 1) * sizeof(char *));
            state->length++;
            state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
        }
    }
    (*redo) = temp_redo;
}