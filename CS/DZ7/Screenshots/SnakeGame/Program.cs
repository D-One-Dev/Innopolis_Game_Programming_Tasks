namespace SnakeGame;

internal static class Program
{
    private const int BoardWidth = 40;
    private const int BoardHeight = 18;
    private const int FrameWidth = BoardWidth + 2;
    private const int BoardTop = 2;
    private const int StatusRow = BoardTop + BoardHeight + 2;
    private const int HelpRow = BoardTop + BoardHeight + 3;
    private const int MinWindowWidth = 60;
    private const int MinWindowHeight = HelpRow + 1;
    private const string HelpText = "Arrows/WASD  P: pause  R: restart  Q/Esc: quit";
    private static readonly string EmptyBoardRow = new(' ', BoardWidth);

    private static volatile bool _exitRequested;
    private static bool _cursorHidden;

    public static int Main()
    {
        if (Console.IsInputRedirected || Console.IsOutputRedirected)
        {
            Console.Error.WriteLine(
                "Snake needs an interactive terminal: stdin and stdout must not be redirected.");
            return 1;
        }

        Console.CancelKeyPress += OnCancelKeyPress;
        Game game = new(BoardWidth, BoardHeight);
        bool paused = false;

        try
        {
            HideCursor();
            Console.Clear();
            Run(game, ref paused);
        }
        finally
        {
            RestoreConsole();

            try
            {
                Console.Clear();
                Console.WriteLine("Snake closed.");
                Console.WriteLine($"Length: {game.Snake.Count}   Score: {game.Score}");
            }
            catch (IOException){}

            Console.CancelKeyPress -= OnCancelKeyPress;
        }

        return 0;
    }

    private static void Run(Game game, ref bool paused)
    {
        int lastWidth = -1;
        int lastHeight = -1;
        bool needsRender = true;
        long nextMoveAt = Environment.TickCount64 + GetMoveDelay(game);

        while (!_exitRequested)
        {
            if (!TryGetWindowSize(out int width, out int height))
            {
                _exitRequested = true;
                break;
            }

            if (width != lastWidth || height != lastHeight)
            {
                lastWidth = width;
                lastHeight = height;
                Console.Clear();
                needsRender = true;

                nextMoveAt = Environment.TickCount64 + GetMoveDelay(game);
            }

            if (ProcessInput(game, ref paused, ref nextMoveAt))
            {
                needsRender = true;
            }

            if (_exitRequested)
            {
                break;
            }

            if (width < MinWindowWidth || height < MinWindowHeight)
            {
                if (needsRender)
                {
                    RenderTooSmall(width, height);
                    needsRender = false;
                }

                Thread.Sleep(20);
                continue;
            }

            long now = Environment.TickCount64;

            if (paused)
            {
                nextMoveAt = now + GetMoveDelay(game);
            }
            else if (game.Status == GameStatus.Running && now >= nextMoveAt)
            {
                game.Step();
                nextMoveAt = now + GetMoveDelay(game);
                needsRender = true;
            }

            if (needsRender)
            {
                Render(game, paused, width);
                needsRender = false;
            }

            Thread.Sleep(10);
        }
    }

    private static bool ProcessInput(Game game, ref bool paused, ref long nextMoveAt)
    {
        bool changed = false;

        try
        {
            while (Console.KeyAvailable)
            {
                ConsoleKey key = Console.ReadKey(intercept: true).Key;

                switch (key)
                {
                    case ConsoleKey.UpArrow:
                    case ConsoleKey.W:
                        game.ChangeDirection(Direction.Up);
                        changed = true;
                        break;

                    case ConsoleKey.DownArrow:
                    case ConsoleKey.S:
                        game.ChangeDirection(Direction.Down);
                        changed = true;
                        break;

                    case ConsoleKey.LeftArrow:
                    case ConsoleKey.A:
                        game.ChangeDirection(Direction.Left);
                        changed = true;
                        break;

                    case ConsoleKey.RightArrow:
                    case ConsoleKey.D:
                        game.ChangeDirection(Direction.Right);
                        changed = true;
                        break;

                    case ConsoleKey.P:
                        if (game.Status == GameStatus.Running)
                        {
                            paused = !paused;
                            nextMoveAt = Environment.TickCount64 + GetMoveDelay(game);
                            changed = true;
                        }

                        break;

                    case ConsoleKey.R:
                        game.Reset();
                        paused = false;
                        nextMoveAt = Environment.TickCount64 + GetMoveDelay(game);
                        changed = true;
                        break;

                    case ConsoleKey.Escape:
                    case ConsoleKey.Q:
                        _exitRequested = true;
                        return changed;
                }
            }
        }
        catch (InvalidOperationException)
        {
            _exitRequested = true;
        }
        catch (IOException)
        {
            _exitRequested = true;
        }

        return changed;
    }

    private static long GetMoveDelay(Game game)
    {
        return Math.Max(70, 150 - game.Score * 6);
    }

    private static void Render(Game game, bool paused, int windowWidth)
    {
        int left = Math.Max(0, (windowWidth - FrameWidth) / 2);
        int top = BoardTop;

        const string title = "S N A K E";
        WriteAt(left + (FrameWidth - title.Length) / 2, 0, title, ConsoleColor.Yellow);

        DrawBorder(left, top);

        for (int y = 0; y < BoardHeight; y++)
        {
            WriteAt(left + 1, top + 1 + y, EmptyBoardRow, ConsoleColor.Gray);
        }

        if (game.Apple is Cell apple)
        {
            WriteCell(left, top, apple, "*", ConsoleColor.Red);
        }

        foreach (Cell cell in game.Snake)
        {
            WriteCell(left, top, cell, "o", ConsoleColor.Green);
        }

        WriteCell(left, top, game.Head, "@", ConsoleColor.Yellow);

        ClearRow(left, StatusRow, windowWidth);
        string state = paused
            ? "PAUSED"
            : game.Status switch
            {
                GameStatus.Won => "YOU WIN",
                GameStatus.Lost => "GAME OVER",
                _ => "RUNNING"
            };

        ConsoleColor stateColor = paused
            ? ConsoleColor.Yellow
            : game.Status switch
            {
                GameStatus.Won => ConsoleColor.Green,
                GameStatus.Lost => ConsoleColor.Red,
                _ => ConsoleColor.Gray
            };

        WriteAt(
            left,
            StatusRow,
            $"Length: {game.Snake.Count,2}   Score: {game.Score,3}   Status: {state}",
            stateColor);

        ClearRow(left, HelpRow, windowWidth);
        WriteAt(left, HelpRow, HelpText, ConsoleColor.DarkGray);

        if (paused)
        {
            DrawOverlay(
                left,
                top,
                ConsoleColor.Yellow,
                "PAUSED",
                "P - continue",
                "Q/Esc - quit");
        }
        else if (game.Status == GameStatus.Lost)
        {
            DrawOverlay(
                left,
                top,
                ConsoleColor.Red,
                "GAME OVER",
                $"Length: {game.Snake.Count}   Score: {game.Score}",
                "R - restart",
                "Q/Esc - quit");
        }
        else if (game.Status == GameStatus.Won)
        {
            DrawOverlay(
                left,
                top,
                ConsoleColor.Green,
                "YOU WIN!",
                $"Length: {game.Snake.Count}   Score: {game.Score}",
                "R - restart",
                "Q/Esc - quit");
        }
    }

    private static void DrawBorder(int left, int top)
    {
        string horizontal = "+" + new string('-', BoardWidth) + "+";

        WriteAt(left, top, horizontal, ConsoleColor.DarkCyan);

        for (int y = 1; y <= BoardHeight; y++)
        {
            WriteAt(left, top + y, "|", ConsoleColor.DarkCyan);
            WriteAt(left + BoardWidth + 1, top + y, "|", ConsoleColor.DarkCyan);
        }

        WriteAt(left, top + BoardHeight + 1, horizontal, ConsoleColor.DarkCyan);
    }

    private static void WriteCell(int left, int top, Cell cell, string symbol, ConsoleColor color)
    {
        WriteAt(left + 1 + cell.X, top + 1 + cell.Y, symbol, color);
    }

    private static void DrawOverlay(
        int boardLeft,
        int boardTop,
        ConsoleColor color,
        params string[] lines)
    {
        int boxWidth = lines.Max(line => line.Length) + 4;
        int boxHeight = lines.Length + 2;
        int left = boardLeft + 1 + (BoardWidth - boxWidth) / 2;
        int top = boardTop + 1 + (BoardHeight - boxHeight) / 2;

        for (int row = 0; row < boxHeight; row++)
        {
            WriteAt(left, top + row, new string(' ', boxWidth), color);
        }

        string border = "+" + new string('-', boxWidth - 2) + "+";
        WriteAt(left, top, border, color);
        WriteAt(left, top + boxHeight - 1, border, color);

        for (int index = 0; index < lines.Length; index++)
        {
            string content = "| " + lines[index].PadRight(boxWidth - 4) + " |";
            WriteAt(left, top + 1 + index, content, color);
        }
    }

    private static void RenderTooSmall(int width, int height)
    {
        Console.Clear();
        Console.ResetColor();

        string[] lines =
        {
            "Terminal window is too small.",
            $"Current size: {width} x {height}",
            $"Required size: at least {MinWindowWidth} x {MinWindowHeight}",
            "Resize the window, or press Q/Esc to quit."
        };

        int startRow = Math.Max(0, (height - lines.Length) / 2);

        for (int index = 0; index < lines.Length; index++)
        {
            int left = Math.Max(0, (width - lines[index].Length) / 2);
            WriteAt(left, startRow + index, lines[index], ConsoleColor.Yellow);
        }
    }

    private static void ClearRow(int left, int row, int windowWidth)
    {
        int length = Math.Max(0, windowWidth - left - 1);
        WriteAt(left, row, new string(' ', length), ConsoleColor.Gray);
    }

    private static void WriteAt(int left, int top, string text, ConsoleColor color)
    {
        if (left < 0 || top < 0 || top >= Console.WindowHeight || left >= Console.WindowWidth - 1)
        {
            return;
        }

        int available = Console.WindowWidth - left - 1;
        if (available <= 0)
        {
            return;
        }

        if (text.Length > available)
        {
            text = text[..available];
        }

        try
        {
            Console.SetCursorPosition(left, top);
            Console.ForegroundColor = color;
            Console.Write(text);
        }
        finally
        {
            Console.ResetColor();
        }
    }

    private static bool TryGetWindowSize(out int width, out int height)
    {
        try
        {
            width = Console.WindowWidth;
            height = Console.WindowHeight;
            return width > 0 && height > 0;
        }
        catch (IOException)
        {
            width = 0;
            height = 0;
            return false;
        }
        catch (PlatformNotSupportedException)
        {
            width = 0;
            height = 0;
            return false;
        }
    }

    private static void HideCursor()
    {
        try
        {
            Console.CursorVisible = false;
            _cursorHidden = true;
        }
        catch (IOException){}
    }

    private static void RestoreConsole()
    {
        try
        {
            if (_cursorHidden)
            {
                Console.CursorVisible = true;
            }
        }
        catch (IOException){}
        finally
        {
            Console.ResetColor();
        }
    }

    private static void OnCancelKeyPress(object? sender, ConsoleCancelEventArgs e)
    {
        e.Cancel = true;
        _exitRequested = true;
    }
}
