using System.Text;

namespace DungeonAdventure;

internal static class Program
{
    private static int Main(string[] args)
    {
        try { Console.OutputEncoding = Encoding.UTF8; }
        catch {}

        int? seed = args.Length > 0 && int.TryParse(args[0], out var parsed) ? parsed : null;
        var world = new GameWorld(seed);

        PrintLine("=== DUNGEON ADVENTURE ===");
        PrintLine("50x50 map: rooms, chests (one contains the key), enemies and a locked exit.");
        PrintLine(seed is int value ? $"Seed: {value}" : "Seed: random");
        PrintLine("Type 'help' to see available actions.");
        PrintLine();

        while (!world.IsFinished)
        {
            PrintMessages(world.BeginStep());
            Describe(world);

            var action = ReadAction();
            if (action is null)
                break;

            PrintMessages(world.Perform(action.Value));

            if (!world.IsFinished)
                PrintMessages(world.RunEnemyPhase());
        }

        PrintLine();
        PrintLine(world.Status switch
        {
            GameStatus.Victory => "VICTORY! The exit is open!",
            GameStatus.Defeat => "You died... The dungeon claimed another life.",
            _ => "Game aborted.",
        });
        PrintLine($"Turns: {world.Turn} | Health: {world.Player.Health}/{Player.MaxHealth} " +
                  $"| Money: {world.Player.Money}");
        return world.Status switch
        {
            GameStatus.Victory => 0,
            GameStatus.Defeat => 2,
            _ => 1,
        };
    }

    private static void Describe(GameWorld world)
    {
        var here = world.Player.Position;
        var content = new List<string>();

        if (here == world.ExitPosition)
            content.Add("EXIT (locked, need key)");

        var chest = world.ChestAt(here);
        if (chest is { IsOpen: false })
            content.Add("chest (closed)");
        else if (chest is { IsOpen: true })
            content.Add("chest (open)");

        var enemies = world.EnemiesAt(here).ToList();
        if (enemies.Count > 0)
            content.Add("enemies: " + string.Join(", ",
                enemies.Select(e => $"{e.Name} {Math.Max(0, e.Health)}/{e.MaxHealth}")));

        if (content.Count is 0)
            content.Add("empty cell - nothing interesting");

        PrintLine($"--- Turn {world.Turn} ---");
        PrintLine($"Cell {here}: {string.Join("; ", content)}");
        PrintLine($"Health: {world.Player.Health}/{Player.MaxHealth} | " +
                  $"Money: {world.Player.Money} | " +
                  $"Weapon: {world.Player.EquippedWeapon.Name} (damage {world.Player.EquippedWeapon.Damage})");
        PrintLine($"Inventory: {(world.Player.Inventory.Count == 0 ? "empty" : string.Join(", ", world.Player.Inventory.Select(i => i.Name)))}");
        PrintLine();
        PrintLine(world.RenderMap());
    }

    private static ActionKind? ReadAction()
    {
        while (true)
        {
            Console.Write("Your action: ");
            var line = Console.ReadLine();
            if (line is null)
                return null;

            var command = line.Trim().ToLowerInvariant();
            if (command.Length is 0)
                continue;

            // Switch statement: constant patterns + combining (or) patterns.
            switch (command)
            {
                case "help" or "?" or "h":
                    PrintHelp();
                    continue;

                case "attack" or "f":
                    return ActionKind.Attack;

                case "defend" or "g":
                    return ActionKind.Defend;

                case "left" or "a":
                    return ActionKind.MoveLeft;

                case "right" or "d":
                    return ActionKind.MoveRight;

                case "up" or "w":
                    return ActionKind.MoveUp;

                case "down" or "s":
                    return ActionKind.MoveDown;

                case "open chest" or "o":
                    return ActionKind.OpenChest;

                case "wait" or "x":
                    return ActionKind.Wait;

                case "exit" or "e":
                    return ActionKind.OpenExit;

                default:
                    PrintLine("Unknown command. Type 'help' to see available actions.");
                    continue;
            }
        }
    }

    private static void PrintHelp()
    {
        PrintLine("Available actions:");
        PrintLine("  attack (f)       - attack all enemies in current cell");
        PrintLine("  defend (g)       - defend (reduces incoming damage)");
        PrintLine("  move (a/d/w/s)   - move to adjacent cell (left/right/down/up)");
        PrintLine("  open chest (o)   - try to open chest in current cell");
        PrintLine("  wait (x)         - do nothing");
        PrintLine("  exit (e)         - open exit with key (must stand on exit cell)");
        PrintLine("  help (h)         - show this help");
    }

    private static void PrintMessages(ActionResult result)
    {
        foreach (var message in result.Messages)
            PrintLine(message);
    }

    private static void PrintLine(string text = "") => Console.WriteLine(text);
}
