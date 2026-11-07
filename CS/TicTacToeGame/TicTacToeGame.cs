using System;
using System.Collections.Generic;

class Program
{
    static void Main()
    {
        TicTacToeGame game = new TicTacToeGame();
        game.Run();
    }
}

class TicTacToeGame
{
    private readonly Board _board = new Board();
    private readonly Random _rnd = new Random();
    private readonly char _humanSymbol = 'X';
    private readonly char _computerSymbol = 'O';

    public void Run()
    {
        Console.WriteLine("--- Tic-Tac-Toe 3x3 ---");
        Console.Write("Who starts? (1 - player, 2 - ai): ");
        string input = Console.ReadLine() ?? "";
        bool humanFirst = input.Trim() == "1";

        if (!humanFirst)
        {
            Console.WriteLine("AI's turn");
            MakeComputerMove();
            if (CheckWin(_computerSymbol))
            {
                PrintBoard();
                Console.WriteLine("AI won!");
                return;
            }
        }

        while (true)
        {
            PrintBoard();

            if (!MakeHumanMove())
                continue;

            if (CheckWin(_humanSymbol))
            {
                PrintBoard();
                Console.WriteLine("You won!");
                return;
            }

            if (_board.IsFull)
            {
                PrintBoard();
                Console.WriteLine("Draw");
                return;
            }

            MakeComputerMove();

            if (CheckWin(_computerSymbol))
            {
                PrintBoard();
                Console.WriteLine("AI won!");
                return;
            }

            if (_board.IsFull)
            {
                PrintBoard();
                Console.WriteLine("Draw!");
                return;
            }
        }
    }

    private void PrintBoard()
    {
        Console.WriteLine();
        for (int r = 0; r < 3; r++)
        {
            Console.Write(" ");
            for (int c = 0; c < 3; c++)
            {
                char cell = _board[r, c];
                Console.Write(cell == ' ' ? "." : cell.ToString());
                if (c < 2) Console.Write(" | ");
            }
            Console.WriteLine();
            if (r < 2) Console.WriteLine("---+---+---");
        }
        Console.WriteLine();
    }

    private bool MakeHumanMove()
    {
        Console.Write("Your turn (Format: RowColumn, I.E. 11): ");
        string s = Console.ReadLine() ?? "";
        if (s.Length != 2 || !char.IsDigit(s[0]) || !char.IsDigit(s[1]))
        {
            Console.WriteLine("Wrong format. Enter two numbers, I.E. 11.");
            return false;
        }

        int row = s[0] - '1';
        int col = s[1] - '1';

        if (row < 0 || row > 2 || col < 0 || col > 2)
        {
            Console.WriteLine("Coordinates out of filed's bounds (1-3).");
            return false;
        }

        if (!_board.IsEmpty(row, col))
        {
            Console.WriteLine("This cell is already taken.");
            return false;
        }

        _board[row, col] = _humanSymbol;
        return true;
    }

    private void MakeComputerMove()
    {
        var move = ChooseComputerMove();
        _board[move.Row, move.Col] = _computerSymbol;
        Console.WriteLine($"AI made it's turn: {move.Row + 1}{move.Col + 1}");
    }

    private (int Row, int Col) ChooseComputerMove()
    {
        (int Row, int Col)? FindWinningMove(char symbol)
        {
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                    if (_board.IsEmpty(r, c))
                    {
                        _board[r, c] = symbol;
                        bool wins = CheckWin(symbol);
                        _board[r, c] = ' ';
                        if (wins) return (r, c);
                    }
            return null;
        }

        var winMove = FindWinningMove(_computerSymbol);
        if (winMove.HasValue) return winMove.Value;

        var blockMove = FindWinningMove(_humanSymbol);
        if (blockMove.HasValue) return blockMove.Value;

        return FindMaxLinesMove();
    }

    private (int Row, int Col) FindMaxLinesMove()
    {
        var candidates = new List<(int Row, int Col)>();
        int maxScore = -1;

        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (_board.IsEmpty(r, c))
                {
                    int score = CountLinesThrough(r, c);
                    if (score > maxScore)
                    {
                        maxScore = score;
                        candidates.Clear();
                        candidates.Add((r, c));
                    }
                    else if (score == maxScore)
                        candidates.Add((r, c));
                }

        return candidates[_rnd.Next(candidates.Count)];
    }

    private int CountLinesThrough(int r, int c) =>
        r == 1 && c == 1 ? 4 : ((r == c || r + c == 2) ? 3 : 2);

    private bool CheckWin(char symbol)
    {
        bool Line(char a, char b, char c) => a == symbol && b == symbol && c == symbol;

        for (int r = 0; r < 3; r++)
            if (Line(_board[r, 0], _board[r, 1], _board[r, 2])) return true;
        for (int c = 0; c < 3; c++)
            if (Line(_board[0, c], _board[1, c], _board[2, c])) return true;
        if (Line(_board[0, 0], _board[1, 1], _board[2, 2])) return true;
        if (Line(_board[0, 2], _board[1, 1], _board[2, 0])) return true;
        return false;
    }
}

class Board
{
    private readonly char[,] _cells = new char[3, 3];

    public Board()
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                _cells[i, j] = ' ';
    }

    public char this[int row, int col]
    {
        get => _cells[row, col];
        set => _cells[row, col] = value;
    }

    public bool IsEmpty(int row, int col) => _cells[row, col] == ' ';

    public bool IsFull
    {
        get
        {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (_cells[i, j] == ' ') return false;
            return true;
        }
    }
}
