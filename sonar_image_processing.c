#include<stdio.h>
#include<stdlib.h>
#include<time.h>

// Generate random matrix
void generate(int *matrix, int n)
{
    int i;

    for(i = 0; i < n * n; i++)
    {
        *(matrix + i) = rand() % 256;
    }
}

// Display matrix
void display(int *matrix, int n)
{
    int i, j;

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            printf("%4d", *(matrix + i * n + j));
        }

        printf("\n");
    }
}

// Swap
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Rotate matrix
void rotate(int *matrix, int n)
{
    int i, j, temp;
    int *p, *q;

    // Transpose matrix
    for(i = 0; i < n; i++)
    {
        for(j = i + 1; j < n; j++)
        {
            p = matrix + i * n + j;
            q = matrix + j * n + i;
            swap(p, q);
        }
    }

    // Reverse each row
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n / 2; j++)
        {
            p = matrix + i * n + j;
            q = matrix + i * n + (n - 1 - j);
            swap(p, q);
        }
    }
}

// 3x3 Smoothing
void smooth(int *matrix, int n)
{
    int *prev = malloc(n * sizeof(int));
    int *curr = malloc(n * sizeof(int));
    int *next = malloc(n * sizeof(int));

    if(prev == NULL || curr == NULL || next == NULL)
    {
        free(prev);
        free(curr);
        free(next);
        return;
    }

    // Save the first row
    for(int col = 0; col < n; col++)
    {
        *(curr + col) = *(matrix + col);
    }

    for(int row = 0; row < n; row++)
    {
        // Save the next original row before overwriting
        if(row + 1 < n)
        {
            for(int col = 0; col < n; col++)
            {
                *(next + col) = *(matrix + (row + 1) * n + col);
            }
        }

        for(int col = 0; col < n; col++)
        {
            int sum = 0, count = 0;

            for(int r = row - 1; r <= row + 1; r++)
            {
                for(int c = col - 1; c <= col + 1; c++)
                {
                    if(r < 0 || r >= n || c < 0 || c >= n)
                    {
                        continue;
                    }

                    if(r == row - 1)
                    {
                        sum += *(prev + c);
                    }
                    else if(r == row)
                    {
                        sum += *(curr + c);
                    }
                    else
                    {
                        sum += *(next + c);
                    }
                    count++;
                }
            }

            *(prev + col) = sum / count;
        }

        // Write smoothed values into the matrix
        for(int col = 0; col < n; col++)
        {
            *(matrix + row * n + col) = *(prev + col);
        }

        // Prepare rows for the next iteration
        if(row + 1 < n)
        {
            int *temp = prev;
            prev = curr;
            curr = next;
            next = temp;
        }
    }

    free(prev);
    free(curr);
    free(next);
}

int main()
{
    int n;
    int a[10][10];
    int *p = &a[0][0];

    srand(time(NULL));

    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);

    if (n < 2 || n > 10)
    {
        printf("Invalid matrix size\n");
        return 0;
    }

    generate(p, n);

    printf("\nOriginal Randomly Generated Matrix:\n");
    display(p, n);

    rotate(p, n);

    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    display(p, n);

    smooth(p, n);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    display(p, n);

    return 0;
}