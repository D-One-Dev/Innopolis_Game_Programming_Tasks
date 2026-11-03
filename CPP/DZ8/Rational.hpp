#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <iosfwd>

class Rational {
public:
    Rational(long long numerator = 0, long long denominator = 1);

    Rational(const Rational& other) noexcept;
    Rational& operator=(const Rational& other) noexcept;
    ~Rational() = default;

    long long numerator() const noexcept { return numerator_; }
    long long denominator() const noexcept { return denominator_; }

    Rational& operator+=(const Rational& rhs);
    Rational& operator-=(const Rational& rhs);
    Rational& operator*=(const Rational& rhs);
    Rational& operator/=(const Rational& rhs);

    void swap(Rational& other) noexcept;

private:
    long long numerator_;
    long long denominator_;

    void normalize();
};

Rational operator+(Rational lhs, const Rational& rhs);
Rational operator-(Rational lhs, const Rational& rhs);
Rational operator*(Rational lhs, const Rational& rhs);
Rational operator/(Rational lhs, const Rational& rhs);

bool operator==(const Rational& lhs, const Rational& rhs) noexcept;
bool operator!=(const Rational& lhs, const Rational& rhs) noexcept;
bool operator<(const Rational& lhs, const Rational& rhs) noexcept;
bool operator>(const Rational& lhs, const Rational& rhs) noexcept;
bool operator<=(const Rational& lhs, const Rational& rhs) noexcept;
bool operator>=(const Rational& lhs, const Rational& rhs) noexcept;

std::ostream& operator<<(std::ostream& os, const Rational& value);
std::istream& operator>>(std::istream& is, Rational& value);

#endif
