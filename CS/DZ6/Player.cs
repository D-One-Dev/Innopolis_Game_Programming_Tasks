namespace DungeonAdventure;

public sealed class Player
{
    public const int MaxHealth = 100;

    public GridPoint Position { get; set; }
    public int Health { get; private set; } = MaxHealth;
    public int Money { get; set; }
    public List<Item> Inventory { get; } = new();

    public Weapon EquippedWeapon { get; private set; } = new Dagger();

    public bool HasShield => Inventory.OfType<Shield>().Any();

    public T? FindItem<T>() where T : Item => Inventory.OfType<T>().FirstOrDefault();

    public void AddItem(Item item)
    {
        Inventory.Add(item);
        // Best weapon is equipped automatically (type pattern in is expression).
        if (item is Weapon weapon && weapon.Damage > EquippedWeapon.Damage)
            EquippedWeapon = weapon;
    }

    public void RemoveItem(Item item) => Inventory.Remove(item);

    public void TakeDamage(int amount) => Health = Math.Max(0, Health - amount);

    public void HealFull() => Health = MaxHealth;
}
