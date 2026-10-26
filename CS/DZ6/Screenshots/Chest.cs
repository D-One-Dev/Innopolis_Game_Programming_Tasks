namespace DungeonAdventure;

public sealed class Chest
{
    public GridPoint Position { get; }
    public Item Contents { get; }
    public bool HasTrap { get; }
    public bool IsOpen { get; private set; }

    public Chest(GridPoint position, Item contents, bool hasTrap)
    {
        Position = position;
        Contents = contents;
        HasTrap = hasTrap;
    }

    public void Open() => IsOpen = true;
}
