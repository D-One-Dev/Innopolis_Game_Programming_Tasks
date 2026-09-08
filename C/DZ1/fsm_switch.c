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

        switch (state)
        {
            case 0:
                switch (ch)
                {
                    case 'a':
                        state = 1;
                        break;
                    case 'b':
                        state = 2;
                        break;
                    case 'c':
                        state = 3;
                        break;
                    default:
                        break;
                }
                break;
            case 1:
                switch (ch)
                {
                    case 'a':
                        state = 2;
                        break;
                    case 'b':
                        state = 3;
                        break;
                    case 'd':
                        state = 0;
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                switch (ch)
                {
                    case 'b':
                        state = 3;
                        break;
                    case 'c':
                        state = 1;
                        break;
                    case 'd':
                        state = 0;
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                switch (ch)
                {
                    case 'c':
                        state = 1;
                        break;
                    case 'a':
                        state = 2;
                        break;
                    case 'd':
                        state = 0;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
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
