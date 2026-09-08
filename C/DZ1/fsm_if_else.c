#include <stdio.h>
#include <string.h>

void fsm(char *input)
{
    int state = 0;
    int length = strlen(input);

    for (int i = 0; i < length; i++)
    {
        char ch = input[i];
        int prevState = state;

        if (state == 0)
        {
            if (ch == 'a') state = 1;
            else if (ch == 'b') state = 2;
            else if (ch == 'c') state = 3;
        }

        else if (state == 1)
        {
            if (ch == 'a') state = 2;
            else if (ch == 'b') state = 3;
            else if (ch == 'd') state = 0;
        }

        else if (state == 2)
        {
            if (ch == 'b') state = 3;
            else if (ch == 'c') state = 1;
            else if (ch == 'd') state = 0;
        }

        else if (state == 3)
        {
            if (ch == 'c') state = 1;
            else if (ch == 'a') state = 2;
            else if (ch == 'd') state = 0;
        }

        printf("Symbol: %c | Previous state: %d | New state: %d\n", ch, prevState, state);
    }
    
    printf("\n\n");
}

int main()
{
    char *str1 = "abcdabcdabcdabcdad";
    char *str2 = "dcbaabcddcbaabcdda";
    char *str3 = "aaaabbbbccccddddab";

    fsm(str1);
    fsm(str2);
    fsm(str3);

    return 0;
}
