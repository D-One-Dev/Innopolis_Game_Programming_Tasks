#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

class Config final
{
  public:
    static Config& instance()
    {
      static Config config;
      return config;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;

    std::optional<std::string> get(const std::string& key) const
    {
      const auto it = data_.find(key);
      if (it == data_.end()) { return std::nullopt; }
      return it->second;
    }

    void set(const std::string& key, const std::string& value) { data_[key] = value; }

    bool loadFromFile(const std::string& path)
    {
      std::ifstream in(path);
      if (!in) { return false; }

      std::string line;
      while (std::getline(in, line))
      {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') { continue; }
        const std::size_t eq = line.find('=');
        if (eq == std::string::npos) { continue; }
        std::string key = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));
        if (!key.empty()) { data_[std::move(key)] = std::move(value); }
      }
      loaded_ = true;
      sourcePath_ = path;
      return true;
    }

    bool saveToFile(const std::string& path) const
    {
      std::ofstream out(path);
      if (!out) { return false; }
      for (const auto& [key, value] : data_) { out << key << '=' << value << '\n'; }
      return static_cast<bool>(out);
    }

    bool isLoaded() const { return loaded_; }
    const std::string& sourcePath() const { return sourcePath_; }
    std::size_t size() const { return data_.size(); }

    void print(std::ostream& os) const
    {
      os << "Config entries (" << data_.size() << ")";
      if (!sourcePath_.empty()) { os << " from " << sourcePath_; }
      os << ":\n";
      for (const auto& [key, value] : data_) { os << "  - " << key << " = " << value << '\n'; }
    }

  private:
    Config()
    {
      const std::string defaultPath = "config.ini";
      if (std::filesystem::exists(defaultPath)) { loadFromFile(defaultPath); }
    }

    ~Config() = default;

    static std::string trim(const std::string& s)
    {
      const std::string whitespace = " \t\r\n";
      const std::size_t begin = s.find_first_not_of(whitespace);
      if (begin == std::string::npos) { return ""; }
      const std::size_t end = s.find_last_not_of(whitespace);
      return s.substr(begin, end - begin + 1);
    }

    std::unordered_map<std::string, std::string> data_;
    bool loaded_ = false;
    std::string sourcePath_;
};

static void printLookup(const Config& cfg, const std::string& key)
{
  const auto value = cfg.get(key);
  std::cout << "get(\"" << key << "\") => ";
  if (value.has_value()) { std::cout << '"' << *value << "\"\n"; }
  else { std::cout << "<missing>\n"; }
}

int main()
{
    std::cout << "__ Task 1: Meyers Singleton Config __\n\n";

    Config& cfg1 = Config::instance();
    Config& cfg2 = Config::instance();

    std::cout << "Same instance address: " << std::boolalpha << (&cfg1 == &cfg2) << '\n';
    std::cout << "Loaded on first access: " << cfg1.isLoaded() << '\n';
    std::cout << "Source path: " << (cfg1.sourcePath().empty() ? "<none>" : cfg1.sourcePath())
              << "\n\n";

    cfg1.print(std::cout);
    std::cout << '\n';

    printLookup(cfg1, "app_name");
    printLookup(cfg1, "volume");
    printLookup(cfg1, "missing_key");

    std::cout << "\nSetting values through second reference...\n";
    cfg2.set("volume", "0.90");
    cfg2.set("difficulty", "hard");

    printLookup(cfg1, "volume");
    printLookup(cfg1, "difficulty");

    const std::string outPath = "config.generated.ini";
    const bool saved = cfg1.saveToFile(outPath);
    std::cout << "\nSaved config to " << outPath << ": " << saved << '\n';

    std::cout << "\nCopy/move/inheritance are forbidden by design:\n";
    std::cout << "  - Config(const Config&) = delete;\n";
    std::cout << "  - Config& operator=(const Config&) = delete;\n";
    std::cout << "  - Config(Config&&) = delete;\n";
    std::cout << "  - Config& operator=(Config&&) = delete;\n";
    std::cout << "  - class Config final { ... };\n";

    return 0;
}
