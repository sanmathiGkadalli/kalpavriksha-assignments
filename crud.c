#include<stdio.h>
struct User {
    int id;
    char name[30];
    int age;
};
//Create a new file (if it doesn't exist)
void createFile() {
    FILE *fptr = fopen("users.txt", "a");
    if(fptr == NULL) {
        printf("Error creating file.\n");
        return;
    }
    fclose(fptr);
}
//Create user
void createUser() {
    struct User u, temp;
    FILE *fptr;
    int found = 0;
    printf("Enter ID: ");
    scanf("%d", &u.id);
    //to check whether id already exists
    fptr = fopen("users.txt", "r");
    while (fscanf(fptr, "%d %s %d", &temp.id, temp.name, &temp.age) != EOF) {
        if (temp.id == u.id) {
            found = 1;
            break;
        }
    }
    fclose(fptr);
    if (found) {
        printf("ID already exists.\n");
        return;
    }
    printf("Enter Name: ");
    scanf("%s", u.name);
    printf("Enter Age: ");
    scanf("%d", &u.age);
    fptr = fopen("users.txt", "a");
    fprintf(fptr, "%d %s %d\n", u.id, u.name, u.age);
    fclose(fptr);
    printf("User added successfully.\n");
}
//Read the file and display the contents
void readFile() {
    struct User u;
    FILE *fptr = fopen("users.txt", "r");
    if(fptr == NULL) {
        return;
    }
    printf("ID  Name    Age\n");
    while(fscanf(fptr, "%d %s %d", &u.id, u.name, &u.age) != EOF) {
        printf("%d  %s    %d\n", u.id, u.name, u.age);
    }
    fclose(fptr);
}
//update a specific record
void updateRecord() {
    struct User u;
    FILE *fptr, *temp;
    int id, found = 0;
    printf("Enter ID to update: ");
    scanf("%d", &id);
    fptr = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");
    while(fscanf(fptr, "%d %s %d", &u.id, u.name, &u.age) != EOF) {
        if(u.id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf("%s", u.name);
            printf("Enter new age: ");
            scanf("%d", &u.age);
        }
        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }
    fclose(fptr);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");
    if(found) {
        printf("User updated successfully.\n");
    } else {
        printf("User not found.\n");
    }
}
//delete a record by modifying the file content
void deleteRecord() {
    struct User u;
    FILE *fptr, *temp;
    int id, found = 0;
    printf("Enter ID to delete: ");
    scanf("%d", &id);
    fptr = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");
    while(fscanf(fptr, "%d %s %d", &u.id, u.name, &u.age) != EOF) {
        if(u.id == id) {
            found = 1;
        } else {
            fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
        }
    }
    fclose(fptr);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");
    if(found) {
        printf("User deleted successfully.\n");
    } else {
        printf("User not found.\n");
    }
}
int main() {
    int choice;
    createFile();
    do {
        printf("1. Create\n");
        printf("2. Read\n");
        printf("3. Update\n");
        printf("4. Delete\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                createUser();
                break;
            case 2:
                readFile();
                break;
            case 3:
                updateRecord();
                break;
            case 4:
                deleteRecord();
                break;
        }
    } while(choice != 5);
    return 0;
}