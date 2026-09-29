#include <cstdio>

namespace groupA
{
    void print()
    {
        std::printf("groupA::print()  ->  Hello from the namespace groupA\n");
    }
}

namespace groupB
{
    void print()
    {
    std::printf("groupB::print()  ->  Hello from the namespace groupB\n");
    }
}

int main()
{
    std::printf("Explicit namespace qualification:\n");
    groupA::print();
    groupB::print();

    std::printf("\nUsing directive (namespace groupA is opened):\n");
    {
        using namespace groupA;
        print();
    }

    std::printf("\nUsing declaration (only groupB::print is imported):\n");
    {
        using groupB::print;
        print();
    }

    return 0;
}
