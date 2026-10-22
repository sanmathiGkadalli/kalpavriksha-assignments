#include<stdio.h>

#define MAX 100

struct Employee
{
    int id;
    char name[30];
    char designation[30];
    int reports[MAX];
    int reportCount;
    int manager;    
};

struct Employee emp[MAX];
int n = 0;

// Find employee
int findEmployee(int id)
{
    for(int i = 0; i < n; i++)
    {
        if(emp[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

// Add employee
void addEmployee()
{
    int id, managerId, temp, i;

    if(n == MAX)
    {
        printf("Employee limit reached.\n");
        return;
    }

    printf("Enter employee ID: ");
    scanf("%d", &id);

    if(findEmployee(id) != -1)
    {
        printf("Employee ID already exists.\n");
        return;
    }

    printf("Enter employee name: ");
    scanf(" %29[^\n]", emp[n].name);
    printf("Enter employee designation: ");
    scanf(" %29[^\n]", emp[n].designation);
    printf("Enter reporting manager ID (-1 for top-level): ");
    scanf("%d", &managerId);

    if(managerId != -1 && findEmployee(managerId) == -1)
    {
        printf("Manager not found.\n");
        return;
    }

    if(managerId == id)
    {
        printf("Can't report to yourself.\n");
        return;
    }

    temp = managerId;

    // Checking circular reporting
    while(temp != -1)
    {
        if(temp == id)
        {
            printf("Circular reporting not allowed.");
            return;
        }

        i = findEmployee(temp);

        if(i == -1)
        {
            break;
        }

        temp = emp[i].manager;
    }

    emp[n].id = id;
    emp[n].manager = managerId;
    emp[n].reportCount = 0;

    if (managerId != -1)
    {
        temp = findEmployee(managerId);
        emp[temp].reports[emp[temp].reportCount] = id;
        emp[temp].reportCount++;
    }

    n++;

    printf("Employee added successfully.\n");
}

// Remove employee
void removeEmployee()
{
    int id, i, j, k, temp, managerId;
    int sibling = -1, fewest = MAX + 1;
    int child[MAX], count;

    printf("Enter Employee ID to remove: ");
    scanf("%d", &id);

    i = findEmployee(id);

    if (i == -1)
    {
        printf("Employee not found.\n");
        return;
    }

    if (emp[i].manager == -1)
    {
        printf("CEO cannot be removed.\n");
        return;
    }

    managerId = emp[i].manager;
    count = emp[i].reportCount;

    for (j = 0; j < count; j++)
    {
        child[j] = emp[i].reports[j];
    }

    // Find sibling with fewest reports
    temp = findEmployee(managerId);

    for (j = 0; j < emp[temp].reportCount; j++)
    {
        int s = findEmployee(emp[temp].reports[j]);

        if (emp[s].id != id && emp[s].reportCount < fewest)
        {
            fewest = emp[s].reportCount;
            sibling = emp[s].id;
        }
    }

    // Remove employee from manager's reports
    for (j = 0; j < emp[temp].reportCount; j++)
    {
        if (emp[temp].reports[j] == id)
        {
            for (k = j; k < emp[temp].reportCount - 1; k++)
            {
                emp[temp].reports[k] = emp[temp].reports[k + 1];
            }

            emp[temp].reportCount--;
            break;
        }
    }

    // Transfer direct reports
    for (j = 0; j < count; j++)
    {
        int c = findEmployee(child[j]);
        int newManager = managerId;

        if (sibling != -1)
        {
            newManager = sibling;
        }

        emp[c].manager = newManager;

        temp = findEmployee(newManager);
        emp[temp].reports[emp[temp].reportCount] = child[j];
        emp[temp].reportCount++;
    }

    // Delete employee from array
    for (j = i; j < n - 1; j++)
    {
        emp[j] = emp[j + 1];
    }

    n--;

    printf("Employee removed successfully!\n");
}

// Count employees recursively
int countUnder(int id)
{
    int i, j, count = 0;

    i = findEmployee(id);

    if (i == -1)
    {
        return 0;
    }

    for (j = 0; j < emp[i].reportCount; j++)
    {
        count++;
        count += countUnder(emp[i].reports[j]);
    }

    return count;
}

// Display hierarchy recursively
void displayHierarchy(int id, int level)
{
    int i, j;

    i = findEmployee(id);

    if (i == -1)
    {
        return;
    }

    for (j = 0; j < level; j++)
    {
        printf("    ");
    }

    printf("%d - %s - %s\n", emp[i].id, emp[i].name, emp[i].designation);

    for (j = 0; j < emp[i].reportCount; j++)
    {
        displayHierarchy(emp[i].reports[j], level + 1);
    }
}

// Display one employee's hierarchy
void displayOne()
{
    int id;

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    if (findEmployee(id) == -1)
    {
        printf("Employee not found!\n");
        return;
    }

    displayHierarchy(id, 0);
    printf("Total Employees Under %d: %d\n", id, countUnder(id));
}

// Display complete hierarchy
void displayAll()
{
    int i, total = 0;

    printf("\n===== ORGANIZATION HIERARCHY =====\n");

    for (i = 0; i < n; i++)
    {
        if (emp[i].manager == -1)
        {
            displayHierarchy(emp[i].id, 0);
        }

        total++;
    }

    printf("Total Employees: %d\n", total);
}

// Main function
int main()
{
    int choice;

    while(1)
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                addEmployee();
                break;

            case 2:
                removeEmployee();
                break;

            case 3:
                displayOne();
                break;

            case 4:
                displayAll();
                break;

            case 5:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }

    return 0;
}