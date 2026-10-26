namespace DungeonAdventure;

public sealed class GameWorld
{
    public const int Width = 50;
    public const int Height = 50;

    private const int ChestOpenChance = 70;
    private const int BatChaseChance = 60;

    private readonly bool[,] _walls;
    private readonly Random _rng;
    private readonly List<Room> _rooms = new();

    private bool _defending;
    private GridPoint? _vacatedFrom;

    public Player Player { get; }
    public List<Chest> Chests { get; } = new();
    public List<Enemy> Enemies { get; } = new();
    public IReadOnlyList<Room> Rooms => _rooms;
    public GridPoint ExitPosition { get; private set; }
    public int Turn { get; private set; }
    public GameStatus Status { get; private set; } = GameStatus.Running;
    public bool IsFinished => Status is not GameStatus.Running;

    public GameWorld(int? seed = null)
    {
        _rng = seed is int s ? new Random(s) : new Random();
        _walls = new bool[Width, Height];
        for (var x = 0; x < Width; x++)
            for (var y = 0; y < Height; y++)
                _walls[x, y] = true;

        Player = new Player();
        GenerateRooms();
        ConnectRooms();
        PlacePlayerAndExit();
        PlaceChests();
        PlaceEnemies();
    }

    private void GenerateRooms()
    {
        var target = _rng.Next(4, 6);
        var attempts = 0;
        while (_rooms.Count < target && attempts++ < 1000)
        {
            var room = new Room(
                X: _rng.Next(1, Width - 13),
                Y: _rng.Next(1, Height - 11),
                Width: _rng.Next(6, 13),
                Height: _rng.Next(5, 11));
            if (_rooms.Any(r => r.Overlaps(room, gap: 1)))
                continue;
            _rooms.Add(room);
        }

        attempts = 0;
        while (_rooms.Count < 4 && attempts++ < 5000)
        {
            var room = new Room(
                X: _rng.Next(1, Width - 8),
                Y: _rng.Next(1, Height - 8),
                Width: _rng.Next(4, 9),
                Height: _rng.Next(4, 9));
            if (_rooms.Any(r => r.Overlaps(room, gap: 1)))
                continue;
            _rooms.Add(room);
        }

        foreach (var room in _rooms)
            CarveRoom(room);
    }

    private void CarveRoom(Room room)
    {
        for (var x = room.X; x < room.X + room.Width; x++)
            for (var y = room.Y; y < room.Y + room.Height; y++)
                _walls[x, y] = false;
    }

    private void ConnectRooms()
    {
        for (var i = 1; i < _rooms.Count; i++)
            CarveCorridor(_rooms[i - 1].Center, _rooms[i].Center);

        if (_rooms.Count >= 3)
            CarveCorridor(_rooms[^1].Center, _rooms[0].Center);
    }

    private void CarveCorridor(GridPoint from, GridPoint to)
    {
        if (_rng.Next(2) == 0)
        {
            CarveHorizontal(from.X, to.X, from.Y);
            CarveVertical(from.Y, to.Y, to.X);
        }
        else
        {
            CarveVertical(from.Y, to.Y, from.X);
            CarveHorizontal(from.X, to.X, to.Y);
        }
    }

    private void CarveHorizontal(int x1, int x2, int y)
    {
        for (var x = Math.Min(x1, x2); x <= Math.Max(x1, x2); x++)
            _walls[x, y] = false;
    }

    private void CarveVertical(int y1, int y2, int x)
    {
        for (var y = Math.Min(y1, y2); y <= Math.Max(y1, y2); y++)
            _walls[x, y] = false;
    }

    private void PlacePlayerAndExit()
    {
        var mapCenter = new GridPoint(Width / 2, Height / 2);

        var playerRoom = _rooms.OrderBy(r => Manhattan(r.Center, mapCenter)).First();
        Player.Position = playerRoom.Center;

        ExitPosition = _rooms
            .Where(r => !r.Equals(playerRoom))
            .OrderBy(r => DistanceToBorder(r.Center))
            .First()
            .Center;
    }

    private void PlaceChests()
    {
        var chestCount = _rng.Next(4, 7);
        var contents = BuildChestContents(chestCount);

        var keyIndex = contents.FindIndex(item => item is ExitKey);
        var trapChoices = Enumerable.Range(0, chestCount).Where(i => i != keyIndex).ToList();
        var trapIndex = trapChoices[_rng.Next(trapChoices.Count)];

        for (var i = 0; i < chestCount; i++)
        {
            var position = FindFreeCell(minDistanceFromPlayer: 4);
            Chests.Add(new Chest(position, contents[i], hasTrap: i == trapIndex));
        }
    }

    private List<Item> BuildChestContents(int count)
    {
        var core = new List<Item> { new Sword(), new Spear(), new Shield(), new HealingPotion() };
        Shuffle(core);

        var othersCount = count - 1;
        var items = core.Take(othersCount).ToList();
        while (items.Count < othersCount)
            items.Add(new MoneyPile(_rng.Next(10, 31)));

        items.Add(new ExitKey());
        Shuffle(items);
        return items;
    }

    private void PlaceEnemies()
    {
        var enemyCount = _rng.Next(6, 10);
        for (var i = 0; i < enemyCount; i++)
        {
            var position = FindFreeCell(minDistanceFromPlayer: 6);
            Enemies.Add(_rng.Next(2) == 0 ? new Bat(position) : new Skeleton(position));
        }
    }

    private GridPoint FindFreeCell(int minDistanceFromPlayer)
    {
        for (var attempt = 0; attempt < 5000; attempt++)
        {
            var p = new GridPoint(_rng.Next(Width), _rng.Next(Height));
            if (!IsWalkable(p)) continue;
            if (p == ExitPosition || p == Player.Position) continue;
            if (Manhattan(p, Player.Position) < minDistanceFromPlayer) continue;
            if (Chests.Any(c => c.Position == p)) continue;
            if (Enemies.Any(e => e.Position == p)) continue;
            return p;
        }
        throw new InvalidOperationException("Failed to find a free cell for placing an object.");
    }

    public bool IsWalkable(GridPoint p) =>
        p is (>= 0 and < Width, >= 0 and < Height) && !_walls[p.X, p.Y];

    public bool IsWall(GridPoint p) => !IsWalkable(p);

    public IEnumerable<Enemy> EnemiesAt(GridPoint position) =>
        Enemies.Where(e => e is { Health: > 0 } && e.Position == position);

    public Chest? ChestAt(GridPoint position) =>
        Chests.FirstOrDefault(c => c.Position == position);

    private static int Manhattan(GridPoint a, GridPoint b) => Math.Abs(a.X - b.X) + Math.Abs(a.Y - b.Y);

    private static int DistanceToBorder(GridPoint p) =>
        Math.Min(Math.Min(p.X, p.Y), Math.Min(Width - 1 - p.X, Height - 1 - p.Y));

    private GridPoint RandomDirectionDelta() => _rng.Next(4) switch
    {
        0 => new GridPoint(-1, 0),
        1 => new GridPoint(1, 0),
        2 => new GridPoint(0, -1),
        _ => new GridPoint(0, 1),
    };

    private void Shuffle(List<Item> list)
    {
        for (var i = list.Count - 1; i > 0; i--)
        {
            var j = _rng.Next(i + 1);
            (list[i], list[j]) = (list[j], list[i]);
        }
    }

    public ActionResult BeginStep()
    {
        Turn++;
        _defending = false;
        _vacatedFrom = null;

        var log = new ActionResult();
        if (Player.Health < Player.MaxHealth && Player.FindItem<HealingPotion>() is HealingPotion potion)
        {
            Player.RemoveItem(potion);
            Player.HealFull();
            log.Say("You drank a healing potion - health fully restored!");
        }
        return log;
    }

    public ActionResult Perform(ActionKind action)
    {
        var log = action switch
        {
            ActionKind.Attack => AttackEnemies(),
            ActionKind.Defend => Defend(),
            ActionKind.MoveLeft => MovePlayer(Direction.Left),
            ActionKind.MoveRight => MovePlayer(Direction.Right),
            ActionKind.MoveUp => MovePlayer(Direction.Up),
            ActionKind.MoveDown => MovePlayer(Direction.Down),
            ActionKind.OpenChest => TryOpenChest(),
            ActionKind.OpenExit => OpenExit(),
            ActionKind.Wait => new ActionResult().Say("You wait and do nothing..."),
            _ => new ActionResult().Say("Unknown action - nothing happened."),
        };
        CheckDeath();
        return log;
    }

    private ActionResult MovePlayer(Direction direction)
    {
        var log = new ActionResult();
        var delta = direction switch
        {
            Direction.Left => new GridPoint(-1, 0),
            Direction.Right => new GridPoint(1, 0),
            Direction.Up => new GridPoint(0, -1),
            Direction.Down => new GridPoint(0, 1),
            _ => default,
        };

        var target = Player.Position + delta;

        if (target is not (>= 0 and < Width, >= 0 and < Height))
        {
            log.Say("You cannot go there - it's beyond the map boundaries.");
            return log;
        }

        if (!IsWalkable(target))
        {
            log.Say("There's a wall in front of you - cannot pass.");
            return log;
        }

        _vacatedFrom = Player.Position;
        Player.Position = target;
        log.Say($"You move to cell {target}.");
        return log;
    }

    private ActionResult AttackEnemies()
    {
        var log = new ActionResult();
        var targets = EnemiesAt(Player.Position).ToList();

        if (targets.Count is 0)
        {
            log.Say("There are no enemies here to attack.");
            return log;
        }

        foreach (var enemy in targets)
        {
            var damage = Math.Max(1, Player.EquippedWeapon.Damage + _rng.Next(-2, 3));
            enemy.Health -= damage;
            log.Say($"You strike with {Player.EquippedWeapon.Name}: {enemy.Name} takes {damage} damage " +
                    $"(HP {Math.Max(0, enemy.Health)}/{enemy.MaxHealth}).");

            if (enemy.IsAlive) continue;

            // Enemies drop random amount of money when killed (type patterns in switch expression).
            var loot = enemy switch
            {
                Bat => _rng.Next(3, 11),
                Skeleton => _rng.Next(5, 21),
                _ => _rng.Next(1, 6),
            };
            Player.Money += loot;
            log.Say($"{enemy.Name} is defeated! It drops {loot} coins.");
        }
        return log;
    }

    private ActionResult Defend()
    {
        _defending = true;
        var log = new ActionResult();
        log.Say(Player.FindItem<Shield>() is Shield
            ? "You raise your shield and take a defensive stance - incoming damage will be greatly reduced."
            : "You take a defensive stance - incoming damage will be reduced this turn.");
        return log;
    }

    private ActionResult TryOpenChest()
    {
        var log = new ActionResult();

        // Negative declaration pattern: if no chest here - exit.
        if (ChestAt(Player.Position) is not Chest chest)
        {
            log.Say("There is no chest here.");
            return log;
        }

        // Property pattern.
        if (chest is { IsOpen: true })
        {
            log.Say("The chest is already open - it's empty.");
            return log;
        }

        if (_rng.Next(100) >= ChestOpenChance)
        {
            log.Say($"The lock won't budge: chest didn't open (chance {ChestOpenChance}%). Try again.");
            return log;
        }

        chest.Open();
        log.Say("Chest opened!");

        if (chest.HasTrap)
        {
            var damage = _rng.Next(15, 26);
            Player.TakeDamage(damage);
            log.Say($"TRAP! You take {damage} damage.");
        }

        switch (chest.Contents)
        {
            case ExitKey key:
                Player.AddItem(key);
                log.Say("You found the EXIT KEY! You can now open the exit.");
                break;

            case Weapon weapon:
            {
                var previous = Player.EquippedWeapon;
                Player.AddItem(weapon);
                log.Say(Player.EquippedWeapon == previous
                    ? $"You found a weapon {weapon.Name} (damage {weapon.Damage}) - it's in your inventory."
                    : $"You found a weapon {weapon.Name} (damage {weapon.Damage}) and equipped it!");
                break;
            }

            case Shield shield:
                Player.AddItem(shield);
                log.Say("You found a shield - your defense is improved.");
                break;

            case HealingPotion potion:
                Player.AddItem(potion);
                log.Say("You found a healing potion (fully restores health).");
                break;

            case MoneyPile money:
                Player.Money += money.Amount;
                log.Say($"You found {money.Amount} coins!");
                break;

            case Item other:
                Player.AddItem(other);
                log.Say($"You found an item: {other.Name}.");
                break;

            default:
                log.Say("The chest is empty.");
                break;
        }

        return log;
    }

    private ActionResult OpenExit()
    {
        var log = new ActionResult();

        if (Player.Position != ExitPosition)
        {
            log.Say("There's no exit here - you must stand on the exit cell.");
            return log;
        }

        // Negative declaration pattern: need key.
        if (Player.FindItem<ExitKey>() is not ExitKey key)
        {
            log.Say("The exit is locked. You need the key - it's hidden in one of the chests.");
            return log;
        }

        Player.RemoveItem(key);
        Status = GameStatus.Victory;
        log.Say("You insert the key into the lock - the exit opens. VICTORY!");
        return log;
    }

    public ActionResult RunEnemyPhase()
    {
        var log = new ActionResult();
        var defending = _defending;
        _defending = false;
        var vacated = _vacatedFrom;
        _vacatedFrom = null;

        foreach (var enemy in Enemies)
        {
            // Property pattern with relational subpattern.
            if (enemy is not { Health: > 0 })
                continue;

            var inPlayerCell = enemy.Position == Player.Position;
            var chased = false;

            // Bat chases if player left its cell (declaration pattern + type pattern).
            if (!inPlayerCell && vacated is GridPoint from && enemy.Position == from &&
                enemy is Bat && _rng.Next(100) < BatChaseChance)
            {
                enemy.Position = Player.Position;
                inPlayerCell = true;
                chased = true;
            }

            if (inPlayerCell)
            {
                AttackPlayer(enemy, chased, defending, log);
            }
            else if (_rng.Next(2) == 0)
            {
                var target = enemy.Position + RandomDirectionDelta();
                if (IsWalkable(target))
                    enemy.Position = target;
            }
        }

        CheckDeath();
        if (Status is GameStatus.Defeat)
            log.Say("You have died in the depths of the dungeon...");
        return log;
    }

    private void AttackPlayer(Enemy enemy, bool halfDamage, bool defending, ActionResult log)
    {
        var damage = enemy.Damage + _rng.Next(-2, 3);

        if (halfDamage)
            damage = (damage + 1) / 2;

        if (defending)
            damage = Player.HasShield ? (damage + 3) / 4 : (damage + 1) / 2;
        else if (Player.HasShield)
            damage = Math.Max(1, damage - 2);

        if (damage is < 0)
            damage = 0;

        if (damage is 0)
        {
            log.Say($"{enemy.Name} attacks, but you block it completely.");
            return;
        }

        Player.TakeDamage(damage);
        log.Say(halfDamage
            ? $"{enemy.Name} attacks in pursuit: -{damage} health (HP {Player.Health}/{Player.MaxHealth})."
            : $"{enemy.Name} hits you: -{damage} health (HP {Player.Health}/{Player.MaxHealth}).");

        if (Player.Health > 0)
        {
            var stolen = Math.Min(Player.Money, _rng.Next(1, 11));
            if (stolen > 0)
            {
                Player.Money -= stolen;
                log.Say($"{enemy.Name} steals {stolen} coins from you!");
            }
        }
    }

    private void CheckDeath()
    {
        // Relational pattern + constant pattern (enum value).
        if (Status is GameStatus.Running && Player.Health is <= 0)
            Status = GameStatus.Defeat;
    }

    public string RenderMap()
    {
        var sb = new System.Text.StringBuilder(Width * (Height + 1));
        for (int y = 0; y < Height; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                var p = new GridPoint(x, y);
                if (p == Player.Position)
                {
                    sb.Append('@');
                    continue;
                }
                if (p == ExitPosition)
                {
                    sb.Append('X');
                    continue;
                }
                var chest = ChestAt(p);
                if (chest is not null)
                {
                    sb.Append(chest.IsOpen ? 'c' : 'C');
                    continue;
                }
                var enemies = EnemiesAt(p).ToList();
                if (enemies.Count > 0)
                {
                    sb.Append('E');
                    continue;
                }
                if (IsWall(p))
                {
                    sb.Append('#');
                }
                else
                {
                    sb.Append('.');
                }
            }
            sb.AppendLine();
        }
        return sb.ToString();
    }
}
