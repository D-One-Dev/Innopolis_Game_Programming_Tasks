#include "Shape.hpp"
#include "String.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kTolerance = 1e-9;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

bool nearlyEqual(double lhs, double rhs, double tolerance = kTolerance)
{
    const double scale = std::max({1.0, std::abs(lhs), std::abs(rhs)});
    return std::abs(lhs - rhs) <= tolerance * scale;
}

template <typename Function>
bool throwsInvalidArgument(Function&& function)
{
    try
    {
        function();
    }
    catch (const std::invalid_argument&)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
    return false;
}

void copyAssign(String& target, const String& source)
{
    target = source;
}

void testShapes()
{
    std::cout << "__ Shapes __\n";

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(1.0));
    shapes.push_back(std::make_unique<Square>(2.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));

    require(nearlyEqual(shapes[0]->area(), kPi),
            "Circle area");
    require(nearlyEqual(shapes[0]->perimeter(), 2.0 * kPi),
            "Circle perimeter");
    require(nearlyEqual(shapes[1]->area(), 4.0),
            "Square area");
    require(nearlyEqual(shapes[1]->perimeter(), 8.0),
            "Square perimeter");
    require(nearlyEqual(shapes[2]->area(), 6.0),
            "Triangle area");
    require(nearlyEqual(shapes[2]->perimeter(), 12.0),
            "Triangle perimeter");

    const double expectedArea = kPi + 4.0 + 6.0;
    const double expectedPerimeter = 2.0 * kPi + 8.0 + 12.0;

    require(nearlyEqual(totalArea(shapes), expectedArea),
            "Total area of all shapes");
    require(nearlyEqual(totalPerimeter(shapes), expectedPerimeter),
            "Total perimeter of all shapes");

    std::cout << "Total area: " << totalArea(shapes) << '\n';
    std::cout << "Total perimeter: " << totalPerimeter(shapes) << '\n';

    auto clones = cloneShapes(shapes);
    require(clones.size() == shapes.size(), "Clone vector size");

    for (std::size_t i = 0; i < shapes.size(); ++i)
    {
        require(clones[i] != nullptr, "Cloned shape is not null");
        require(clones[i].get() != shapes[i].get(),
                "clone() must create a separate object");
        require(nearlyEqual(clones[i]->area(), shapes[i]->area()),
                "Cloned shape area");
        require(nearlyEqual(clones[i]->perimeter(), shapes[i]->perimeter()),
                "Cloned shape perimeter");
    }

    require(dynamic_cast<Circle*>(clones[0].get()) != nullptr,
            "Clone preserves Circle type");
    require(dynamic_cast<Square*>(clones[1].get()) != nullptr,
            "Clone preserves Square type");
    require(dynamic_cast<Triangle*>(clones[2].get()) != nullptr,
            "Clone preserves Triangle type");

    shapes.clear();
    require(nearlyEqual(totalArea(clones), expectedArea),
            "Clones survive after the original vector is destroyed");

    require(throwsInvalidArgument([] { (void)Circle(-1.0); }),
            "Negative circle radius must be rejected");
    require(throwsInvalidArgument([] { (void)Square(-2.0); }),
            "Negative square side must be rejected");
    require(throwsInvalidArgument([] { (void)Triangle(1.0, 2.0, 10.0); }),
            "Triangle inequality violation must be rejected");
    require(throwsInvalidArgument([] { (void)Triangle(0.0, 1.0, 1.0); }),
            "Zero triangle side must be rejected");
}

void testString()
{
    std::cout << "\n__ String __\n";

    static_assert(std::is_nothrow_move_constructible<String>::value,
                  "String move constructor must be noexcept");
    static_assert(std::is_nothrow_move_assignable<String>::value,
                  "String move assignment must be noexcept");
    static_assert(std::is_nothrow_destructible<String>::value,
                  "String destructor must be noexcept");

    String empty;
    require(empty.empty(), "Default String is empty");
    require(empty.size() == 0, "Default String size");
    require(std::strcmp(empty.c_str(), "") == 0, "Default String c_str");

    String hello("hello");
    require(hello.size() == 5, "String size");
    require(std::strcmp(hello.c_str(), "hello") == 0, "String c_str");

    String copy(hello);
    require(copy == hello, "Copy constructor copies the value");
    require(copy.c_str() != hello.c_str(),
            "Copy constructor allocates independent storage");

    copy[0] = 'H';
    require(hello == String("hello"), "Original String is unchanged");
    require(copy == String("Hello"), "Modified copy has the new value");

    String assigned;
    assigned = hello;
    require(assigned == hello, "Copy assignment copies the value");

    copyAssign(assigned, assigned);
    require(assigned == hello, "Self copy assignment is safe");

    String moved(std::move(copy));
    require(moved == String("Hello"), "Move constructor copies the value");
    require(copy.empty(), "Moved-from String is empty");
    require(std::strcmp(copy.c_str(), "") == 0,
            "Moved-from String has a valid c_str");

    String moveAssigned("temporary");
    moveAssigned = std::move(moved);
    require(moveAssigned == String("Hello"), "Move assignment moves the value");
    require(moved.empty(), "Move-assigned-from String is empty");

    String swappedA("A");
    String swappedB("B");
    swappedA.swap(swappedB);
    require(swappedA == String("B") && swappedB == String("A"),
            "String::swap swaps values");

    String::resetCounters();
    constexpr std::size_t count = 100;
    {
        std::vector<String> strings;
        strings.reserve(1);

        for (std::size_t i = 0; i < count; ++i)
        {
            const std::string text = "item " + std::to_string(i);
            strings.emplace_back(text.c_str());
        }

        const String::Counters stats = String::counters();
        std::cout << "On " << count << " vector insertions with reallocations: "
                  << "copy ctor = " << stats.copy_constructed
                  << ", move ctor = " << stats.move_constructed << '\n';

        require(stats.copy_constructed == 0,
                "std::vector must move elements instead of copying them "
                "because String::String(String&&) is noexcept");
        require(stats.move_constructed > 0,
                "std::vector should have moved elements during reallocation");
        require(strings.size() == count, "Vector size after insertions");

        for (std::size_t i = 0; i < count; ++i)
        {
            const std::string expected = "item " + std::to_string(i);
            require(std::strcmp(strings[i].c_str(), expected.c_str()) == 0,
                    "Vector contents are intact after reallocations");
        }
    }
}
}

int main()
{
    testShapes();
    testString();

    std::cout << "\nAll checks passed.\n";
    return 0;
}
