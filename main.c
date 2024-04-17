//
//  main.c
//  Hole-Fitting-Algorithms
//
//  Created by Behrouz Barati on 4/16/24.
//

#include <stdio.h>
#include <stdlib.h>

// Declare global variables including linked list of blocks containing id, starting address, ending address, link to next node in list
#define MENU_OF_OPTIONS "Hole-fitting Algorithms\n-----------------\n1) Enter parameters\n2) Allocate memory for block using First-fit\n3) Allocate memory for block using Best-fit\n4) Allocate memory for block using Worst-fit\n5) Deallocate memory for block\n6) Defragment memory\n7) Quit program\n\n"

struct block {
    int blockID;
    int startAddress;
    int endAddress;
    struct block *next;
}*blockList = NULL;
typedef struct block blockType;

int physicalMemorySize;
int remaining;

//********************************************************************
void initializePhysicalMemory(void) {
    // Declare/Initialize local variables
    int size;
    // Prompt for size of physical memory
    printf("Enter size of physical memory: ");
    scanf("%d", &size);
    // Initialize remaining memory to size of physical memory
    physicalMemorySize = size;
    remaining = size;
    // Allocate memory for linked list of blocks
    blockList = (blockType *)malloc(sizeof(blockType));
    // Create "dummy" block with id -1, starting/ending address of 0 and link of NULL
    blockList->blockID = -1;
    blockList->startAddress = 0;
    blockList->endAddress = 0;
    blockList->next = NULL;
    printf("\nPhysical memory initialized with size %d.\n\n", physicalMemorySize);
}

/********************************************************************/
void printAllocationTable(void) {
    // Declare/Initialize local variables
    blockType *currentBlock = blockList->next;
    // Print header of table containing fields: ID, Start, End
    printf("ID\tStart\tEnd\n-------------------\n");
    // Print each block in list with while-loop, loop until end of list, advancing "current block" pointer
    while (currentBlock != NULL) {
        printf("%d\t%d\t%d\n", currentBlock->blockID, currentBlock->startAddress, currentBlock->endAddress);
        currentBlock = currentBlock->next;
    }
}

/********************************************************************/
void allocateFirstFit(void) {
    // Declare/Initialize local variables
    int id, size;
    blockType *currentBlock = blockList, *newBlock;
    // Prompt for block id & block size
    printf("Enter block id: ");
    scanf("%d", &id);
    printf("Enter block size: ");
    scanf("%d", &size);
    // If block size is larger than remaining memory, print message, return
    if (size > remaining) {
        printf("Not enough memory to allocate block.\n");
        return;
    }
    // If block list is "empty", allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
    if (blockList->next == NULL) {
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock->blockID = id;
        newBlock->startAddress = 0;
        newBlock->endAddress = size;
        newBlock->next = NULL;
        blockList->next = newBlock;
        remaining -= size;
        printAllocationTable();
        return;
    }
    // Initialize "current block" to compare hole
    while (currentBlock->next != NULL) {
        // Check for duplicate id
        if (currentBlock->next->blockID == id) {
            printf("Block ID %d already exists.\n", id);
            return;
        }
        int start = currentBlock->endAddress;
        int end = (currentBlock->next == NULL) ? physicalMemorySize : currentBlock->next->startAddress;
        // If block fits in hole, allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
        if (end - start >= size) {
            newBlock = (blockType *)malloc(sizeof(blockType));
            newBlock->blockID = id;
            newBlock->startAddress = start;
            newBlock->endAddress = start + size;
            newBlock->next = currentBlock->next;
            currentBlock->next = newBlock;
            remaining -= size;
            printAllocationTable();
            return;
        }
        // Advance "current block" pointer
        currentBlock = currentBlock->next;
    }
    // Check last hole
    int lastStart = currentBlock->endAddress;
    if (physicalMemorySize - lastStart >= size) {
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock->blockID = id;
        newBlock->startAddress = lastStart;
        newBlock->endAddress = lastStart + size;
        newBlock->next = NULL;
        currentBlock->next = newBlock;
        remaining -= size;
        printAllocationTable();
        return;
    }
    // If end of list reached, print message no fitting hole
    printf("No fitting hole found for block.\n");
}

/***************************************************************/
void deallocateBlock(void) {
    // Declare/Initialize local variables
    int id;
    blockType *currentBlock = blockList, *previousBlock = NULL;
    // Prompt for block id
    printf("Enter block id to deallocate: ");
    scanf("%d", &id);
    // Initialize "current block"
    while (currentBlock != NULL && currentBlock->blockID != id) {
        previousBlock = currentBlock;
        currentBlock = currentBlock->next;
    }
    // If "current block" is NULL, print message id not found
    if (currentBlock == NULL) {
        printf("Block ID %d not found.\n", id);
    } else {
        // Else remove block, adjusting "previous block" pointer around it, increment remaining memory with block size, free block with matching id, print allocation table
        if (previousBlock != NULL) {
            previousBlock->next = currentBlock->next;
        }
        int blockSize = currentBlock->endAddress - currentBlock->startAddress;
        remaining += blockSize;
        free(currentBlock);
        printAllocationTable();
    }
}

/********************************************************************/
void defragmentMemory(void) {
    // Declare/Initialize local variables
    blockType *currentBlock = blockList->next;
    int currentStart = 0;
    // Initialize "current block"
    while (currentBlock != NULL) {
        // Adjust start and end fields of each block, compacting together
        currentBlock->startAddress = currentStart;
        currentStart += currentBlock->endAddress - currentBlock->startAddress;
        currentBlock->endAddress = currentStart;
        currentBlock = currentBlock->next;
    }
    // Print allocation table
    printAllocationTable();
}

/********************************************************************/
void freeMemory(blockType *node) {
    // If node is NULL return
    if (node == NULL) return;
    // Else call self on link field of node, free node
    freeMemory(node->next);
    free(node);
}

//*************************************************************
int main(void) {
    // Declare local vars
    int selection;
    // While user has not chosen to quit
    do {
        // Print menu of options
        printf(MENU_OF_OPTIONS);
        // Prompt for menu selection
        printf("Enter selection: ");
        scanf("%d", &selection);
        // Call appropriate procedure based on choice--use switch statement or series of if, else if, else statements
        switch (selection) {
            case 1:
                initializePhysicalMemory();
                break;
            case 2:
                allocateFirstFit();
                break;
            case 5:
                deallocateBlock();
                break;
            case 6:
                defragmentMemory();
                break;
            case 7:
                printf("Quitting program...\n");
                freeMemory(blockList);
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
    } while (selection != 7); // While loop
    return 0; // Indicates success
} // End of procedure
