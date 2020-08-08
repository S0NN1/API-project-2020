#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

typedef struct array
{
    char *data;
    struct array *next;
} array;

void divideind(int ind1, int ind2, char *cmd, int length);

bool getinput(char *inputln){};

void printstr(char **strings){

};

void changestr(){

};

void delstr(){

};

void undo(){

};

void redo(){

};

int main()
{
    int cmdlength = 0;
    int ind1 = 0, ind2 = 0;
    char **strings = NULL;
    char *cmd = NULL;
    char c;
    int index = 0;
    while (true)
    {
        do
        {
            c = getchar_unlocked();
            index++;
            if (cmd == NULL)
            {
                cmd = (char *)malloc(sizeof(char));
                cmd[0] = c;
            }
            else
            {
                cmd = realloc(cmd, (index) * sizeof(char));
                cmd[index - 1] = c;
            }
        } while (c != '\n');
        cmdlength = strlen(cmd);
        c = cmd[cmdlength - 2];
        divideind(ind1, ind2, cmd, cmdlength);
        printf("%d  %d", ind1, ind2);
        switch (c)
        {
        case ('p'):
            printstr(strings);
            index = 0;
            return 0;
        case ('d'):
            delstr();
            printf("delete");
            break;
        case ('c'):
            changestr();
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

void divideind(int ind1, int ind2, char *cmd, int length)
{
    printf("tommy gay");
    int num = 0;
    int check = 0;
    int m;
    int i;
    for (i = 0; i < length - 2; i++)
    {
        if (cmd[i] != ',')
        {
            check++;
        }
        else
            break;
    }
    double k = 0;
    for (int j = i - 1; j >= 0; j--)
    {
        printf("\n%c\n", cmd[j]);
        num = cmd[j] - '0';
        printf("pipo %d pipo\n", num);
        ind1 = ind1 + (int)(num * pow(10, k));
        printf("%d", ind1);
        k++;
    }
    k = 0;
    for (int m = check; m < length - 2; m++)
    {
        if (cmd[m] != '\n')
        {
            m++;
        }
        else
            break;
    }
    for (int n = m - 1; n >= 0; n--)
    {
        num = cmd[n] - '0';
        ind2 = ind2 + (int)(num * pow(10, k));
        k++;
    }
}
