/*
 * htable.c
 *
 *  Created on: Apr 20, 2011
 *      Author: Mattics Phi
 *
 *      http://geeksforgeeks.org/?p=7740 (Code for mergesort linked list)
 *      http://cslibrary.stanford.edu/110/BinaryTrees.html
 */

#include "header.h"

#define TABLESIZE 255

int openFile(int argc, char *argv[]);
void otherMethods(int fp, int argc, char *argv[]);
int makeTable(int fp, int *letterCount);
struct node *createNode(char c);
struct node *createLinkedList();
struct node *buildTree(struct node *head);
struct node *super(struct node *head);
void getPath(struct node *head, int fp, int *letterCount, 
             int fileOpen, int check);
void sortTable(int loop);
struct node *createSuper(struct node *superNode, struct node *left,
                         struct node *right);
struct node *createLeft(struct node *superNode, struct node *left);
struct node *createRight(struct node *superNode, struct node *right);
struct node *placeSuper(struct node *head, struct node *temp,
                        struct node *newHead, struct node *superNode);

struct node {
    int count, path;
    char letter;
    struct node *left, *right, *parent, *next;
};

static struct node **table;

/* ================================================================== */

/*
 * Main
 */
int main(int argc, char *argv[])
{
    int fp;
    
    /* Open the file */
    fp = openFile(argc, argv);
    
    /* Other methods */
    otherMethods(fp, argc, argv);
    
    close(fp);
    
    return 0;
}

/* ================================================================== */

/*
 * Opening file to read
 */
int openFile(int argc, char *argv[])
{
    int fp;
    
    if (argc < 2 || argc > 3) {
        perror("usage: hdecode (infile | -) [ outfile ]");
        exit(3);
    } else if (argc == 2 && strcmp(argv[1], "-") == 0) {
        return STDIN_FILENO;
    } else {
        if ((fp = open(argv[1], O_RDONLY)) < 0) {
            perror(argv[1]);
            exit(3);
        }
    }
    
    return fp;
}

/* ================================================================== */

/*
 * Other methods function
 */
void otherMethods(int fp, int argc, char *argv[])
{
    struct node *head;
    int letterCount = 0, fileOpen, check;
    
    if (argc == 3) {
        check = 1;
        if ((fileOpen = creat(argv[2], 0666)) < 0) {
            perror(argv[2]);
            exit(3);
        }
    } else if (argc == 2) {
        check = 0;
    }

    fp = makeTable(fp, &letterCount);
    head = createLinkedList();
    head = buildTree(head);
    getPath(head, fp, &letterCount, fileOpen, check);
    
    close(fileOpen);
}

/* ================================================================== */

/*
 * Creates the table
 */
int makeTable(int fp, int *letterCount)
{
    int i, j, loop, count;
    char c;
    struct node *newNode;
    
    if ((i = read(fp, &loop, sizeof(int))) != 0);
    
    /* Initializing the table to NULL */
    table = malloc(sizeof(*table) * (loop + 1));
    for (i = 0; i < loop; i++) {
        table[i] = NULL;
    }
    
    for (j = 0; j < loop; j++) {
        if ((i = read(fp, &c, sizeof(char))) != 0) {
            newNode = createNode(c);
        }
        if ((i = read(fp, &count, sizeof(int))) != 0) {
            newNode->count = count;
            *letterCount += count;
        }
        table[j] = newNode;
    }
    
    sortTable(loop);
    
    return fp;
}

/* ================================================================== */

/*
 * Creates a new node to add to table
 */
struct node *createNode(char c)
{
    struct node *newNode = malloc(sizeof(struct node *));
    
    newNode->count = 1;
    newNode->letter = c;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    newNode->next = NULL;
    
    return newNode;
}

/* ================================================================== */

/*
 * Sorts the table based on number of occurances
 */
void sortTable(int loop)
{
    int i, j;
    struct node *temp;
    
    for (i = 0; i < loop && table[i] != NULL; i++) {
        for (j = 0; j < loop - 1 && table[j+1] != NULL; j++) {
            if (table[j]->count > table[j+1]->count) {
                temp = table[j+1];
                table[j+1] = table[j];
                table[j] = temp;
            } else if (table[j]->count == table[j + 1]->count) {
                if ((int)table[j]->letter > (int)table[j + 1]->letter) {
                    temp = table[j + 1];
                    table[j + 1] = table[j];
                    table[j] = temp;
                }
            }
        }
    }
}

/* ================================================================== */

/*
 * Creates the linked list
 */
struct node *createLinkedList()
{
    int i = 0;
    
    while (table[i] != NULL) {
        if (table[i + 1] != NULL) {
            table[i]->next = table[i + 1];
        }
        i++;
    }
    
    return table[0];
}

/* ================================================================== */

/*
 * Builds a tree from the linked list
 */
struct node *buildTree(struct node *head)
{
    while (head->next != NULL) {
        head = super(head);
    }
    
    return head;
}

/* ================================================================== */

/*
 * Combines the first two nodes and puts it back in the list
 */
struct node *super(struct node *head)
{
    struct node *left, *right, *temp, *superNode, *newHead;
    
    left = head;
    right = left->next;
    
    if (right == NULL) {
        return left;
    } else {
        temp = right->next;
        superNode = createSuper(superNode, left, right);
        left = createLeft(superNode, left);
        right = createRight(superNode, right);
        
        if (temp == NULL) {
            return superNode;
        }
        
        /* Put the supernode in front */
        if (superNode->count <= temp->count) {
            superNode->next = temp;
            return superNode;
        }
        
        newHead = temp;
        return placeSuper(head, temp, newHead, superNode);
    }
    return NULL;
}

/* ================================================================== */

/*
 * Creates the supernode
 */
struct node *createSuper(struct node *superNode, struct node *left,
                         struct node *right)
{
    superNode = malloc(sizeof(* superNode));
    superNode->parent = NULL;
    superNode->letter = -1;
    superNode->left = left;
    superNode->right = right;
    superNode->count = left->count + right->count;
    
    return superNode;
}

/* ================================================================== */

/*
 * Creates the left node
 */
struct node *createLeft(struct node *superNode, struct node *left)
{
    left->parent = superNode;
    left->next = NULL;
    left->path = 0;
    
    return left;
}

/* ================================================================== */

/*
 * Creates the rightnode
 */
struct node *createRight(struct node *superNode, struct node *right)
{
    right->parent = superNode;
    right->next = NULL;
    right->path = 1;
    
    return right;
}

/* ================================================================== */

/*
 * Places the supernode within the linked list
 */
struct node *placeSuper(struct node *head, struct node *temp,
                        struct node *newHead, struct node *superNode)
{
    /* Find where supernode belongs and places it in there */
    for (; temp != NULL; temp = temp->next) {
        
        /* Puts it at the end of the list */
        if (temp->next == NULL) {
            temp->next = superNode;
            superNode->next = NULL;
            return newHead;
            
            /* Puts it between nodes */
        } else if (temp->count < superNode->count && 
                   superNode->count <= temp->next->count) {
            superNode->next = temp->next;
            temp->next = superNode;
            return newHead;
        }
    }
    return NULL;
}

/* ================================================================== */

/*
 * Prints the letters to output
 
 */
void getPath(struct node *head, int fp, int *letterCount, 
             int fileOpen, int check)
{
    int count = 0, i;
    char c;
    struct node *travel = head;
    
    while ((i = read(fp, &c, sizeof(char))) != 0) {
        while (count < 8 && *letterCount > 0) {
            if ((c & 128) == 128) {
                travel = travel->right;
                if (travel->left == NULL && travel->right == NULL) {
                    if (check == 0) {
                        printf("%c", travel->letter);
                    } else {
                        write(fileOpen, &travel->letter, sizeof(char));
                    }
                    travel = head;
                    *letterCount = *letterCount - 1;
                }
            } else {
                travel = travel->left;
                if (travel->left == NULL && travel->right == NULL) {
                    if (check == 0) {
                        printf("%c", travel->letter);
                    } else {
                        write(fileOpen, &travel->letter, sizeof(char));
                    }
                    travel = head;
                    *letterCount = *letterCount - 1;                
                }
            }
            count++;
            c = c << 1;
        }
        count = 0;
    }
}







