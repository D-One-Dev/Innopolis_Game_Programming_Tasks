namespace DungeonAdventure;

public abstract record Item(string Name);

public abstract record Weapon(string Name, int Damage) : Item(Name);

public sealed record Dagger() : Weapon("Dagger", 5);

public sealed record Sword() : Weapon("Sword", 12);

public sealed record Spear() : Weapon("Spear", 20);

public sealed record Shield() : Item("Shield");

public sealed record HealingPotion() : Item("Healing Potion");

public sealed record ExitKey() : Item("Exit Key");

public sealed record MoneyPile(int Amount) : Item("Money");
