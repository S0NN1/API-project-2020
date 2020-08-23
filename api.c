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

void input_checker(int undo_len, int *u_on_hold, int u_done, int *sum, bool *pending_undo, char (*u_r), const char *cmd,
                   char **ptr,
                   size_t cmd_length, int *addr1, int *addr2, bool *read, char *c);

void get_addresses(int *addr1, int *addr2, const char *cmd, size_t cmd_length);

char *get_input();

bool is_empty(const char *string);

commands *empty(commands *state);

void initialize_node(commands *temp, commands *const *main_struct, char command, int addr1, int addr2);

void push(commands *state, char *string, int index);

void print(current_state *state, int addr1, int addr2);

void
change(current_state *state, int addr1, int addr2, commands **undo, commands **redo, bool is_redo);

void
delete(current_state *state, int addr1, int addr2, commands **undo);

void redo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len);

void undo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len);

void undo_change(current_state *state, commands *undo, commands **redo);

void undo_delete(current_state *state, commands *undo, commands **redo);


int main() {
    //struct storing current strings and relative length
    current_state *state = (current_state *) calloc(1, sizeof(current_state));
    //struct storing redo and relative strings
    commands *redo_state = NULL;
    //struct storing undo and relative strings
    commands *undo_state = NULL;
    //command range
    int addr1 = 0, addr2 = 0;
    int undo_len = 0, redo_len = 0, u_done = 0, sum = 0, u_on_hold = 0, temp=0;
    bool pending_undo = false, read = true;
    //command identifier 'p', 'c', 'd', 'r', 'u', 'q'
    char c, u_r = 0;
    //command string
    char *cmd;
    char *ptr; //used only for u/r
    //array length
    size_t cmd_length = 0;
    while (true) {
        if (read) {
            cmd = get_input();
            cmd_length = strlen(cmd);
        }
        //get identifier
        c = cmd[cmd_length - 2];
        input_checker(undo_len, &u_on_hold, u_done, &sum, &pending_undo, &u_r, cmd, &ptr, cmd_length, &addr1, &addr2,
                      &read, &c);
        //identify command
        switch (c) {
            case ('p'):
                print(state, addr1, addr2);
                read = true;
                sum = 0;
                break;
            case ('c'):
                change(state, addr1, addr2, &undo_state, &redo_state, false);
                undo_len++;
                redo_state = empty(redo_state);
                redo_len = 0;
                read = true;
                sum = 0;
                break;
            case ('d'):
                delete(state, addr1, addr2, &undo_state);
                undo_len++;
                redo_state = empty(redo_state);
                redo_len = 0;
                read = true;
                sum = 0;
                break;
            case ('u'):
                temp=undo_len;
                undo(state, addr1, &undo_state, &redo_state, &undo_len, &redo_len);
                if (undo_len == 0) {
                    u_done += temp;
                } else {
                    u_done += addr1;
                }
                u_on_hold = 0;
                pending_undo = false;
                sum = 0;
                break;
            case ('r'):
                redo(state, addr1, &undo_state, &redo_state, &undo_len, &redo_len);
                u_on_hold = 0;
                pending_undo = false;
                sum = 0;
                break;
            case ('q'):
                return 0;
            default:
                //printf("ARE YOU WINNING SON???");
                break;
        }
    }
}


void input_checker(int undo_len, int *u_on_hold, int u_done, int *sum, bool *pending_undo, char *u_r, const char *cmd,
                   char **ptr,
                   size_t cmd_length, int *addr1, int *addr2, bool *read, char *c) {
    if ((*c) == 'u' || (*c) == 'r') {
        (*addr1) = (int) strtol(cmd, ptr, 10);
        (*pending_undo) = true;
        if ((*c) == 'u') {
            if ((*addr1) <= undo_len) {
                (*sum) += (*addr1);
            } else (*sum) += undo_len;
            (*u_on_hold) += (*addr1);
        } else {
            if ((*addr1) <= u_done + (*u_on_hold)) {
                (*sum) -= (*addr1);
            } else
                (*sum) -= u_done + (*u_on_hold);
        }
        if ((*sum) < 0) {
            (*u_r) = 'r';
        } else (*u_r) = 'u';
        (*c) = 'n';
    } else if ((*c) != '.' && (*c) != 'q' && sum != 0 && (*pending_undo) == true) {
        (*c) = (*u_r);
        if ((*sum) < 0) {
            (*addr1) = -(*sum);
        } else (*addr1) = (*sum);
        (*read) = false;
    } else {
        get_addresses(addr1, addr2, cmd, cmd_length);
    }
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

char *get_input() {
    //input string
    char input[1025];
    //scan stdin input until it reaches '\n' (included)
    fgets(input, 1024, stdin);
    char *ptr = malloc((strlen(input) + 1) * sizeof(char));
    strcpy(ptr, input);
    return ptr;
}

bool is_empty(const char *string) {
    return string ==NULL ||(string[0] == '.' && string[1] == '\n');
}

commands *empty(commands *state) {
    while (state != NULL) {
        commands *temp = state->next;
        for (int i = 0; i < state->length; ++i) {
            if (state->modified_strings != NULL) {
                state->modified_strings[i] = NULL;
            }
        }
        free(state->modified_strings);
        state->modified_strings = NULL;
        free(state);
        state = NULL;
        state = temp;
    }
    state = NULL;
    return state;
}

void initialize_node(commands *temp, commands *const *main_struct, char command, int addr1, int addr2) {
    temp->command = command;
    temp->addr1 = addr1;
    temp->addr2 = addr2;
    temp->next = (*main_struct);
    temp->modified_strings = NULL;
    temp->length = 0;
}

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
            state->length++;
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

void
change(current_state *state, int addr1, int addr2, commands **undo, commands **redo, bool is_redo) {
    commands *temp_undo = (commands *) malloc(sizeof(commands));
    initialize_node(temp_undo, undo, 'c', addr1, addr2);
    //string length used for resizing
    size_t c = 0;
    for (int i = addr1; i <= addr2; ++i) {
        //first initialization of state
        if (state->strings == NULL && i == 1) {
            //initialize first node
            state->strings = (char **) malloc(sizeof(char *));
            if (is_redo) {
                state->strings[0] = (*redo)->modified_strings[0];
            } else {
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
                if (is_redo) {
                    temp = (*redo)->modified_strings[i - addr1];
                } else {
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
                if (is_redo) {
                    temp = (*redo)->modified_strings[i - addr1];
                } else {
                    temp = get_input();
                }
                if (i > state->length) {
                    push(temp_undo, NULL, i - addr1);
                    state->length++;
                } else {
                    push(temp_undo, state->strings[i - 1], i - addr1);
                }
                state->strings[i - 1] = temp;
            }
        }
    }
    (*undo) = temp_undo;
}

void
delete(current_state *state, int addr1, int addr2, commands **undo) {
    commands *temp_undo = (commands *) malloc(sizeof(commands));
    initialize_node(temp_undo, undo, 'd', addr1, addr2);
    //undo/redo temp nodes
    if (addr1 > state->length) {
        (*undo) = temp_undo;
        return;
    } else {
        if (addr2 >= state->length) {
            int i;
            for (i = addr1 - 1; i < state->length; ++i) {
                if (i >= 0) {
                    if (state->length != 0) {
                        push(temp_undo, state->strings[i], i - addr1 + 1);
                    }
                    state->strings[i] = NULL;
                    state->length--;
                }
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
            int i;
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

void redo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len) {
    if ((*redo) == NULL || addr1 == 0) {
        return;
    } else {
        int i = 0;
        commands *temp;
        while ((*redo) != NULL) {
            if (i != addr1) {
                if ((*redo)->command == 'c') {
                    change(state, (*redo)->addr1, (*redo)->addr2, undo, redo, true);
                } else {
                    delete(state, (*redo)->addr1, (*redo)->addr2, undo);
                }
                (*redo_len)--;
                (*undo_len)++;
                temp = (*redo);
                (*redo) = (*redo)->next;
                free(temp);
                i++;
            } else break;
        }
    }
}

void undo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len) {
    if (state == NULL || (*undo) == NULL || addr1 == 0) {
        return;
    } else {
        int i = 0;
        commands *temp;
        while ((*undo) != NULL) {
            if (i != addr1) {
                if ((*undo)->command == 'c') {
                    undo_change(state, (*undo), redo);
                } else {
                    if ((*undo)->modified_strings != NULL) {
                        undo_delete(state, (*undo), redo);
                    } else {
                        commands *temp_redo = (commands *) malloc(sizeof(commands));
                        initialize_node(temp_redo, redo, 'd', (*undo)->addr1, (*undo)->addr2);
                        (*redo) = temp_redo;
                    }
                }
                (*redo_len)++;
                (*undo_len)--;
                temp = (*undo);
                (*undo) = (*undo)->next;
                free(temp);
                i++;
            } else break;
        }
    }
}

void undo_change(current_state *state, commands *undo, commands **redo) {
    commands *temp_redo = (commands *) malloc(sizeof(commands));
    initialize_node(temp_redo, redo, 'c', undo->addr1, undo->addr2);
    for (int i = undo->addr1; i <= undo->addr2; ++i) {
        if (is_empty(undo->modified_strings[i - undo->addr1])) {
            push(temp_redo, state->strings[i - 1], i - undo->addr1);
            state->strings[i - 1] = NULL;
            if (undo->addr2 <= state->length) {
                state->length--;
            }
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
    initialize_node(temp_redo, redo, 'd', undo->addr1, undo->addr2);
    //shifting state strings or invalid delete
    if (state->strings[undo->addr1 - 1] != NULL && !is_empty(undo->modified_strings[0])) {
        for (int i = undo->addr1; i <= undo->addr2; ++i) {
            char *temp = state->strings[i - 1];
            if (!is_empty(state->strings[state->length - 1] )){
                state->strings = (char **) realloc(state->strings, (state->length + 1) * sizeof(char *));
                state->length++;
            }
            state->strings[state->length - 1] = temp;
            state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
        }
    } else if (!is_empty(undo->modified_strings[0])) {
        for (int i = undo->addr1; i <= undo->addr2; ++i) {
            if (i > state->mem_len) {
                state->strings = (char **) realloc(state->strings, (state->length + 1) * sizeof(char *));
            }
            if (i > state->length) {
                state->length++;
            }
            state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
        }
    }
    (*redo) = temp_redo;
}

