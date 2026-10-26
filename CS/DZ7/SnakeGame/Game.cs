namespace SnakeGame;

public enum Direction
{
    Up,
    Down,
    Left,
    Right
}

public enum GameStatus
{
    Running,
    Won,
    Lost
}

public enum StepResult
{
    Moved,
    AteApple,
    Won,
    Lost
}

public readonly record struct Cell(int X, int Y)
{
    public Cell Move(Direction direction) => direction switch
    {
        Direction.Up => new Cell(X, Y - 1),
        Direction.Down => new Cell(X, Y + 1),
        Direction.Left => new Cell(X - 1, Y),
        Direction.Right => new Cell(X + 1, Y),
        _ => throw new ArgumentOutOfRangeException(nameof(direction))
    };
}

public sealed class Game
{
    public const int InitialLength = 5;

    private readonly LinkedList<Cell> _snake = new LinkedList<Cell>();
    private readonly HashSet<Cell> _occupied = new HashSet<Cell>();
    private readonly Random _random;

    private Direction _direction;
    private Direction? _queuedDirection;

    public Game(int width, int height, Random? random = null)
    {
        if (width < InitialLength + 3)
        {
            throw new ArgumentOutOfRangeException(nameof(width), "The board is too narrow.");
        }

        if (height < 3)
        {
            throw new ArgumentOutOfRangeException(nameof(height), "The board is too short.");
        }

        Width = width;
        Height = height;
        _random = random ?? Random.Shared;
        Reset();
    }

    public int Width { get; }

    public int Height { get; }

    public IReadOnlyCollection<Cell> Snake => _snake;

    public Cell Head => _snake.First!.Value;

    public Cell? Apple { get; private set; }

    public Direction Direction => _direction;

    public GameStatus Status { get; private set; }

    public int Score => _snake.Count - InitialLength;

    public void Reset()
    {
        _snake.Clear();
        _occupied.Clear();

        int startX = Width / 2;
        int startY = Height / 2;

        for (int offset = 0; offset < InitialLength; offset++)
        {
            Cell cell = new(startX - offset, startY);
            _snake.AddLast(cell);
            _occupied.Add(cell);
        }

        _direction = Direction.Right;
        _queuedDirection = null;
        Status = GameStatus.Running;
        SpawnApple();
    }

    public void ChangeDirection(Direction direction)
    {
        if (Status != GameStatus.Running)
        {
            return;
        }

        if (IsOpposite(_direction, direction))
        {
            return;
        }

        _queuedDirection = direction;
    }

    public StepResult Step()
    {
        if (Status != GameStatus.Running)
        {
            return Status == GameStatus.Won ? StepResult.Won : StepResult.Lost;
        }

        if (_queuedDirection is not null)
        {
            _direction = _queuedDirection.Value;
            _queuedDirection = null;
        }

        Cell newHead = Head.Move(_direction);

        if (newHead.X < 0 || newHead.X >= Width || newHead.Y < 0 || newHead.Y >= Height)
        {
            Status = GameStatus.Lost;
            return StepResult.Lost;
        }

        Cell tail = _snake.Last!.Value;
        bool grows = Apple == newHead;

        bool movingIntoVacatingTail = !grows && newHead == tail;

        if (_occupied.Contains(newHead) && !movingIntoVacatingTail)
        {
            Status = GameStatus.Lost;
            return StepResult.Lost;
        }

        if (!grows)
        {
            _snake.RemoveLast();
            _occupied.Remove(tail);
        }

        _snake.AddFirst(newHead);
        _occupied.Add(newHead);

        if (!grows)
        {
            return StepResult.Moved;
        }

        SpawnApple();

        if (Apple is null)
        {
            Status = GameStatus.Won;
            return StepResult.Won;
        }

        return StepResult.AteApple;
    }

    private void SpawnApple()
    {
        int freeCells = Width * Height - _occupied.Count;

        if (freeCells <= 0)
        {
            Apple = null;
            return;
        }

        int selected = _random.Next(freeCells);

        for (int y = 0; y < Height; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                Cell cell = new(x, y);

                if (_occupied.Contains(cell))
                {
                    continue;
                }

                if (selected == 0)
                {
                    Apple = cell;
                    return;
                }

                selected--;
            }
        }

        Apple = null;
    }

    private static bool IsOpposite(Direction first, Direction second) =>
        (first == Direction.Up && second == Direction.Down) ||
        (first == Direction.Down && second == Direction.Up) ||
        (first == Direction.Left && second == Direction.Right) ||
        (first == Direction.Right && second == Direction.Left);
}
