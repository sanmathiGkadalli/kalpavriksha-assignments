#include<stdio.h>
#define MAX 100
struct Student {
    int rollNo;
    char name[30];
    float marks1, marks2, marks3;
};
//calculate total marks
float totalMarks(struct Student s) {
    return s.marks1 + s.marks2 + s.marks3;
}
//calculate avg
float averageMarks(float total) {
    return total / 3.0;
}
//grade assignment
char assignGrade(float avg) {
    if(avg >= 85) {
        return 'A';
    } else if(avg >= 70) {
        return 'B';
    } else if(avg >= 50) {
        return 'C';
    } else if(avg >= 35) {
        return 'D';
    } else {
        return 'F';
    }
}
//print stars
void performancePattern(char g) {
    int num;
    if(g == 'A') {
        num = 5;
    } else if(g == 'B') {
        num = 4;
    } else if(g == 'C') {
        num = 3;
    } else if(g == 'D') {
        num = 2;
    } else {
        num = 0;
    }
    for(int i = 0; i < num; i++) {
        printf("*");
    }
    printf("\n");
}
//print roll nos
void printRollNos(struct Student std[], int n) {
    if(n <= 0) {
        return;
    }
    printRollNos(std, n - 1);
    printf("%d ", std[n - 1].rollNo);
}
int main() {
    struct Student std[MAX];
    int n;
    printf("Enter total number of students: ");
    scanf("%d", &n);
    for(int i = 0; i < n; i++) {
        scanf("%d %s %f %f %f", &std[i].rollNo, std[i].name, &std[i].marks1, &std[i].marks2, &std[i].marks3);
    }
    for(int i = 0; i < n; i++) {
        printf("Roll: %d\n", std[i].rollNo);
        printf("Name: %s\n", std[i].name);
        float total = totalMarks(std[i]);
        printf("Total: %.0f\n", total);
        float average = averageMarks(total);
        printf("Average: %.2f\n", average);
        char grade = assignGrade(average);
        printf("Grade: %c\n", grade);
        if(average < 35) {
            continue;
        }
        printf("Performance: ");
        performancePattern(grade);
        printf("\n");
    }
    printf("List of Roll Numbers (via recursion): ");
    printRollNos(std, n);
}