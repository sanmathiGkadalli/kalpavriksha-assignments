#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

struct FreeBlock
{
    int block;
    struct FreeBlock *next, *prev;
};

struct FileNode
{
    char name[50];
    int isDir, size, count;
    int blocks[NUM_BLOCKS];

    struct FileNode *parent, *child;
    struct FileNode *next, *prev;
};

struct FreeBlock *freeHead = NULL, *freeTail = NULL;
struct FileNode *root, *cwd;
int usedBlocks = 0;

// Add free block at tail
void addBlock(int block)
{
    struct FreeBlock *newBlock = malloc(sizeof(struct FreeBlock));

    newBlock->block = block;
    newBlock->next = NULL;
    newBlock->prev = freeTail;

    if(freeHead == NULL)
    {
        freeHead = newBlock;
    }
    else
    {
        freeTail->next = newBlock;
    }
    freeTail = newBlock;
}

// Get free block from head
int getBlock()
{
    if(freeHead == NULL)
    {
        return -1;
    }

    struct FreeBlock *temp = freeHead;
    int block = temp->block;

    freeHead = temp->next;

    if(freeHead == NULL)
    {
        freeTail = NULL;
    }
    else
    {
        freeHead->prev = NULL;
    }

    free(temp);
    return block;
}

// Count free blocks
int freeBlocks()
{
    int count = 0;
    struct FreeBlock *temp = freeHead;

    while(temp != NULL)
    {
        count++;
        temp = temp->next;
    }

    return count;
}

// Initialize disk and root
void initialize()
{
    for(int i = 0; i < NUM_BLOCKS; i++)
    {
        addBlock(i);
    }

    root = calloc(1, sizeof(struct FileNode));
    strcpy(root->name, "/");
    root->isDir = 1;
    cwd = root;
}

// Find a file or directory
struct FileNode *findNode(char *name)
{
    if(cwd->child == NULL)
    {
        return NULL;
    }

    struct FileNode *temp = cwd->child;

    do
    {
        if(strcmp(temp->name, name) == 0)
        {
            return temp;
        }

        temp = temp->next;
    } while(temp != cwd->child);

    return NULL;
}

// Insert node into circular doubly linked list
void insertNode(struct FileNode *node)
{
    node->parent = cwd;

    if(cwd->child == NULL)
    {
        cwd->child = node;
        node->next = node->prev = node;
    }
    else
    {
        struct FileNode *last = cwd->child->prev;

        last->next = node;
        node->prev = last;
        node->next = cwd->child;
        cwd->child->prev = node;
    }
}

// Remove node from circular list
void removeNode(struct FileNode *node)
{
    if(node->next == node)
    {
        cwd->child = NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if(cwd->child == node)
        {
            cwd->child = node->next;
        }
    }
}

// Create file or directory
void createNode(char *name, int isDir)
{
    if(findNode(name))
    {
        printf("Name already exists.\n");
        return;
    }

    struct FileNode *node = calloc(1, sizeof(struct FileNode));
    strcpy(node->name, name);
    node->isDir = isDir;

    insertNode(node);

    if(isDir)
    {
        printf("Directory '%s' created successfully.\n", name);
    }
    else
    {
        printf("File '%s' created successfully.\n", name);
    }
}

// Write data into file
void writeFile(char *name, char *content)
{
    struct FileNode *file = findNode(name);

    if(file == NULL || file->isDir)
    {
        printf("File not found.\n");
        return;
    }

    int size = strlen(content);
    int required = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if(required > freeBlocks() + file->count)
    {
        printf("Disk is full.\n");
        return;
    }

    // Return old blocks
    for(int i = 0; i < file->count; i++)
    {
        addBlock(file->blocks[i]);
    }

    usedBlocks -= file->count;
    file->count = 0;

    // Allocate new blocks and store content
    for(int i = 0; i < required; i++)
    {
        int block = getBlock();
        int start = i * BLOCK_SIZE;
        int bytes = size - start;

        if(bytes > BLOCK_SIZE)
        {
            bytes = BLOCK_SIZE;
        }

        file->blocks[i] = block;
        memcpy(virtualDisk[block], content + start, bytes);

        file->count++;
        usedBlocks++;
    }

    file->size = size;
    printf("Data written successfully (size=%d bytes).\n", size);
}

// Read file content
void readFile(char *name)
{
    struct FileNode *file = findNode(name);

    if(file == NULL || file->isDir)
    {
        printf("File not found.\n");
        return;
    }

    for(int i = 0; i < file->count; i++)
    {
        int bytes = file->size - i * BLOCK_SIZE;

        if(bytes > BLOCK_SIZE)
        {
            bytes = BLOCK_SIZE;
        }

        fwrite(virtualDisk[file->blocks[i]], 1, bytes, stdout);
    }

    printf("\n");
}

// Delete file
void deleteFile(char *name)
{
    struct FileNode *file = findNode(name);

    if(file == NULL || file->isDir)
    {
        printf("File not found.\n");
        return;
    }

    for(int i = 0; i < file->count; i++)
    {
        addBlock(file->blocks[i]);
    }

    usedBlocks -= file->count;
    removeNode(file);
    free(file);

    printf("File deleted successfully.\n");
}

// Remove empty directory
void removeDir(char *name)
{
    struct FileNode *dir = findNode(name);

    if(dir == NULL || !dir->isDir)
    {
        printf("Directory not found.\n");
        return;
    }

    if(dir->child != NULL)
    {
        printf("Directory is not empty.\n");
        return;
    }

    removeNode(dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

// List directory contents
void listFiles()
{
    if(cwd->child == NULL)
    {
        printf("(empty)\n");
        return;
    }

    struct FileNode *temp = cwd->child;

    do
    {
        printf("%s%s\n", temp->name, temp->isDir ? "/" : "");
        temp = temp->next;
    } while(temp != cwd->child);
}

// Print current path
void pwd()
{
    char path[1000] = "";
    struct FileNode *temp = cwd;

    while(temp != root) 
    {
        char part[60];
        sprintf(part, "/%s%s", temp->name, path);
        strcpy(path, part);
        temp = temp->parent;
    }

    printf("%s\n", strlen(path) ? path : "/");
}

// Change directory
void cd(char *name)
{
    if(strcmp(name, "..") == 0)
    {
        if(cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }
    }
    else
    {
        struct FileNode *dir = findNode(name);

        if(dir == NULL || !dir->isDir)
        {
            printf("Directory not found.\n");
            return;
        }

        cwd = dir;
    }

    printf("Moved to ");
    pwd();
}

// Display disk usage
void df()
{
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlocks());
    printf("Disk Usage: %.2f%%\n", (float)usedBlocks / NUM_BLOCKS * 100);
}

// Free all directory and file nodes
void freeDirectory(struct FileNode *dir)
{
    if (dir->child == NULL)
    {
        return;
    }

    struct FileNode *temp = dir->child;

    do
    {
        struct FileNode *next = temp->next;

        if(temp->isDir)
        {
            freeDirectory(temp);
        }
        else
        {
            for(int i = 0; i < temp->count; i++)
            {
                addBlock(temp->blocks[i]);
            }
        }

        free(temp);
        temp = next;
    } while(temp != dir->child);

    dir->child = NULL;
}

// Exit and release memory
void exitVFS()
{
    freeDirectory(root);
    free(root);

    while(freeHead != NULL)
    {
        struct FreeBlock *temp = freeHead;
        freeHead = freeHead->next;
        free(temp);
    }

    printf("Memory released. Exiting program...\n");
}

int main()
{
    char command[20], name[51], content[10000];

    initialize();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while(1)
    {
        printf("%s > ", cwd == root ? "/" : cwd->name);

        if(scanf("%19s", command) != 1)
        {
            break;
        }

        if(strcmp(command, "mkdir") == 0 || strcmp(command, "create") == 0)
        {
            scanf("%50s", name);
            createNode(name, strcmp(command, "mkdir") == 0);
        }
        else if(strcmp(command, "write") == 0)
        {
            scanf("%50s", name);
            getchar();
            fgets(content, sizeof(content), stdin);

            content[strcspn(content, "\n")] = '\0';

            int len = strlen(content);

            if(len >= 2 && content[0] == '"' && content[len - 1] == '"')
            {
                memmove(content, content + 1, len - 2);
                content[len - 2] = '\0';
            }

            writeFile(name, content);
        }
        else if(strcmp(command, "read") == 0)
        {
            scanf("%50s", name);
            readFile(name);
        }
        else if(strcmp(command, "delete") == 0)
        {
            scanf("%50s", name);
            deleteFile(name);
        }
        else if(strcmp(command, "rmdir") == 0)
        {
            scanf("%50s", name);
            removeDir(name);
        }
        else if(strcmp(command, "cd") == 0)
        {
            scanf("%50s", name);
            cd(name);
        }
        else if(strcmp(command, "ls") == 0)
        {
            listFiles();
        }
        else if(strcmp(command, "pwd") == 0)
        {
            pwd();
        }
        else if(strcmp(command, "df") == 0)
        {
            df();
        }
        else if(strcmp(command, "exit") == 0)
        {
            exitVFS();
            break;
        }
        else
        {
            printf("Invalid command.\n");
        }
    }

    return 0;
}