#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>


int main(int argc, char **argv){
    struct node{
        int num;
        struct node *ptr;
    };

    typedef struct node NODE;


    NODE *head, *first, *temp = 0;
    int count = 0;
    int choice = 1;
    first = 0;

    while (choice){
        head  = (NODE *)malloc(sizeof(NODE));
        printf("\nEnter an integer value\n");
        scanf("%d", &head-> num);

        if (first != 0){
            temp->ptr = head;
            temp = head;
        }
        else{
            first = temp = head;
        }

        printf("\n0 to stop. 1 to continue?\n");
        scanf("%d", &choice);
    }

    temp->ptr = 0;
    temp = first;

    while (temp != 0){
        printf("%d=>", temp->num);
        count++;
        temp = temp -> ptr;
    }

    printf("NULL\n");
    printf("Total nodes: %d\n", count);

}
