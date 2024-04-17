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
    blockList -> blockID = -1;
    blockList -> startAddress = 0;
    blockList -> endAddress = 0;
    blockList -> next = NULL;
    printf("\nPhysical memory initialized with size %d.\n\n", physicalMemorySize);
}

/********************************************************************/
void printAllocationTable(void) {
    // Declare/Initialize local variables
    blockType *currentBlock = blockList -> next;
    // Print header of table containing fields: ID, Start, End
    printf("\nID\tStart\tEnd\n-------------------\n");
    // Print each block in list with while-loop, loop until end of list, advancing "current block" pointer
    while (currentBlock != NULL) {
        printf("%d\t%d\t\t%d\n", currentBlock -> blockID, currentBlock -> startAddress, currentBlock -> endAddress);
        currentBlock = currentBlock -> next;
    }
    printf("\n");
}

/********************************************************************/
void allocateFirstFit(void) {
    // Declare/Initialize local variables
    int ID, size;
    blockType *currentBlock = blockList, *newBlock;
    // Prompt for block ID & block size
    printf("Enter block ID: ");
    scanf("%d", &ID);
    printf("Enter block size: ");
    scanf("%d", &size);
    // If block size is larger than remaining memory, print message, return
    if (size > remaining) {
        printf("\nNot enough memory to allocate block.\n\n");
        return;
    }
    // If block list is "empty", allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
    if (blockList -> next == NULL) {
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock -> blockID = ID;
        newBlock -> startAddress = 0;
        newBlock -> endAddress = size;
        newBlock -> next = NULL;
        blockList -> next = newBlock;
        remaining -= size;
        printAllocationTable();
        return;
    }
    // Initialize "current block" to compare hole
    while (currentBlock -> next != NULL) {
        // Check for duplicate ID
        if (currentBlock -> next -> blockID == ID) {
            printf("\nBlock ID %d already exists.\n\n", ID);
            return;
        }
        int start = currentBlock -> endAddress;
        int end = (currentBlock -> next == NULL) ? physicalMemorySize : currentBlock -> next -> startAddress;
        // If block fits in hole, allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
        if (end - start >= size) {
            newBlock = (blockType *)malloc(sizeof(blockType));
            newBlock -> blockID = ID;
            newBlock -> startAddress = start;
            newBlock -> endAddress = start + size;
            newBlock -> next = currentBlock -> next;
            currentBlock -> next = newBlock;
            remaining -= size;
            printAllocationTable();
            return;
        }
        // Advance "current block" pointer
        currentBlock = currentBlock -> next;
    }
    // Check last hole
    int lastStart = currentBlock -> endAddress;
    if (physicalMemorySize - lastStart >= size) {
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock -> blockID = ID;
        newBlock -> startAddress = lastStart;
        newBlock -> endAddress = lastStart + size;
        newBlock -> next = NULL;
        currentBlock -> next = newBlock;
        remaining -= size;
        printAllocationTable();
        return;
    }
    // If end of list reached, print message no fitting hole
    printf("\nNo fitting hole found for block.\n\n");
}

/***************************************************************/
void allocateBestFit(void) {
    // Declare/Initialize local variables
    int ID, size, bestSize = physicalMemorySize + 1;
    blockType *bestBlock = NULL, *currentBlock = blockList, *newBlock, *prevBlock = blockList;
    // Prompt for block ID & block size
    printf("Enter block ID: ");
    scanf("%d", &ID);
    printf("Enter block size: ");
    scanf("%d", &size);
    // If block size is larger than remaining memory, print message, return
    if (size > remaining) {
        printf("Not enough memory to allocate block.\n");
        return;
    }
    // If block list is "empty", allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
    if (blockList -> next == NULL) {
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock -> blockID = ID;
        newBlock -> startAddress = 0;
        newBlock -> endAddress = size;
        newBlock -> next = NULL;
        blockList -> next = newBlock;
        remaining -= size;
        printAllocationTable();
        return;
    }
    // Initialize "current block" to compare hole
    while (currentBlock -> next != NULL) {
        // Check for duplicate ID
        if (currentBlock -> next -> blockID == ID) {
            printf("Block ID %d already exists.\n", ID);
            return;
        }
        int start = currentBlock -> endAddress;
        int end = currentBlock -> next -> startAddress;
        int holeSize = end - start;
        // If block fits in hole and hole is smaller than best so far
        if (holeSize >= size && holeSize < bestSize) {
            bestSize = holeSize;
            bestBlock = prevBlock;
        }
        // Advance "current block" pointer
        prevBlock = currentBlock;
        currentBlock = currentBlock -> next;
    }
    // Check last hole
    int lastStart = currentBlock -> endAddress;
    if (physicalMemorySize - lastStart >= size && (physicalMemorySize - lastStart) < bestSize) {
        bestSize = physicalMemorySize - lastStart;
        bestBlock = currentBlock;
    }
    // If no fitting hole found
    if (bestBlock == NULL) {
        printf("No fitting hole found for block.\n");
    } else {
        // Allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock -> blockID = ID;
        newBlock -> startAddress = bestBlock -> endAddress;
        newBlock -> endAddress = bestBlock -> endAddress + size;
        newBlock -> next = bestBlock -> next;
        bestBlock -> next = newBlock;
        remaining -= size;
        printAllocationTable();
    }
}

/***************************************************************/
void allocateWorstFit(void) {
    // Declare/Initialize local variables
    int ID, size, worstSize = 0;
    blockType *worstBlock = NULL, *currentBlock = blockList, *newBlock, *prevBlock = blockList;
    // Prompt for block ID & block size
    printf("Enter block ID: ");
    scanf("%d", &ID);
    printf("Enter block size: ");
    scanf("%d", &size);
    // If block size is larger than remaining memory, print message, return
    if (size > remaining) {
        printf("Not enough memory to allocate block.\n");
        return;
    }
    // If block list is "empty", allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
    if (blockList -> next == NULL) {
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock -> blockID = ID;
        newBlock -> startAddress = 0;
        newBlock -> endAddress = size;
        newBlock -> next = NULL;
        blockList -> next = newBlock;
        remaining -= size;
        printAllocationTable();
        return;
    }
    // Initialize "current block" to compare hole
    while (currentBlock -> next != NULL) {
        // Check for duplicate ID
        if (currentBlock -> next -> blockID == ID) {
            printf("Block ID %d already exists.\n", ID);
            return;
        }
        int start = currentBlock -> endAddress;
        int end = currentBlock -> next -> startAddress;
        int holeSize = end - start;
        // If block fits in hole and hole is larger than worst so far
        if (holeSize >= size && holeSize > worstSize) {
            worstSize = holeSize;
            worstBlock = prevBlock;
        }
        // Advance "current block" pointer
        prevBlock = currentBlock;
        currentBlock = currentBlock -> next;
    }
    // Check last hole
    int lastStart = currentBlock -> endAddress;
    if (physicalMemorySize - lastStart >= size && (physicalMemorySize - lastStart) > worstSize) {
        worstSize = physicalMemorySize - lastStart;
        worstBlock = currentBlock;
    }
    // If no fitting hole found
    if (worstBlock == NULL) {
        printf("No fitting hole found for block.\n");
    } else {
        // Allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table
        newBlock = (blockType *)malloc(sizeof(blockType));
        newBlock -> blockID = ID;
        newBlock -> startAddress = worstBlock -> endAddress;
        newBlock -> endAddress = worstBlock -> endAddress + size;
        newBlock -> next = worstBlock -> next;
        worstBlock -> next = newBlock;
        remaining -= size;
        printAllocationTable();
    }
}

/***************************************************************/
void deallocateBlock(void) {
    // Declare/Initialize local variables
    int ID;
    blockType *currentBlock = blockList, *previousBlock = NULL;
    // Prompt for block ID
    printf("Enter block ID to deallocate: ");
    scanf("%d", &ID);
    // Initialize "current block"
    while (currentBlock != NULL && currentBlock -> blockID != ID) {
        previousBlock = currentBlock;
        currentBlock = currentBlock -> next;
    }
    // If "current block" is NULL, print message id not found
    if (currentBlock == NULL) {
        printf("\nBlock ID %d not found.\n\n", ID);
    } else {
        // Else remove block, adjusting "previous block" pointer around it, increment remaining memory with block size, free block with matching id, print allocation table
        if (previousBlock != NULL) {
            previousBlock -> next = currentBlock -> next;
        }
        int blockSize = currentBlock -> endAddress - currentBlock -> startAddress;
        remaining += blockSize;
        free(currentBlock);
        printAllocationTable();
    }
}

/********************************************************************/
void defragmentMemory(void) {
    // Declare/Initialize local variables
    blockType *currentBlock = blockList->next;
    int currentStart = 0; // Start address for the first block after defragmentation

    // While end of block list not reached
    while (currentBlock != NULL) {
        // Calculate the actual size of the current block
        int blockSize = currentBlock->endAddress - currentBlock->startAddress;
        // Adjust start field of the current block
        currentBlock->startAddress = currentStart;
        // Set the new end address based on the new start address and the block size
        currentBlock->endAddress = currentBlock->startAddress + blockSize;
        // Update currentStart for the next block
        currentStart += blockSize;
        // Move to the next block
        currentBlock = currentBlock->next;
    }
    // Print allocation table to show the new compacted layout
    printAllocationTable();
}

/********************************************************************/
void freeMemory(blockType *node) {
    // If node is NULL return
    if (node == NULL) return;
    // Else call self on link field of node, free node
    freeMemory(node -> next);
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
            case 3:
                allocateBestFit();
                break;
            case 4:
                allocateWorstFit();
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
