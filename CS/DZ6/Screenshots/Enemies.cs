namespace DungeonAdventure;

public abstract class Enemy
{
    public string Name { get; }
    public int MaxHealth { get; }
    public int Health { get; set; }
    public int Damage { get; }
    public GridPoint Position { get; set; }

    public abstract bool Chases { get; }

    public bool IsAlive => Health > 0;

    protected Enemy(string name, int maxHealth, int damage, GridPoint position)
    {
        Name = name;
        MaxHealth = maxHealth;
        Health = maxHealth;
        Damage = damage;
        Position = position;
    }
}

public sealed class Skeleton : Enemy
{
    public Skeleton(GridPoint position) : base("Skeleton", maxHealth: 20, damage: 7, position) { }

    public override bool Chases => false;
}

public sealed class Bat : Enemy
{
    public Bat(GridPoint position) : base("Bat", maxHealth: 10, damage: 5, position) { }

    public override bool Chases => true;
}
