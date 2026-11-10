#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

void printVector(const std::vector<int>& v)
{
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

int main()
{
    std::cout << "Enter elements count N: ";
    int n = 0;
    if (!(std::cin >> n) || n <= 0)
    {
        std::cerr << "Error: N must be a whole number greater than 0.\n";
        return 1;
    }

    std::vector<int> v;
    std::copy_n(std::istream_iterator<int>(std::cin), static_cast<std::size_t>(n),
                std::back_inserter(v));
    if (!std::cin)
    {
        std::cerr << "Error: entered less than N numbers.\n";
        return 1;
    }

    std::cout << "Initial vector: ";
    printVector(v);

    const long long evenSum =
        std::accumulate(v.begin(), v.end(), 0LL,
                        [](long long sum, int x) { return x % 2 == 0 ? sum + x : sum; });
    std::cout << "a) Sum of even elements: " << evenSum << '\n';

    const double average =
        static_cast<double>(std::accumulate(v.begin(), v.end(), 0LL)) /
        static_cast<double>(v.size());
    const auto aboveAverage =
        std::count_if(v.begin(), v.end(), [average](int x) { return x > average; });
    std::cout << "b) Arithmetic mean: " << average
              << ", elements greater than mean: " << aboveAverage << '\n';

    std::transform(v.begin(), v.end(), v.begin(), [](int x) { return std::abs(x); });
    std::cout << "c) Vector after replacing negative elements with absolute values: ";
    printVector(v);

    const bool allPositive = std::all_of(v.begin(), v.end(), [](int x) { return x > 0; });
    std::cout << "d) All elements positive: " << (allPositive ? "yes" : "no") << '\n';

    const int maxElement = *std::max_element(v.begin(), v.end());
    std::cout << "e) Max element: " << maxElement << '\n';

    return 0;
}
