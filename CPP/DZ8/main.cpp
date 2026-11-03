#include "Buffer.hpp"
#include "Rational.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>

int main() {
    std::cout << "__ Rational __\n";

    const Rational half(1, 2);
    const Rational third(1, 3);

    std::cout << half << " + " << third << " = " << half + third << '\n';
    std::cout << half << " - " << third << " = " << half - third << '\n';
    std::cout << half << " * " << third << " = " << half * third << '\n';
    std::cout << half << " / " << third << " = " << half / third << '\n';

    assert(half + third == Rational(5, 6));
    assert(half - third == Rational(1, 6));
    assert(half * third == Rational(1, 6));
    assert(half / third == Rational(3, 2));

    const Rational normalized(6, -8);
    std::cout << "6/-8 = " << normalized << '\n';
    assert(normalized == Rational(-3, 4));
    assert(normalized.numerator() == -3);
    assert(normalized.denominator() == 4);

    assert(Rational(2, 4) == Rational(1, 2));
    assert(Rational(-2, 4) == Rational(-1, 2));
    assert(Rational(0, 17) == Rational(0, 1));
    assert(Rational(1, 3) < Rational(1, 2));

    Rational sum(1, 4);
    sum += Rational(1, 4);
    assert(sum == Rational(1, 2));
    std::cout << "1/4 += 1/4 -> " << sum << '\n';

    try {
        const Rational invalid(1, 0);
        (void)invalid;
        assert(false && "Creation of 1/0 should raise an exception");
    } catch (const std::invalid_argument& error) {
        std::cout << "Expected exception: " << error.what() << '\n';
    }

    try {
        const Rational result = Rational(1, 2) / Rational(0, 1);
        (void)result;
        assert(false && "Division by 0 should raise an exception");
    } catch (const std::domain_error& error) {
        std::cout << "Expected exception: " << error.what() << '\n';
    }

    std::cout << "\n__ Buffer __\n";

    Buffer original{1, 2, 3, 4};
    Buffer copy = original;

    copy[0] = 42;
    std::cout << "original[0] = " << original[0]
              << ", copy[0] = " << copy[0] << '\n';
    assert(original[0] == 1);
    assert(copy[0] == 42);

    Buffer assigned;
    assigned = original;
    assigned[1] = 99;
    assert(original[1] == 2);
    assert(assigned[1] == 99);

    Buffer moved = std::move(assigned);
    assert(moved.size() == 4);
    assert(moved[1] == 99);
    assert(assigned.empty());
    assert(assigned.data() == nullptr);

    try {
        (void)original.at(10);
        assert(false && "at() should sheck boundaries");
    } catch (const std::out_of_range& error) {
        std::cout << "Expected exception" << error.what() << '\n';
    }

    std::cout << "All tests passed.\n";
    return 0;
}
