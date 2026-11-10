#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace
{
    std::string toLower(const std::string& s)
    {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (std::size_t i = 0; i + 1 < result.size(); ++i)
    {
        const unsigned char c1 = static_cast<unsigned char>(result[i]);
        const unsigned char c2 = static_cast<unsigned char>(result[i + 1]);
        if (c1 == 0xD0 && c2 >= 0x90 && c2 <= 0x9F) result[i + 1] = static_cast<char>(c2 + 0x20);
        else if (c1 == 0xD0 && c2 == 0x81)
        {
            result[i] = static_cast<char>(0xD1);
            result[i + 1] = static_cast<char>(0x91);
        }
    }
    return result;
}

bool isWordChar(const std::string& s, std::size_t i)
{
    const unsigned char c = static_cast<unsigned char>(s[i]);
    if (c < 0x80) return std::isalnum(c) != 0;

    if (i + 1 < s.size())
    {
        const unsigned char c2 = static_cast<unsigned char>(s[i + 1]);
        if ((c & 0xE0) == 0xC0 && (c2 & 0xC0) == 0x80)
        {
            const unsigned codePoint = (static_cast<unsigned>(c & 0x1F) << 6) | (c2 & 0x3F);
            return (codePoint >= 0x0410 && codePoint <= 0x044F) ||
                   codePoint == 0x0401 || codePoint == 0x0451;
        }
    }
    return false;
}

std::vector<std::string> splitWords(const std::string& s)
{
    std::vector<std::string> words;
    std::string current;

    for (std::size_t i = 0; i < s.size();)
    {
        if (isWordChar(s, i))
        {
            const unsigned char c = static_cast<unsigned char>(s[i]);
            const std::size_t len = (c < 0x80) ? 1 : 2;
            current.append(s, i, len);
            i += len;
        }
        else
        {
            if (!current.empty()) words.push_back(current);
            current.clear();
            ++i;
        }
    }
    if (!current.empty()) words.push_back(current);

    return words;
}
}

int main()
{
    std::cout << "Enter text:\n";
    std::string line;
    std::getline(std::cin, line);
    if (!std::cin && line.empty())
    {
        std::cerr << "Error: could not read string.\n";
        return 1;
    }

    const std::vector<std::string> words = splitWords(toLower(line));

    std::map<std::string, int> frequency;
    std::for_each(words.begin(), words.end(),
                  [&frequency](const std::string& w) { ++frequency[w]; });

    std::vector<std::pair<std::string, int>> top(frequency.begin(), frequency.end());
    std::sort(top.begin(), top.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  if (a.second != b.second)
                      return a.second > b.second;
                  return a.first < b.first;
              });

    std::cout << "\nUnique words: " << frequency.size() << '\n';
    std::cout << "\nTop 5 most frequent words:\n";
    const std::size_t shown = std::min<std::size_t>(top.size(), 5);
    for (std::size_t i = 0; i < shown; ++i)
        std::cout << ' ' << i + 1 << ". " << top[i].first << " — " << top[i].second << '\n';

    const std::size_t onceCount =
        std::count_if(frequency.begin(), frequency.end(),
                      [](const std::pair<const std::string, int>& p) { return p.second == 1; });
    std::cout << "\nWords encountered only once (" << onceCount << "):\n";
    std::for_each(frequency.begin(), frequency.end(),
                  [](const std::pair<const std::string, int>& p) {
                      if (p.second == 1)
                          std::cout << ' ' << p.first << '\n';
                  });
    return 0;
}
