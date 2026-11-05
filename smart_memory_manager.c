#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct Block
{
    int start;
    int size;
    int id;
    int free;
};

struct Block *blocks;
char *memory;

int count = 1, capacity = 2, totalSize, nextID = 1, strategy = 1;

// Increase metadata array size
int increaseCapacity()
{
    if(count < capacity)
    {
        return 1;
    }

    capacity = capacity * 2;

    struct Block *temp = realloc(blocks, capacity * sizeof(struct Block));

    if(temp == NULL)
    {
        capacity = capacity / 2;
        return 0;
    }

    blocks = temp;
    return 1;
}

// Find a free block using selected strategy
int findBlock(int size)
{
    int best = -1;

    for(int i = 0; i < count; i++)
    {
        if(blocks[i].free && blocks[i].size >= size)
        {
            if(strategy == 1)
            {
                return i;
            }

            if(best == -1)
            {
                best = i;
            }
            else if(strategy == 2 && blocks[i].size < blocks[best].size)
            {
                best = i;
            }
            else if(strategy == 3 && blocks[i].size > blocks[best].size)
            {
                best = i;
            }
        }
    }

    return best;
}

// Merge adjacent free blocks
void mergeBlocks()
{
    int i = 0;

    while(i < count - 1)
    {
        if(blocks[i].free && blocks[i + 1].free)
        {
            blocks[i].size += blocks[i + 1].size;

            for(int j = i + 1; j < count - 1; j++)
            {
                blocks[j] = blocks[j + 1];
            }

            count--;
        }
        else
        {
            i++;
        }
    }
}

// Allocate memory
void allocateMemory()
{
    int size;

    printf("Enter size to allocate: ");

    if(scanf("%d", &size) != 1)
    {
        printf("Invalid input!\n");
        return;
    }

    if(size <= 0 || size > totalSize)
    {
        printf("Invalid size!\n");
        return;
    }

    int pos = findBlock(size);

    if(pos == -1)
    {
        int freeMemory = 0;

        for(int i = 0; i < count; i++)
        {
            if(blocks[i].free)
            {
                freeMemory += blocks[i].size;
            }
        }

        printf("Allocation failed!\n");

        if(freeMemory >= size)
		{
			printf("Allocation failed: no contiguous free block is large enough.\n");
		}
        else
        {
            printf("Not enough free memory.\n");
        }

        return;
    }

    if(nextID <= 0 || nextID == INT_MAX)
    {
        printf("Cannot generate more allocation IDs.\n");
        return;
    }

    int id = nextID++;

    if(blocks[pos].size > size)
    {
        if(!increaseCapacity())
        {
            printf("Memory allocation failed!\n");
            return;
        }

        for(int i = count; i > pos + 1; i--)
        {
            blocks[i] = blocks[i - 1];
        }

        blocks[pos + 1].start = blocks[pos].start + size;
        blocks[pos + 1].size = blocks[pos].size - size;
        blocks[pos + 1].id = 0;
        blocks[pos + 1].free = 1;

        blocks[pos].size = size;
        count++;
    }

    blocks[pos].free = 0;
    blocks[pos].id = id;

    printf("Allocation successful!\n");
    printf("ID: %d, Start: %d, Size: %d\n", id, blocks[pos].start, size);
}

// Release memory
void releaseMemory()
{
    int id, found = 0;

    printf("Enter allocation ID: ");

    if(scanf("%d", &id) != 1)
    {
        printf("Invalid input!\n");
        return;
    }

    for(int i = 0; i < count; i++)
    {
        if(!blocks[i].free && blocks[i].id == id)
        {
            blocks[i].free = 1;
            blocks[i].id = 0;
            found = 1;
            break;
        }
    }

    if(found)
    {
        mergeBlocks();
        printf("Memory released successfully!\n");
    }
    else
    {
        printf("Invalid allocation ID!\n");
    }
}

// Resize memory
void resizeMemory()
{
    int id, newSize, pos = -1;

    printf("Enter allocation ID: ");

    if(scanf("%d", &id) != 1)
    {
        printf("Invalid input!\n");
        return;
    }

    printf("Enter new size: ");

    if(scanf("%d", &newSize) != 1)
    {
        printf("Invalid input!\n");
        return;
    }

    if(newSize <= 0 || newSize > totalSize)
    {
        printf("Invalid size!\n");
        return;
    }

    for(int i = 0; i < count; i++)
    {
        if(!blocks[i].free && blocks[i].id == id)
        {
            pos = i;
            break;
        }
    }

    if(pos == -1)
    {
        printf("Allocation ID not found!\n");
        return;
    }

    int oldSize = blocks[pos].size;

    // If new size is smaller
    if(newSize < oldSize)
    {
        if(!increaseCapacity())
        {
            printf("Resize failed!\n");
            return;
        }

        for(int i = count; i > pos + 1; i--)
        {
            blocks[i] = blocks[i - 1];
        }

        blocks[pos + 1].start = blocks[pos].start + newSize;
        blocks[pos + 1].size = oldSize - newSize;
        blocks[pos + 1].id = 0;
        blocks[pos + 1].free = 1;

        blocks[pos].size = newSize;
        count++;

        mergeBlocks();

        printf("Memory resized successfully!\n");
        return;
    }

    // If size is same
    if(newSize == oldSize)
    {
        printf("Size is already the same.\n");
        return;
    }

    // Try expanding into next free block
    if(pos + 1 < count && blocks[pos + 1].free && blocks[pos + 1].size >= newSize - oldSize)
    {
        int extra = newSize - oldSize;

        if(blocks[pos + 1].size == extra)
        {
            blocks[pos].size = newSize;

            for(int i = pos + 1; i < count - 1; i++)
            {
                blocks[i] = blocks[i + 1];
            }

            count--;
        }
        else
        {
            blocks[pos + 1].start += extra;
            blocks[pos + 1].size -= extra;
            blocks[pos].size = newSize;
        }

        printf("Memory resized successfully!\n");
        return;
    }

    // If not possible, move allocation to another free block
    int newPos = findBlock(newSize);

    if(newPos == -1)
    {
        printf("Resize failed! Not enough contiguous memory.\n");
        return;
    }

    int oldStart = blocks[pos].start;

    // Save old data temporarily
    char *temp = malloc(oldSize);

    if(temp == NULL)
    {
        printf("Resize failed!\n");
        return;
    }

    memcpy(temp, memory + oldStart, oldSize);

    // Allocate new block first
    if(blocks[newPos].size > newSize)
    {
        if(!increaseCapacity())
        {
            free(temp);
            printf("Resize failed!\n");
            return;
        }

        for(int i = count; i > newPos + 1; i--)
        {
            blocks[i] = blocks[i - 1];
        }

        blocks[newPos + 1].start = blocks[newPos].start + newSize;
        blocks[newPos + 1].size = blocks[newPos].size - newSize;
        blocks[newPos + 1].id = 0;
        blocks[newPos + 1].free = 1;

        blocks[newPos].size = newSize;
        count++;

        if(newPos < pos)
        {
            pos++;
        }
    }

    int newStart = blocks[newPos].start;

    memcpy(memory + newStart, temp, oldSize);

    // Release old block
    blocks[pos].free = 1;
    blocks[pos].id = 0;

    // Set new allocation
    blocks[newPos].free = 0;
    blocks[newPos].id = id;

    free(temp);
    mergeBlocks();

    printf("Memory resized successfully!\n");
}

// Display memory map
void displayMap()
{
    printf("\nMemory Map (%d bytes)\n", totalSize);

    for(int i = 0; i < count; i++)
    {
        if(blocks[i].free)
        {
            printf("[Free: %d bytes, Start: %d]\n", blocks[i].size, blocks[i].start);
        }
        else
        {
            printf("[Allocated #%d: %d bytes, Start: %d]\n", blocks[i].id, blocks[i].size, blocks[i].start);
        }
    }
}

// Display statistics
void displayStatistics()
{
    int allocated = 0, freeMemory = 0, allocatedBlocks = 0, freeBlocks = 0, largest = 0;

    for(int i = 0; i < count; i++)
    {
        if(blocks[i].free)
        {
            freeMemory += blocks[i].size;
            freeBlocks++;

            if(blocks[i].size > largest)
            {
                largest = blocks[i].size;
            }
        }
        else
        {
            allocated += blocks[i].size;
            allocatedBlocks++;
        }
    }

    printf("\nTotal Memory: %d bytes\n", totalSize);
    printf("Allocated Memory: %d bytes\n", allocated);
    printf("Free Memory: %d bytes\n", freeMemory);
    printf("Allocated Blocks: %d\n", allocatedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Largest Free Block: %d bytes\n", largest);
}

// Change allocation strategy
void changeStrategy()
{
    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Choose strategy: ");

    int choice;

    if(scanf("%d", &choice) != 1)
    {
        printf("Invalid input!\n");
        while(getchar() != '\n');
        return;
    }

    if(choice >= 1 && choice <= 3)
    {
        strategy = choice;

        if(choice == 1)
        {
            printf("First Fit selected.\n");
        }
        else if(choice == 2)
        {
            printf("Best Fit selected.\n");
        }
        else
        {
            printf("Worst Fit selected.\n");
        }
    }
    else
    {
        printf("Invalid choice!\n");
    }
}

// Main function
int main()
{
    printf("Enter memory pool size (1 to 1000000): ");

    if(scanf("%d", &totalSize) != 1)
    {
        printf("Invalid input!\n");
        return 1;
    }

    if(totalSize < 1 || totalSize > 1000000)
    {
        printf("Invalid pool size!\n");
        return 0;
    }

    memory = malloc(totalSize);

    if(memory == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    blocks = malloc(capacity * sizeof(struct Block));

    if(blocks == NULL)
    {
        printf("Memory allocation failed!\n");
        free(memory);
        return 1;
    }

    // Initially entire memory is free
    blocks[0].start = 0;
    blocks[0].size = totalSize;
    blocks[0].id = 0;
    blocks[0].free = 1;

    printf("\nSelect allocation strategy:\n");
    changeStrategy();

    int choice;

    do
    {
        printf("\nMenu:\n");
        printf("1. Allocate Memory\n");
        printf("2. Release Memory\n");
        printf("3. Resize Memory\n");
        printf("4. Display Memory Map\n");
        printf("5. Display Memory Statistics\n");
        printf("6. Change Strategy\n");
        printf("7. Exit\n");

        printf("Enter choice: ");

        if(scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            choice = 0;
            continue;
        }

        switch(choice)
        {
            case 1:
                allocateMemory();
                break;

            case 2:
                releaseMemory();
                break;

            case 3:
                resizeMemory();
                break;

            case 4:
                displayMap();
                break;

            case 5:
                displayStatistics();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                printf("Exiting program...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while(choice != 7);

    free(memory);
    free(blocks);

    return 0;
}