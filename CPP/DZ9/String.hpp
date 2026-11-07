#ifndef STRING_HPP
#define STRING_HPP

#include <cstddef>
#include <iosfwd>

class String {
public:
    struct Counters {
        std::size_t copy_constructed = 0;
        std::size_t move_constructed = 0;
        std::size_t copy_assigned = 0;
        std::size_t move_assigned = 0;
    };

    String() noexcept = default;
    explicit String(const char* text);

    String(const String& other);
    String(String&& other) noexcept;

    String& operator=(const String& other);
    String& operator=(String&& other) noexcept;

    ~String();

    const char* c_str() const noexcept;
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    char& operator[](std::size_t index) noexcept { return data_[index]; }
    const char& operator[](std::size_t index) const noexcept { return data_[index]; }

    void swap(String& other) noexcept;

    static void resetCounters() noexcept;
    static Counters counters() noexcept;

private:
    char* data_ = nullptr;
    std::size_t size_ = 0;

    static Counters counters_;
};

bool operator==(const String& lhs, const String& rhs) noexcept;
bool operator!=(const String& lhs, const String& rhs) noexcept;

void swap(String& lhs, String& rhs) noexcept;

std::ostream& operator<<(std::ostream& os, const String& value);

#endif
