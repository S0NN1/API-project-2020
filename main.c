#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

struct array {
    char *data;
    struct array *prev;
    struct array *next;
};

struct saved_commands {
    struct array *commands;
    struct saved_commands *next;
};

void divide_addr(int *ind1, int *ind2, char *cmd, int length);

void print_str(char **strings, int length, int addr1, int addr2);


void change_str(char ***strings, int addr1, int addr2, int *length, int len, struct saved_commands **undo,
                struct saved_commands **redo, char *cmd);



//void delstr();

//void undo();

//void redo();


int main() {
    struct saved_commands *redo = NULL;
    struct saved_commands *undo = NULL;
    int addr1 = 0, addr2 = 0;
    int cmd_length = 0;
    int *ind1;
    int *ind2;
    int *length;
    char **strings = (char **) malloc(sizeof(char *));
    int strings_length = 1;
    char *cmd = NULL;
    char c;
    int index = 0;
    ind1 = &addr1;
    ind2 = &addr2;
    length = &strings_length;
    while (true) {
        do {
            c = getchar_unlocked();
            if (cmd == NULL) {
                cmd = (char *) malloc(sizeof(char));
                cmd[0] = c;
                index++;
            } else {
                if (index >= cmd_length) {
                    cmd = realloc(cmd, ((index + 2) * 2) * sizeof(char));
                    cmd_length = ((index + 2) * 2);
                    cmd[index] = c;
                    index++;
                } else {
                    cmd[index] = c;
                    index++;
                }
            }
        } while (c != '\n');
        if (index < cmd_length) {
            cmd = (char *) realloc(cmd, index * sizeof(char));
        }
        cmd_length = index;
        c = cmd[cmd_length - 2];
        divide_addr(ind1, ind2, cmd, cmd_length);
        switch (c) {
            case ('p'):
                print_str(strings, strings_length, addr1, addr2);
                index = 0;
                cmd_length = 0;
                cmd = NULL;
                break;
            case ('d'):
                //delstr();
                printf("delete\n");
                break;
            case ('c'):
                change_str(&strings, addr1, addr2, length, strings_length, &undo, &redo, cmd);
                //printf("change\n");
                index = 0;
                cmd_length = 0;
                cmd = NULL;
                break;
            case ('r'):
                //redo();
                //printf("redo\n");
                break;
            case ('u'):
                //undo();
                printf("undo\n");
                break;
            case ('q'):
                free(cmd);
                for (int i = 0; i < strings_length; ++i) {
                    free(strings[i]);
                }
                free(strings);
                return 0;
            case ('.'):
                //printf("punto\n");
                index = 0;
                cmd_length = 0;
                cmd = NULL;
                break;
        }
    }
}

void divide_addr(int *ind1, int *ind2, char *cmd, int length) {
    int addr1 = 0, addr2 = 0;
    int num = 0;
    int check = 0;
    int i;
    for (i = 0; i < length - 2; i++) {
        if (cmd[i] != ',') {
            check++;
        } else
            break;
    }
    double k = 0;
    for (int j = i - 1; j >= 0; j--) {
        num = cmd[j] - '0';
        addr1 = addr1 + (int) (num * pow(10, k));
        k++;
    }
    k = 0;
    int m;
    for (m = check; m < length - 2; m++) {
        if (cmd[m] == '\n') {
            break;
        }
    }
    for (int n = m - 1; n > check; n--) {
        num = cmd[n] - '0';
        addr2 = addr2 + (int) (num * pow(10, k));
        k++;
    }
    *ind1 = addr1;
    *ind2 = addr2;
}

void print_str(char **strings, int length, int addr1, int addr2) {
    if (strings[0] == NULL) {
        for (int i = addr1; i <= addr2; ++i) {
            printf(".\n");
        }
    } else {
        if (addr1 == 0 && addr2 == 0) {
            printf(".\n");
            return;
        } else if (addr1 == 0) {
            printf(".\n");
            addr1++;
        }
        for (int i = addr1; i <= addr2; ++i) {
            if (i > length) {
                printf(".\n");
            } else {
                printf("%s", strings[i - 1]);
            }
        }
    }
}


void change_str(char ***strings, int addr1, int addr2, int *length, int len, struct saved_commands **undo,
                struct saved_commands **redo, char *cmd) {
    struct saved_commands *undo_node = (struct saved_commands *) malloc(sizeof(struct saved_commands));
    undo_node->next = *undo;
    (*undo) = undo_node;
    undo_node->commands = (struct array *) malloc(sizeof(struct array));
    undo_node->commands->data = cmd;
    undo_node->commands->prev = NULL;
    struct array *first_string_undo = (struct array *) malloc(sizeof(struct array));
    struct array *string_undo = (struct array *) malloc(sizeof(struct array));


    //fill redo structure
    struct saved_commands *redo_node = (struct saved_commands *) malloc(sizeof(struct saved_commands));
    redo_node->next = *redo;
    (*redo) = redo_node;
    redo_node->commands = (struct array *) malloc(sizeof(struct array));
    redo_node->commands->data = cmd;
    redo_node->commands->prev = NULL;
    struct array *first_string_redo = (struct array *) malloc(sizeof(struct array));
    struct array *string_redo = (struct array *) malloc(sizeof(struct array));;
    int index = 0;
    char d;
    for (int i = 0; i < addr2 - addr1 + 1; ++i) {
        index = 0;
        string_redo->next = NULL;
        do {
            d = getchar_unlocked();
            if (index == 0) {
                string_redo->data = (char *) malloc(1025 * sizeof(char));//TODO EFFICIENCY
            }
            string_redo->data[index] = d;
            index++;
        } while (d != '\n');
        string_redo->data = (char *) realloc(string_redo->data, strlen(string_redo->data) * sizeof(char));
        if (i == 0) {
            first_string_redo = string_redo;
        }
        string_redo->next = (struct array *) malloc(sizeof(struct array));;
        string_redo = string_redo->next;
    }
    string_redo->next = NULL;
    redo_node->commands->next = first_string_redo;
    first_string_redo->prev = redo_node->commands;
    struct array *p;
    p = first_string_redo;
    //copy to array and fill

    int k;
    int j = 0;
    for (k = addr1; k <= addr2; ++k) {
        string_undo->next = NULL;
        if (k > len) {
            (*strings)[k - 1] = (char *) malloc(strlen(p->data) * sizeof(char));
            //TODO FILL UNDO
            string_undo->data = (char *) malloc(2 * sizeof(char));
            string_undo->data = ".\n";
            strcpy((*strings)[k - 1], p->data);
        } else {
            (*strings)[k - 1] = (char *) realloc((*strings)[k - 1], strlen(p->data));
            //TODO FILL UNDO
            if (strlen(*strings[k - 1])== 0) {
                string_undo->data = (char *) malloc(2 * sizeof(char));
                string_undo->data = ".\n";
            } else {
                string_undo->data = (char *) malloc(strlen((*strings[k - 1])) * sizeof(char));
                strcpy(string_undo->data, (*strings[k - 1]));
            }
            strcpy((*strings[k - 1]), p->data);
        }
        if (k == addr1) {
            first_string_undo = string_undo;
        }
        string_undo->next = (struct array *) malloc(sizeof(struct array));
        string_undo = string_undo->next;
        p = p->next;
    }
    string_undo->next = NULL;
    undo_node->commands->next = first_string_undo;
    first_string_undo->prev = undo_node->commands;
    if (addr2 > len) {
        *length = addr2;
    }
}