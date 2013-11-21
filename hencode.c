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
void addToTable(char c);
struct node *createNode(char c);
void otherMethods(int argc, char *argv[], int fp);
int sortTable();
struct node *makeLinkedList();
struct node *buildTree(struct node *head);
struct node *super(struct node *head);
void reBuildTable();
int printTable(struct node *head, int argc, char *argv[], int count);
struct node *searchNode(char c, struct node *head);
void filePath(int fp, int newFile, struct node *head);
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
    int fp, i;
    char c;
   
    /* Initializing the table to NULL */
    table = malloc(sizeof(*table) * (TABLESIZE + 1));
    for (i = 0; i < TABLESIZE; i++) {
        table[i] = NULL;
    }

    /* Open the file */
    fp = openFile(argc, argv);
   
    /* Scan in characters until end of file */
    while ((i = read(fp, &c, sizeof(char))) != 0) {
        addToTable(c);
    }
   
    /* Other methods */
    otherMethods(argc, argv, fp);
    
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
    struct stat stat_record;
   
    if (argc < 2 || argc > 3) {
        perror("usage: hencode infile [ outfile ]");
        exit(3);
    } else {
        if (stat(argv[1], &stat_record)) {
            perror("argv[1]");
            exit(3);
        } else if (!stat_record.st_size) {
            perror("argv[1]");
            exit(3);
        }
        if ((fp = open(argv[1], O_RDONLY)) < 0) {
            perror(argv[1]);
            exit(3);
        }
    }
    
    return fp;
}

/* ================================================================== */

/*
 * Add character to table
 */
void addToTable(char c)
{
    if ((table[(int)c]) == NULL ) {
        table[(int)c] = createNode(c);
    } else {
        table[(int)c]->count++;
    }
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
 * Other main function
 */
void otherMethods(int argc, char *argv[], int fp)
{
    struct node *head;
    int count, file;
   
    count = sortTable();
    head = makeLinkedList();
    head = buildTree(head);
    reBuildTable();
    file = printTable(head, argc, argv, count);
    filePath(fp, file, head);
}

/* ================================================================== */

/*
 * Sorts the table based on number of occurances
 */
int sortTable()
{
    int counter = 0, i, j;
    struct node *temp;

    for (i = 0; i < TABLESIZE; i++) {
        if (table[i] != NULL) {
            table[counter] = table[i];
            table[i] = NULL;
            counter++;
        }
    }

    for (i = 0; i < TABLESIZE && table[i] != NULL; i++) {
        for (j = 0; j < TABLESIZE - 1 && table[j+1] != NULL; j++) {
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
    
    return counter;
}

/* ================================================================== */

/*
 * Makes a linked list from the table
 */
struct node *makeLinkedList()
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
 * Sorts the table by ascii value
 */
void reBuildTable()
{
    int i, j;
    struct node *temp;
   
    for (i = 0; i < TABLESIZE && table[i] != NULL; i++) {
        for (j = 0; j < TABLESIZE - 1 && table[j+1] != NULL; j++) {
            if (table[j]->letter > table[j+1]->letter) {
                temp = table[j+1];
                table[j+1] = table[j];
                table[j] = temp;
            }
        }
    }
}

/* ================================================================== */

/*
 * Prints the table based on ascii value
 */
int printTable(struct node *head, int argc, char *argv[], int count)
{
    int i = 0, newFile;
    
    if (argc == 2) {
        newFile = STDIN_FILENO;
    } else if (argc == 3) {
        if ((newFile = creat(argv[2], 0666)) < 0) {
            perror(argv[2]);
            exit(3);
        }
    }
    
    write(newFile, &count, sizeof(int));
    while (table[i] != NULL) {
        write(newFile, &table[i]->letter, sizeof(char));
        write(newFile, &table[i]->count, sizeof(int));
        i++;
    }
    
    return newFile;
}

/* ================================================================== */

/*
 * Searches the tree for node to be printed by using recursion
 */
struct node *searchNode(char c, struct node *head)
{
    struct node *left, *right;
   
    if (head == NULL) {
        return NULL;
    }
   
    if (head->letter == c) {
        return head;
    }
   
    left = searchNode(c, head->left);
    right = searchNode(c, head->right);
   
    if (left != NULL) {
        if (left->letter == c) {
            return left;
        }
    } else if (right != NULL) {
        if (right->letter == c) {
            return right;
        }
    }
   
    return NULL;
}

/* ================================================================== */

/*
 * Prints the path to the file
 */

void filePath(int fp, int newFile, struct node *head)
{
    int i, n = 0, temp[100], temp1[100], j, count = 0;
    char c, send = 0;
    struct node *nodeFound;
    
    if (lseek(fp, 0, SEEK_SET) < 0) {
        perror("lseek");
        exit(3);
    }
    
    while ((i = read(fp, &c, sizeof(char))) != 0) {
        nodeFound = searchNode(c, head);
        for (; nodeFound->parent != NULL; nodeFound = nodeFound->parent) {
            temp[n++] = nodeFound->path;             
        }
        for (j = 0, n--; n >= 0; j++, n--) {
            temp1[j] = temp[n];
            if (temp1[j] == 0) {
                send = send << 1;
            } else {
                send = send << 1;
                send = send | 1;
            }
            count++;
            if (count == 8) {
                write(newFile, &send, sizeof(char));
                count = 0;
                send = 0;
            }
        }
        n = 0;
    }
    if (n < 8) {
        for (; n < 8; n++) {
            send = send << 1;
        }
    }
    write(newFile, &send, sizeof(char));
    
    close(newFile);
}




