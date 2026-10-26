using System.Globalization;
using System.Security;
using System.Text;

namespace DirectoryListing;

internal static class Program
{
    private const int TypeWidth = 5;
    private const int DateWidth = 19;
    private const int SizeWidth = 12;

    private static int Main(string[] args)
    {
        ConfigureConsole();

        if (args.Length == 1 && (args[0] == "-h" || args[0] == "--help"))
        {
            PrintUsage(Console.Out);
            return 0;
        }

        if (args.Length != 1)
        {
            PrintUsage(Console.Error);
            return 2;
        }

        if (string.IsNullOrWhiteSpace(args[0]))
        {
            Console.Error.WriteLine("Error: the directory path must not be empty.");
            return 2;
        }

        string fullPath;

        try
        {
            fullPath = Path.GetFullPath(args[0]);
        }
        catch (Exception ex) when (
            ex is ArgumentException or NotSupportedException or PathTooLongException or SecurityException)
        {
            Console.Error.WriteLine($"Error: invalid path: {ex.Message}");
            return 1;
        }

        if (!Directory.Exists(fullPath))
        {
            if (File.Exists(fullPath))
            {
                Console.Error.WriteLine($"Error: the path is a file, not a directory: {fullPath}");
            }
            else
            {
                Console.Error.WriteLine($"Error: directory not found or inaccessible: {fullPath}");
            }

            return 1;
        }

        try
        {
            List<EntryInfo> entries = ReadEntries(fullPath);
            PrintEntries(fullPath, entries);
            PrintWarnings(entries);
            return 0;
        }
        catch (UnauthorizedAccessException)
        {
            Console.Error.WriteLine($"Error: access to the directory was denied: {fullPath}");
            return 1;
        }
        catch (DirectoryNotFoundException)
        {
            Console.Error.WriteLine($"Error: the directory disappeared while it was being read: {fullPath}");
            return 1;
        }
        catch (IOException ex)
        {
            Console.Error.WriteLine($"Error: could not read the directory: {ex.Message}");
            return 1;
        }
        catch (SecurityException ex)
        {
            Console.Error.WriteLine($"Error: access to the directory was denied: {ex.Message}");
            return 1;
        }
    }

    private static List<EntryInfo> ReadEntries(string directoryPath)
    {
        List<EntryInfo> entries = new();
        DirectoryInfo directory = new(directoryPath);

        foreach (FileSystemInfo info in directory.EnumerateFileSystemInfos())
        {
            try
            {
                FileAttributes attributes = info.Attributes;
                bool isDirectory = (attributes & FileAttributes.Directory) != 0;

                long? size = null;
                if (!isDirectory && info is FileInfo file)
                {
                    size = file.Length;
                }

                entries.Add(new EntryInfo(
                    info.Name,
                    isDirectory,
                    info.LastWriteTime,
                    size,
                    attributes,
                    null));
            }
            catch (Exception ex) when (
                ex is IOException or UnauthorizedAccessException or SecurityException)
            {
                entries.Add(new EntryInfo(
                    info.Name,
                    info is DirectoryInfo,
                    null,
                    null,
                    0,
                    ex.Message));
            }
        }

        entries.Sort(static (first, second) =>
        {
            int byType = second.IsDirectory.CompareTo(first.IsDirectory);
            if (byType != 0)
            {
                return byType;
            }

            return StringComparer.OrdinalIgnoreCase.Compare(first.Name, second.Name);
        });

        return entries;
    }

    private static void PrintEntries(string fullPath, List<EntryInfo> entries)
    {
        Console.WriteLine($"Directory: {fullPath}");

        int directories = entries.Count(entry => entry.IsDirectory);
        int files = entries.Count - directories;
        Console.WriteLine($"Entries: {entries.Count} (directories: {directories}, files: {files})");
        Console.WriteLine();

        if (entries.Count == 0)
        {
            Console.WriteLine("(the directory is empty)");
            return;
        }

        int attributesWidth = Math.Max(
            "Attributes".Length,
            entries.Max(entry => entry.Error is null
                ? FormatAttributes(entry.Attributes).Length
                : "<unavailable>".Length));

        Console.WriteLine(
            $"{"Type",-TypeWidth}  {"Last modified",-DateWidth}  {"Size",SizeWidth}  " +
            $"{"Attributes".PadRight(attributesWidth)}  Name");

        Console.WriteLine(
            $"{new string('-', TypeWidth)}  {new string('-', DateWidth)}  " +
            $"{new string('-', SizeWidth)}  {new string('-', attributesWidth)}  " +
            new string('-', 4));

        foreach (EntryInfo entry in entries)
        {
            string type = entry.IsDirectory ? "DIR" : "FILE";
            string date = entry.LastWriteTime?.ToString(
                "yyyy-MM-dd HH:mm:ss",
                CultureInfo.InvariantCulture) ?? "?";
            string size = entry.Size?.ToString(CultureInfo.InvariantCulture) ?? string.Empty;
            string attributes = entry.Error is null
                ? FormatAttributes(entry.Attributes)
                : "<unavailable>";

            Console.WriteLine(
                $"{type,-TypeWidth}  {date,-DateWidth}  {size,SizeWidth}  " +
                $"{attributes.PadRight(attributesWidth)}  {entry.Name}");
        }
    }

    private static void PrintWarnings(List<EntryInfo> entries)
    {
        List<EntryInfo> failed = entries.FindAll(entry => entry.Error is not null);

        if (failed.Count == 0)
        {
            return;
        }

        Console.Error.WriteLine();
        Console.Error.WriteLine("Warnings:");

        foreach (EntryInfo entry in failed)
        {
            Console.Error.WriteLine($"  {entry.Name}: {entry.Error}");
        }
    }

    private static string FormatAttributes(FileAttributes attributes)
    {
        if (attributes == 0 || attributes == FileAttributes.Normal)
        {
            return "Normal";
        }

        List<string> names = new();

        Add(FileAttributes.Directory, "Directory");
        Add(FileAttributes.ReadOnly, "ReadOnly");
        Add(FileAttributes.Hidden, "Hidden");
        Add(FileAttributes.System, "System");
        Add(FileAttributes.Archive, "Archive");
        Add(FileAttributes.Temporary, "Temporary");
        Add(FileAttributes.SparseFile, "SparseFile");
        Add(FileAttributes.ReparsePoint, "ReparsePoint");
        Add(FileAttributes.Compressed, "Compressed");
        Add(FileAttributes.Offline, "Offline");
        Add(FileAttributes.NotContentIndexed, "NotContentIndexed");
        Add(FileAttributes.Encrypted, "Encrypted");
        Add(FileAttributes.Device, "Device");

        return names.Count == 0 ? attributes.ToString() : string.Join(", ", names);

        void Add(FileAttributes flag, string name)
        {
            if ((attributes & flag) == flag)
            {
                names.Add(name);
            }
        }
    }

    private static void ConfigureConsole()
    {
        try
        {
            Console.OutputEncoding = new UTF8Encoding(encoderShouldEmitUTF8Identifier: false);
        }
        catch (IOException){}
        catch (SecurityException){}
    }

    private static void PrintUsage(TextWriter writer)
    {
        writer.WriteLine("Usage: DirectoryListing <directory>");
        writer.WriteLine();
        writer.WriteLine("Lists the files and immediate subdirectories of <directory>,");
        writer.WriteLine("including their last modification date, size and attributes.");
        writer.WriteLine();
        writer.WriteLine("Exit codes: 0 - success, 1 - filesystem error, 2 - invalid arguments.");
    }
}

internal sealed record EntryInfo(
    string Name,
    bool IsDirectory,
    DateTime? LastWriteTime,
    long? Size,
    FileAttributes Attributes,
    string? Error);
