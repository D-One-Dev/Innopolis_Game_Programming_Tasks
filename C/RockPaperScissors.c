// Камень-ножницы-бумага для одного на Си

#include <stdio.h>
#include <string.h>

char *rockWins = "Rock Wins!\n";
char *paperWins = "Paper wins!\n";
char *scissorsWin = "Scissors win!\n";
char *draw = "Draw!\n";
char *invalidInput = "Invalid input!\n";

int main(int argc, char *argv[])
{
  char *player1 = argv[1];
  char *player2 = argv[2];

  if (!strcmp(player1, "rock"))
  {
    if (!strcmp(player2, "rock")) printf(draw);
    else if (!strcmp(player2, "paper")) printf(paperWins);
    else if (!strcmp(player2, "scissors")) printf(scissorsWin);
    else printf(invalidInput);
  }

  else if (!strcmp(player1, "paper"))
  {
    if (!strcmp(player2, "rock")) printf(paperWins);
    else if (!strcmp(player2, "paper")) printf(draw);
    else if (!strcmp(player2, "scissors")) printf(scissorsWin);
    else printf(invalidInput);
  }

  else if (!strcmp(player1, "scissors"))
  {
    if (!strcmp(player2, "rock")) printf(rockWins);
    else if (!strcmp(player2, "paper")) printf(scissorsWin);
    else if (!strcmp(player2, "scissors")) printf(draw);
    else printf(invalidInput);
  }

  else printf(invalidInput);
}
