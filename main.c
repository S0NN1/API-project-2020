#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

typedef struct array {
    char *data;
    struct array *prev;
    struct array *next;
} array;

void divideaddr(int *ind1, int *ind2, char *cmd, int length);

void printstr(char **strings, int length, int addr1, int addr2);

void getinput(char **strings, int addr1, int addr2);

void changestr(char **strings, int addr1, int addr2, int *length, int len, array **commands);

void delstr();

void undo();

void redo();


int main() {
    array *commands;
    commands = (array*)malloc(sizeof(array));
    commands->next = NULL;
    commands->prev = NULL;
    commands->data= NULL;
    int addr1 = 0, addr2 = 0;
    int cmdlength = 0;
    int *ind1;
    int *ind2;
    int *length;
    char **strings = (char **) calloc(1, sizeof(char *));
    int stringslength = 1;
    char *cmd = NULL;
    char c;
    int index = 0;
    ind1 = &addr1;
    ind2 = &addr2;
    length = &stringslength;
    while (true) {
        do {
            c = getchar_unlocked();
            if (cmd == NULL) {
                cmd = (char *) malloc(sizeof(char));
                cmd[0] = c;
                index++;
            } else {
                if (index >= cmdlength) {
                    cmd = realloc(cmd, (index + 2) * sizeof(char));
                    cmd[index] = c;
                    index++;
                } else {
                    cmd[index] = c;
                    index++;
                }
            }
        } while (c != '\n');
        if (index <= cmdlength) {
            cmd[index] = '\0';
        }
        cmdlength = strlen(cmd);
        c = cmd[cmdlength - 2];
        divideaddr(ind1, ind2, cmd, cmdlength);
        switch (c) {
            case ('p'):
                printstr(strings, stringslength, addr1, addr2);
                index = 0;
                break;
            case ('d'):
                delstr();
                printf("delete");
                break;
            case ('c'):
                changestr(strings,addr1, addr2,length, stringslength, &commands);
                printf("change");
                break;
            case ('r'):
                redo();
                printf("redo");
                break;
            case ('u'):
                undo();
                printf("undo");
                break;
            case ('q'):
                return 0;
            default:
                printf("default");
                break;
        }
    }
}

void divideaddr(int *ind1, int *ind2, char *cmd, int length) {
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

void printstr(char **strings, int length, int addr1, int addr2) {
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

void changestr(char **strings, int addr1, int addr2, int *length, int len, array **commands) {
    int templen=0;
    char c;
    char **strings1 = (char **) malloc(sizeof(char *));
    int index = 0;
    //receive input and store in a temp strings1
    for (int i = 0; i <= addr2 - addr1 + 1; ++i) {
        index = 0;
        if (i != 0) {
            strings1 = realloc(strings1, (i + 1) * sizeof(char *));
        }
        strings1[i] = (char *) malloc(sizeof(char));
        do {
            c = getchar_unlocked();
            if (index != 0) {
                strings1[i] = realloc(strings1[i], (index + 1) * sizeof(char));
            }
            strings1[i][index] = c;
            index++;
        } while (c != '\n');
    templen++;
    }
    for (int j = 0; j < templen; ++j) {
        printf("%s", strings1[j]);
    }
}