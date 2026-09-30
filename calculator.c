#include<stdio.h>
#include<ctype.h>
int num[100];
char op[100];
int numTop = -1, opTop = -1;
void pushNum(int x) {
    num[++numTop] = x;
}
int popNum() {
    return num[numTop--];
}
void pushOp(char x) {
    op[++opTop] = x;
}
char popOp() {
    return op[opTop--];
}
int priority(char x) {
    if(x == '*' || x == '/') {
        return 2;
    }
    return 1;
}
int calculate(int a, int b, char x) {
    if(x == '+') {
        return a + b;
    } else if(x == '-') {
        return a - b;
    } else if(x == '*') {
        return a * b;
    } else {
        return a / b;
    }
}
int main() {
    char s[100];
    int i = 0, valid = 1;
    printf("Enter expression: ");
    fgets(s, 100, stdin);
    while(s[i] != '\0' && s[i] != '\n') {
        if(s[i] == ' ') {
            i++;
            continue;
        }
        if(isdigit(s[i])) {
            int n = 0;
            //for double or more digit numbers
            while(isdigit(s[i])) {
                n = n * 10 + (s[i] - '0');
                i++;
            }
            pushNum(n);
        } else if(s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
            char x = s[i];
            //when operator on top in operator stack has more precedence than current operator
            while(opTop >= 0 && priority(op[opTop]) >= priority(x)) {
                char o = popOp();
                int b = popNum(), a = popNum();
                if(o == '/' && b == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                pushNum(calculate(a, b, o));
            }
            pushOp(x);
            i++;
        } else {
            valid = 0;
            break;
        }
    }
    //for remaining operators in stack if any after precedence check
    while(opTop >= 0 && valid) {
        char o = popOp();
        int b = popNum(), a = popNum();
        if(o == '/' && b == 0) {
            printf("Error: Division by zero.\n");
            return 0;
        }
        pushNum(calculate(a, b, o));
    }
    if(!valid || numTop != 0) {
        printf("Error: Invalid expression.\n");
    } else {
        printf("%d\n", num[numTop]);
    }
    return 0;
}