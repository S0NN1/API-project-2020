#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#define NIGGER 32768
int frigo_var=0;
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
    int mem_len;
    struct cmds *next;
} commands;

void input_checker(int undo_len, int *u_on_hold, int u_done, int *sum, bool *pending_undo, char (*u_r), const char *cmd,
                   char **ptr,
                   size_t cmd_length, int *addr1, int *addr2, bool *read, char *c);

void get_addresses(int *addr1, int *addr2, const char *cmd, size_t cmd_length);

char *get_input();

commands *empty(commands *state);

void initialize_node(commands *temp, commands *const *main_struct, char command, int addr1, int addr2);

void push(commands *state, char *string, int index, int addr1, int addr2, int state_len);

void print(current_state *state, int addr1, int addr2);

void
change(current_state *state, int addr1, int addr2, commands **undo, commands **redo, bool is_redo);

void
delete(current_state *state, int addr1, int addr2, commands **undo, commands**redo, bool is_redo);

void redo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len);

void undo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len);

void undo_change(current_state *state, commands *undo, commands **redo);

void undo_delete(current_state *state, commands *undo, commands **redo);


int main() {
    //struct storing current strings and relative length
    current_state *state = (current_state *) calloc(1, sizeof(current_state));
    state->strings=realloc(state->strings, NIGGER*sizeof(char*));
    state->mem_len=NIGGER;
    //struct storing redo and relative strings
    commands *redo_state = NULL;
    //struct storing undo and relative strings
    commands *undo_state = NULL;
    //command range
    int addr1 = 0, addr2 = 0;
    int undo_len = 0, redo_len = 0, u_done = 0, sum = 0, u_on_hold = 0, temp;
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
                u_done = 0;
                break;
            case ('d'):
                delete(state, addr1, addr2, &undo_state, &redo_state, false);
                undo_len++;
                redo_state = empty(redo_state);
                redo_len = 0;
                read = true;
                sum = 0;
                u_done = 0;
                break;
            case ('u'):
                temp = undo_len;
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
                temp = redo_len;
                redo(state, addr1, &undo_state, &redo_state, &undo_len, &redo_len);
                if (redo_len == 0) {
                    u_done -= temp;
                } else u_done -= addr1;
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
            if ((*addr1) <= undo_len - (*sum)) {
                (*sum) += (*addr1);
            } else (*sum) = undo_len;
            (*u_on_hold) += (*sum);
        } else {
            if ((*addr1) <= u_done + (*sum)) {
                (*sum) -= (*addr1);
            } else
                (*sum) = -u_done;
            (*u_on_hold) = (*sum);
        }
        if ((*sum) < 0) {
            (*u_r) = 'r';
            (*u_on_hold) = 0;
        } else (*u_r) = 'u';
        (*c) = 'n';
    } else if ((*c) != '.' && (*c) != 'q' && sum != 0 && (*pending_undo)) {
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
    char temp[20];
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
}

char *get_input() {
    //input string
    char input[1025];
    char c;
    int len = 0;
    while ((c = getchar_unlocked()) != EOF) {
        input[len] = c;
        len++;
        if (input[len - 1] == '\n')
            break;
    }
    input[len] = '\0';
    char *temp = malloc((len + 1) * sizeof(char));
    memcpy(temp, input, len + 1);
    return temp;
}

commands *empty(commands *state) {
    while (state != NULL) {
        commands *temp = state->next;
        if (state->modified_strings != NULL && state->modified_strings[0]!=NULL) {
           for (int i = 0; i < state->length; ++i) {
                free(state->modified_strings[i]);
            }
            free(state->modified_strings);
        } else state->modified_strings=NULL;
        free(state);
       state = temp;
    }
    return NULL;
}

void initialize_node(commands *temp, commands *const *main_struct, char command, int addr1, int addr2) {
    temp->command = command;
    temp->addr1 = addr1;
    temp->addr2 = addr2;
    temp->next = (*main_struct);
    temp->modified_strings = NULL;
    temp->length = 0;
    temp->mem_len = 0;
}

void push(commands *state, char *string, int index, int addr1, int addr2, int state_len) {
    if (string == NULL) {
        return;
    }
    if (state->modified_strings == NULL) {
        if (addr2 < state_len) {
            state->mem_len = addr2 - addr1 + 1;
        } else
            state->mem_len = state_len - addr1 + 1;
        state->modified_strings = (char **) malloc(state->mem_len*sizeof(char *));
        state->modified_strings[0] = string;
        state->length = 1;
    } else {
        state->length++;
        state->modified_strings[state->length - 1] = string;
    }
}

void print(current_state *state, int addr1, int addr2) {
    //uninitialized strings (print before change)
    if (state->strings == NULL) {
        for (int i = addr1; i <= addr2; ++i) {
            fputs(".\n", stdout);
        }
    } else {
        for (int i = addr1; i <= addr2; ++i) {
            //if string isn't included
            if (i == 0 || i > state->length ||
                state->strings[i - 1] == NULL) {
                fputs(".\n", stdout);
            }
                //if string is included
            else {
                fputs(state->strings[i - 1], stdout);
            }
        }
    }
}

void
change(current_state *state, int addr1, int addr2, commands **undo, commands **redo, bool is_redo) {
    commands *temp_undo = (*redo);
    char **old_strings;
    if (!is_redo) {
        temp_undo = (commands *) malloc(sizeof(commands));
        initialize_node(temp_undo, undo, 'c', addr1, addr2);
    } else {
        old_strings= (*redo)->modified_strings;
        temp_undo->addr1 =(*redo)->addr1;
        temp_undo->addr2 =(*redo)->addr2;
        temp_undo->length=0;
        temp_undo->mem_len = (*redo)->addr2 - (*redo)->addr1 + 1;
        temp_undo->modified_strings = malloc(temp_undo->mem_len*sizeof(char *));
    }
    //string length used for resizing
    int old_len = state->length;
    if (state->mem_len < addr2) {
        state->mem_len*=2;
        state->strings = (char **) realloc(state->strings, state->mem_len * sizeof(char *));
    }
    for (int i = addr1; i <= addr2; ++i) {
        char *temp;
        if (is_redo) {
            temp = old_strings[i - addr1];
        } else {
            temp = get_input();
        }
        if (i > state->length) {
            state->length++;
        }
        if (i <= old_len) {
            push(temp_undo, state->strings[i - 1], i - addr1, addr1, addr2, old_len);
        }
        state->strings[i - 1] = temp;
    }
    if (is_redo){
        free(old_strings);
    }
    temp_undo->next=(*undo);
    (*undo) = temp_undo;
}

void
delete(current_state *state, int addr1, int addr2, commands **undo, commands **redo, bool is_redo) {
    commands *temp_undo;
    if (!is_redo){
        temp_undo = (commands *) malloc(sizeof(commands));
    } else  temp_undo= (*redo);
    initialize_node(temp_undo, undo, 'd', addr1, addr2);
    int old_len;
    //undo/redo temp nodes
    if (addr1 > state->length || (addr1 == 0 && addr2 == 0)) {
        (*undo) = temp_undo;
        return;
    } else {
        if (addr1 == 0) {
            addr1 = 1;
            temp_undo->addr1 = 1;
        }
        if (addr2 > state->length) {
            addr2 = state->length;
            temp_undo->addr2 = addr2;
        }
        old_len = state->length;
        for (int i = addr1; i <= addr2; ++i) {
            push(temp_undo, state->strings[i - 1], i - addr1, addr1, addr2, addr2);
            state->strings[i - 1] = NULL;
            state->length--;
        }
        if (addr2 == old_len) {
            (*undo) = temp_undo;
            return;
        }
        int k = addr1 - 1;
        for (int i = addr2; i < old_len; ++i) {
            if (state->strings[i] != NULL) {
                char *temp = state->strings[i];
                state->strings[k] = temp;
                state->strings[i] = NULL;
                k++;
            } else break;
        }
    }
    temp_undo->length = temp_undo->mem_len;
    (*undo) = temp_undo;
}

void redo(current_state *state, int addr1, commands **undo, commands **redo, int *undo_len, int *redo_len) {
    if ((*redo) == NULL || addr1 == 0) {
        return;
    } else {
        int i = 0;
        commands *next;
        while ((*redo) != NULL) {
            next=(*redo)->next;
            if (i != addr1) {
                if ((*redo)->command == 'c') {
                    change(state, (*redo)->addr1, (*redo)->addr2, undo, redo, true);
                } else {
                    delete(state, (*redo)->addr1, (*redo)->addr2, undo, redo, true);
                }
                (*redo_len)--;
                (*undo_len)++;
                (*redo) = next;
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
        while ((*undo) != NULL) {
            commands *next = (*undo)->next;
            if (i != addr1) {
                if ((*undo)->command == 'c') {
                    undo_change(state, (*undo), redo);
                } else {
                    if ((*undo)->modified_strings != NULL) {
                        undo_delete(state, (*undo), redo);
                    } else {
                        (*undo)->next = (*redo);
                        free((*undo)->modified_strings);
                        (*undo)->modified_strings = NULL;
                        (*redo) = (*undo);
                    }
                }
                (*redo_len)++;
                (*undo_len)--;
                (*undo) = next;
                i++;
            } else break;
        }
    }

}

void undo_change(current_state *state, commands *undo, commands **redo) {
    commands *temp_redo = undo;
    char **temp_strings = undo->modified_strings;
    //initialize_node(temp_redo, redo, 'c', undo->addr1, undo->addr2);
    if (undo->modified_strings == NULL) {
        temp_redo->length = 0;
        for (int i = undo->addr1; i <= undo->addr2; ++i) {
            push(temp_redo, state->strings[i - 1], i - undo->addr1, undo->addr1, undo->addr2, undo->addr2);
            state->strings[i - 1] = NULL;
            if (state->length > 0) {
                state->length--;
            }
        }
    } else {
        int i;
        int old_len = temp_redo->length;
        temp_redo->length = 0;
        temp_redo->mem_len = undo->addr2 - undo->addr1 + 1;
        temp_redo->modified_strings = malloc(temp_redo->mem_len*sizeof(char *));
        for (i = undo->addr1; i <= undo->addr2; ++i) {
            if (i - undo->addr1 < old_len) {
                push(temp_redo, state->strings[i - 1], i - undo->addr1, undo->addr1, undo->addr2, undo->addr2);
                state->strings[i - 1] = temp_strings[i - undo->addr1];
            } else {
                push(temp_redo, state->strings[i - 1], i - undo->addr1, undo->addr1, undo->addr2, undo->addr2);
                state->strings[i - 1] = NULL;
                if (state->length > 0) {
                    state->length--;
                }
            }
        }
    }
    free(temp_strings);
    temp_redo->next = (*redo);
    (*redo) = temp_redo;
}

void undo_delete(current_state *state, commands *undo, commands **redo) {
    commands *temp_redo = undo;
    int i;
    //initialize_node(temp_redo, redo, 'd', undo->addr1, undo->addr2);
    //shifting state strings or invalid delete
    if (undo->modified_strings == NULL) {
        temp_redo->next = (*redo);
        (*redo) = temp_redo;
        return;
    }
    int old_len = state->length;
    for (i = undo->addr1; i <= undo->addr2; ++i) {
        char *temp = state->strings[i - 1];
        int j = 1;
        while (temp != NULL) {
            char *shifted = state->strings[i - 1 + (j - 1) * (undo->addr2 - undo->addr1 + 1)];
            if (j == 1)
                state->strings[i - 1 + (j - 1) * (undo->addr2 - undo->addr1 + 1)] = undo->modified_strings[i -
                                                                                                           undo->addr1];
            else {
                state->strings[i - 1 + (j - 1) * (undo->addr2 - undo->addr1 + 1)] = temp;
            }
            temp = shifted;
            j++;
        }
        if (i > old_len) {
            state->strings[i - 1] = undo->modified_strings[i - undo->addr1];
        }
        state->length++;
    }
    temp_redo->next = (*redo);
    free(undo->modified_strings);
    undo->modified_strings = NULL;
    (*redo) = temp_redo;
}