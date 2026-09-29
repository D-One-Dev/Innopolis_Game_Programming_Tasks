#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 3
#define CELLS (SIZE * SIZE)

#define EMPTY ' '
#define PLAYER 'X'
#define COMPUTER 'O'

static char board[CELLS];

static const int win_lines[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
    {0, 4, 8}, {2, 4, 6}
};

static void ClearBoard(void)
{
    int i;
    for (i = 0; i < CELLS; i++) {
        board[i] = EMPTY;
    }
}

static int IsEmpty(int idx)
{
    return board[idx] == EMPTY;
}

static void PrintCell(char c)
{
    putchar(c);
}

static void PrintBoard(void)
{
    int r, c;

    printf("    1 2 3\n");
    for (r = 0; r < SIZE; r++) {
        printf("%d   ", r + 1);
        for (c = 0; c < SIZE; c++) {
            PrintCell(board[r * SIZE + c]);
            printf(" ");
        }
        putchar('\n');
    }
    putchar('\n');
}

static char CheckWinner(void)
{
    int i, a, b, c;

    for (i = 0; i < 8; i++) {
        a = win_lines[i][0];
        b = win_lines[i][1];
        c = win_lines[i][2];
        if (board[a] != EMPTY && board[a] == board[b] && board[b] == board[c]) {
            return board[a];
        }
    }
    return 0;
}

static int IsBoardFull(void)
{
    int i;
    for (i = 0; i < CELLS; i++) {
        if (IsEmpty(i)) {
            return 0;
        }
    }
    return 1;
}

static int AskFirstPlayer(void)
{
    char line[64];

    for (;;) {
        printf("Who's starting? 1 - player, 2 - computer: ");
        fflush(stdout);
        if (fgets(line, sizeof line, stdin) == NULL) {
            exit(0);
        }
        if (line[0] == '1') {
            return 1;
        }
        if (line[0] == '2') {
            return 0;
        }
        printf("Wrong input.");
    }
}

static int ReadPlayerMove(void)
{
    char buf[64];
    size_t len;
    int r, c, idx;

    printf("Your turn (format:YX ('23')): ");
    fflush(stdout);

    if (fgets(buf, sizeof buf, stdin) == NULL) {
        exit(0);
    }

    len = 0;
    while (buf[len] != '\0' && buf[len] != '\n' && buf[len] != '\r') {
        len++;
    }
    buf[len] = '\0';

    if (len != 2 || buf[0] < '1' || buf[0] > '3' || buf[1] < '1' || buf[1] > '3') {
        printf("Wrong input.\n\n");
        return 0;
    }

    r = buf[0] - '1';
    c = buf[1] - '1';
    idx = r * SIZE + c;

    if (!IsEmpty(idx)) {
        printf("Cell %s already taken.\n\n", buf);
        return 0;
    }

    board[idx] = PLAYER;
    return 1;
}

static void ComputerMove(void)
{
    int empty[CELLS];
    int count = 0;
    int i, idx;

    for (i = 0; i < CELLS; i++) {
        if (!IsEmpty(i)) {
            continue;
        }
        board[i] = COMPUTER;
        if (CheckWinner() == COMPUTER) {
            printf("Computer's turn: %d%d\n\n", i / SIZE + 1, i % SIZE + 1);
            return;
        }
        board[i] = EMPTY;
    }

    for (i = 0; i < CELLS; i++) {
        if (IsEmpty(i)) {
            empty[count++] = i;
        }
    }
    if (count == 0) {
        return;
    }

    idx = empty[rand() % count];
    board[idx] = COMPUTER;
    printf("Computer's turn: %d%d\n\n", idx / SIZE + 1, idx % SIZE + 1);
}

enum game_state {
    STATE_CONTINUE,
    STATE_PLAYER_WIN,
    STATE_COMPUTER_WIN,
    STATE_DRAW
};

static int GetState(void)
{
    char winner = CheckWinner();

    if (winner == PLAYER) {
        return STATE_PLAYER_WIN;
    }
    if (winner == COMPUTER) {
        return STATE_COMPUTER_WIN;
    }
    if (IsBoardFull()) {
        return STATE_DRAW;
    }
    return STATE_CONTINUE;
}

static void PrintResult(int state)
{
    PrintBoard();
    if (state == STATE_PLAYER_WIN) {
        printf("Player won (X)!\n");
    } else if (state == STATE_COMPUTER_WIN) {
        printf("Computer won (O)!\n");
    } else {
        printf("Draw - all cells taken.\n");
    }
}

int main(void)
{
    int player_first;

    srand((unsigned)time(NULL));

    player_first = AskFirstPlayer();

    ClearBoard();
    printf("\nYou're playing as 'X', computer - as 'O'.\n\n");

    if (!player_first) {
        ComputerMove();
    }
    PrintBoard();

    for (;;) {
        int state;

        if (!ReadPlayerMove()) {
            continue;
        }
        state = GetState();
        if (state != STATE_CONTINUE) {
            PrintResult(state);
            break;
        }
        ComputerMove();
        state = GetState();
        if (state != STATE_CONTINUE) {
            PrintResult(state);
            break;
        }
        PrintBoard();
    }

    return 0;
}
