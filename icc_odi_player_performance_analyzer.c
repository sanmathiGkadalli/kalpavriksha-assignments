#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"
#define MAX 10

// Player structure
typedef struct
{
    int id;
    char name[51];
    char role[20];
    int runs;
    float avg, sr;
    int wickets;
    float economy, performance;
} ODIPlayer;

typedef struct PlayerNode
{
    ODIPlayer p;
    struct PlayerNode *next;
    struct PlayerNode *roleNext;
} PlayerNode;

typedef struct
{
    int id;
    char name[30];
    int count;
    PlayerNode *head;
    PlayerNode *roleHead[3];
} Team;

Team team[MAX];

int getRole(char role[])
{
    if(strcmp(role, "Batsman") == 0)
    {
        return 0;
    }
    if(strcmp(role, "Bowler") == 0)
    {
        return 1;
    }
    return 2;
}

char *roleName(int r)
{
    if(r == 0)
    {
        return "Batsman";
    }
    if(r == 1)
    {
        return "Bowler";
    }
    return "All-rounder";
}

// Calculate performance index
float calculatePerformance(ODIPlayer p)
{
    if(strcmp(p.role, "Batsman") == 0)
    {
        return (p.avg * p.sr) / 100;
    }
    if(strcmp(p.role, "Bowler") == 0)
    {
        return (p.wickets * 2) + (100 - p.economy);
    }
    return (p.avg * p.sr) / 100 + (p.wickets * 2);
}

// Insert player into team and sorted role list
void addNode(int t, ODIPlayer p)
{
    PlayerNode *newNode = malloc(sizeof(PlayerNode));
    if(newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    newNode->p = p;
    newNode->next = team[t].head;
    team[t].head = newNode;
    int r = getRole(p.role);
    PlayerNode **current = &team[t].roleHead[r];
    while(*current != NULL && (*current)->p.performance >= p.performance)
    {
        current = &(*current)->roleNext;
    }
    newNode->roleNext = *current;
    *current = newNode;
    team[t].count++;
}

// Binary search for team ID
int searchTeam(int id)
{
    int low = 0, high = MAX - 1;
    while(low <= high)
    {
        int mid = (low + high) / 2;
        if(team[mid].id == id)
        {
            return mid;
        }
        if(team[mid].id < id)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1;
}

// Calculate average batting strike rate
float averageSR(int t)
{
    float total = 0;
    int count = 0;
    PlayerNode *temp = team[t].head;
    while(temp != NULL)
    {
        if(strcmp(temp->p.role, "Bowler") != 0)
        {
            total += temp->p.sr;
            count++;
        }
        temp = temp->next;
    }
    if(count == 0)
    {
        return 0;
    }
    return total / count;
}

// Display player details
void printPlayer(ODIPlayer p)
{
    printf("%-5d %-23s %-13s %-7d %-7.1f %-7.1f %-6d %-6.1f %.2f\n", p.id, p.name, p.role, p.runs, p.avg, p.sr, p.wickets, p.economy, p.performance);
}

void printHeading()
{
    printf("==========================================================================\n");
    printf("%-5s %-23s %-13s %-7s %-7s %-7s %-6s %-6s %s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("==========================================================================\n");
}

// Initialize teams and players from header file
void initialize()
{
    for(int i = 0; i < MAX; i++)
    {
        team[i].id = i + 1;
        strcpy(team[i].name, teams[i]);
        team[i].count = 0;
        team[i].head = NULL;
        for(int j = 0; j < 3; j++)
        {
            team[i].roleHead[j] = NULL;
        }
    }
    for(int i = 0; i < playerCount; i++)
    {
        ODIPlayer p;
        p.id = players[i].id;
        strcpy(p.name, players[i].name);
        strcpy(p.role, players[i].role);
        p.runs = players[i].totalRuns;
        p.avg = players[i].battingAverage;
        p.sr = players[i].strikeRate;
        p.wickets = players[i].wickets;
        p.economy = players[i].economyRate;
        p.performance = calculatePerformance(p);
        int t = -1;
        for(int j = 0; j < MAX; j++)
        {
            if(strcmp(team[j].name, players[i].team) == 0)
            {
                t = j;
                break;
            }
        }
        if(t != -1)
        {
            addNode(t, p);
        }
    }
}

// Add new player
void addPlayer()
{
    int id, r, runs, wickets;
    float avg, sr, economy;
    char name[51];
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int t = searchTeam(id);
    if(t == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }
    if(team[t].count >= 50)
    {
        printf("Team is full!\n");
        return;
    }
    printf("Enter Player ID: ");
    scanf("%d", &id);
    if(id < 1 || id > 1000)
    {
        printf("Invalid Player ID!\n");
        return;
    }
    for(int i = 0; i < MAX; i++)
    {
        PlayerNode *temp = team[i].head;
        while(temp != NULL)
        {
            if(temp->p.id == id)
            {
                printf("Player ID already exists!\n");
                return;
            }
            temp = temp->next;
        }
    }
    ODIPlayer p;
    p.id = id;
    printf("Enter Name: ");
    scanf(" %50[^\n]", name);
    strcpy(p.name, name);
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &r);
    if(r < 1 || r > 3)
    {
        printf("Invalid role!\n");
        return;
    }
    strcpy(p.role, roleName(r - 1));
    printf("Total Runs: ");
    scanf("%d", &runs);
    printf("Batting Average: ");
    scanf("%f", &avg);
    printf("Strike Rate: ");
    scanf("%f", &sr);
    printf("Wickets: ");
    scanf("%d", &wickets);
    printf("Economy Rate: ");
    scanf("%f", &economy);
    if(runs < 0 || avg < 0 || sr < 0 || wickets < 0 || economy < 0)
    {
        printf("Invalid statistics!\n");
        return;
    }
    p.runs = runs;
    p.avg = avg;
    p.sr = sr;
    p.wickets = wickets;
    p.economy = economy;
    p.performance = calculatePerformance(p);
    addNode(t, p);
    printf("Player added successfully to Team %s!\n", team[t].name);
}

// Display players of a team
void displayTeam()
{
    int id;
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int t = searchTeam(id);
    if(t == -1)
    {
        printf("Team not found!\n");
        return;
    }
    printf("\nPlayers of Team %s:\n", team[t].name);
    printHeading();
    PlayerNode *temp = team[t].head;
    while(temp != NULL)
    {
        printPlayer(temp->p);
        temp = temp->next;
    }
    printf("==========================================================================\n");
    printf("Total Players: %d\n", team[t].count);
    printf("Average Batting Strike Rate: %.2f\n", averageSR(t));
}

// Display teams by average strike rate
void sortTeams()
{
    int order[MAX];
    for(int i = 0; i < MAX; i++)
    {
        order[i] = i;
    }
    for(int i = 0; i < MAX - 1; i++)
    {
        for(int j = 0; j < MAX - i - 1; j++)
        {
            if(averageSR(order[j]) < averageSR(order[j + 1]))
            {
                int temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("ID   Team Name          Avg Bat SR     Total Players\n");
    printf("=========================================================\n");
    for(int i = 0; i < MAX; i++)
    {
        int t = order[i];
        printf("%-5d %-19s %-14.2f %d\n", team[t].id, team[t].name, averageSR(t), team[t].count);
    }
    printf("=========================================================\n");
}

// Display top K players of a team
void topKPlayers()
{
    int id, r, k;
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int t = searchTeam(id);
    if(t == -1)
    {
        printf("Team not found!\n");
        return;
    }
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &r);
    if(r < 1 || r > 3)
    {
        printf("Invalid role!\n");
        return;
    }
    printf("Enter number of players: ");
    scanf("%d", &k);
    if(k <= 0)
    {
        printf("Invalid K!\n");
        return;
    }
    printf("\nTop %d %s of Team %s:\n", k, roleName(r - 1), team[t].name);
    printHeading();
    PlayerNode *temp = team[t].roleHead[r - 1];
    int count = 0;
    while(temp != NULL && count < k)
    {
        printPlayer(temp->p);
        temp = temp->roleNext;
        count++;
    }
    if(count == 0)
    {
        printf("No players found!\n");
    }
}

// Heap functions for merging sorted team lists
typedef struct
{
    PlayerNode *node;
    int teamIndex;
} HeapItem;

HeapItem heap[MAX];
int heapSize = 0;

void push(HeapItem item)
{
    int i = heapSize++;
    while(i > 0)
    {
        int parent = (i - 1) / 2;
        if(heap[parent].node->p.performance >= item.node->p.performance)
        {
            break;
        }
        heap[i] = heap[parent];
        i = parent;
    }
    heap[i] = item;
}

HeapItem pop()
{
    HeapItem result = heap[0];
    HeapItem last = heap[--heapSize];
    int i = 0;
    while(2 * i + 1 < heapSize)
    {
        int child = 2 * i + 1;
        if(child + 1 < heapSize && heap[child + 1].node->p.performance > heap[child].node->p.performance)
        {
            child++;
        }
        if(last.node->p.performance >= heap[child].node->p.performance)
        {
            break;
        }
        heap[i] = heap[child];
        i = child;
    }
    if(heapSize > 0)
    {
        heap[i] = last;
    }
    return result;
}

// Display all players of a role across all teams
void displayAllRole()
{
    int r;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &r);
    if(r < 1 || r > 3)
    {
        printf("Invalid role!\n");
        return;
    }
    r--;
    heapSize = 0;
    // Add first player of each team's sorted list
    for(int i = 0; i < MAX; i++)
    {
        if(team[i].roleHead[r] != NULL)
        {
            HeapItem item;
            item.node = team[i].roleHead[r];
            item.teamIndex = i;
            push(item);
        }
    }
    printf("\n%s of All Teams:\n", roleName(r));
    printf("============================================================================\n");
    printf("%-5s %-23s %-15s %-13s %-7s %-7s %-6s %-6s %s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "Perf.Index");
    printf("============================================================================\n");
    // Merge the sorted lists
    while(heapSize > 0)
    {
        HeapItem item = pop();
        ODIPlayer p = item.node->p;
        printf("%-5d %-23s %-15s %-13s %-7d %-7.1f %-6.1f %-6d %.2f\n", p.id, p.name, team[item.teamIndex].name, p.role, p.runs, p.avg, p.sr, p.wickets, p.performance);
        // Add next player from the same team's list
        if(item.node->roleNext != NULL)
        {
            item.node = item.node->roleNext;
            push(item);
        }
    }
    printf("============================================================================\n");
}

int main()
{
    initialize();
    int choice;
    while(1)
    {
        printf("\n");
        printf("====================================================\n");
        printf("       ICC ODI Player Performance Analyzer\n");
        printf("====================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams\n");
        printf("6. Exit\n");
        printf("====================================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1:
                addPlayer();
                break;
            case 2:
                displayTeam();
                break;
            case 3:
                sortTeams();
                break;
            case 4:
                topKPlayers();
                break;
            case 5:
                displayAllRole();
                break;
            case 6:
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}