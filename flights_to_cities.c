#include <stdio.h>
#include <string.h>

int main()
{
    int n;
    char source[100005][101], destination[100005][101];
    int incoming[100005] = {0};
    scanf("%d", &n);

    for(int i = 0; i < n; i++)
    {
        scanf("%s -> %s", source[i], destination[i]);
    }

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(strcmp(source[i], destination[j]) == 0)
            {
                incoming[i] = 1;
            }
        }
    }

    int start = 0;
    for(int i = 0; i < n; i++)
    {
        if(incoming[i] == 0)
        {
            start = i;
            break;
        }
    }

    for(int i = 0; i < n; i++)
    {
        printf("%s->%s", source[start], destination[start]);
        if(i != n - 1)
        {
            printf(", ");
        }
        for(int j = 0; j < n; j++)
        {
            if(strcmp(destination[start], source[j]) == 0)
            {
                start = j;
                break;
            }
        }
    }

    return 0;
}