#include<stdio.h>
#include<string.h>

//declare constants for ticket price
#define STD_PRICE 150
#define PRE_PRICE 250
#define REC_PRICE 400

//calculate subtotal
float calc_subtotal(int choice, int n, char ticket_category[])
{
    float total;

    switch(choice)
    {
        case 1:
            strcpy(ticket_category, "Standard");
            total = STD_PRICE * n;
            break;
        case 2:
            strcpy(ticket_category, "Premium");
            total = PRE_PRICE * n;
            break;
        case 3:
            strcpy(ticket_category, "Recliner");
            total = REC_PRICE * n;
            break;
    }

    return total;
}

//calculate discount
float calc_discount(int n, float total) 
{
    float discount;

    if(n >= 5) 
    {
        discount = 0.1 * total;
    }
    else 
    {
        discount = 0;
    }

    return discount;
}

//calculate total
float calc_total(float total, float discount)
{
    return total - discount;
}

int main() 
{
    char name[30], ticket_category[10];
    int ticket_no, ticket_category_no;
    float total, sub_total, discount;

    printf("Enter name: ");
    scanf("%s", name);
    printf("Enter number of tickets: ");
    scanf("%d", &ticket_no);

    if(ticket_no < 1 || ticket_no > 20) 
    {
        printf("No sufficient tickets. Only 20 tickets available");
        return 1;
    }

    printf("Enter ticket category (1. Standard 2. Premium 3. Recliner): ");
    scanf("%d", &ticket_category_no);

    if(ticket_category_no < 1 || ticket_category_no > 3) 
    {
        printf("Invalid category type. Please select 1, 2 or 3\n");
        return 1;
    }

    sub_total = calc_subtotal(ticket_category_no, ticket_no, ticket_category);
    discount = calc_discount(ticket_no, sub_total);
    total = calc_total(sub_total, discount);

    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", ticket_category);
    printf("Tickets: %d\n", ticket_no);
    printf("Subtotal: %.2f\n", sub_total);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", total);
}