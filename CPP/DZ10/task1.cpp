#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::cout << "Enter numbers count N: ";

    int n = 0;
    if (!(std::cin >> n) || n < 0) {
        std::cerr << "Error: N must be a whole number greater than or equal to 0.\n";
        return 1;
    }

    std::vector<int> numbers;
    if (n > 0) {
        std::copy_n(std::istream_iterator<int>(std::cin), static_cast<std::size_t>(n),
                    std::back_inserter(numbers));
        if (!std::cin) {
            std::cerr << "Error: Entered less than N numbers.\n";
            return 1;
        }
    }

    std::sort(numbers.begin(), numbers.end(), std::greater<int>{});

    const std::size_t sizeBefore = numbers.size();
    numbers.erase(std::unique(numbers.begin(), numbers.end()), numbers.end());
    const std::size_t removedCount = sizeBefore - numbers.size();

    std::cout << "Result: ";
    std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    std::cout << "Elements deleted: " << removedCount << '\n';

    return 0;
}
