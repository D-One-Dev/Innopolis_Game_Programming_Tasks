namespace DungeonAdventure;

public readonly record struct GridPoint(int X, int Y)
{
    public static GridPoint operator +(GridPoint a, GridPoint b) => new(a.X + b.X, a.Y + b.Y);

    public override string ToString() => $"({X}, {Y})";
}

public enum Direction { Left, Right, Up, Down }

public readonly record struct Room(int X, int Y, int Width, int Height)
{
    public GridPoint Center => new(X + Width / 2, Y + Height / 2);

    public bool Overlaps(Room other, int gap) =>
        X - gap < other.X + other.Width &&
        X + Width + gap > other.X &&
        Y - gap < other.Y + other.Height &&
        Y + Height + gap > other.Y;
}

public enum GameStatus { Running, Victory, Defeat }

public enum ActionKind
{
    Attack, Defend, MoveLeft, MoveRight, MoveUp, MoveDown,
    OpenChest, Wait, OpenExit, Unknown
}

public sealed class ActionResult
{
    public List<string> Messages { get; } = new();

    public ActionResult Say(string message)
    {
        Messages.Add(message);
        return this;
    }
}
