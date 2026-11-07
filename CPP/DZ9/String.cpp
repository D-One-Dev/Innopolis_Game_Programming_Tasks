#include "String.hpp"

#include <cstring>
#include <ostream>
#include <stdexcept>
#include <utility>

String::Counters String::counters_{};

String::String(const char* text) : data_(nullptr), size_(0)
{
    if (text == nullptr)
    {
        throw std::invalid_argument("String: text must not be null");
    }

    size_ = std::strlen(text);
    data_ = new char[size_ + 1];
    std::memcpy(data_, text, size_ + 1);
}

String::String(const String& other) : data_(nullptr), size_(other.size_)
{
    if (other.data_ != nullptr)
    {
        data_ = new char[size_ + 1];
        std::memcpy(data_, other.data_, size_ + 1);
    }
    else
    {
        size_ = 0;
    }
    ++counters_.copy_constructed;
}

String::String(String&& other) noexcept
    : data_(other.data_), size_(other.size_)
{
    other.data_ = nullptr;
    other.size_ = 0;
    ++counters_.move_constructed;
}

String& String::operator=(const String& other)
{
    if (this != &other)
    {
        String copy(other);
        swap(copy);
        ++counters_.copy_assigned;
    }
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (this != &other)
    {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        ++counters_.move_assigned;
    }
    return *this;
}

String::~String()
{
    delete[] data_;
}

const char* String::c_str() const noexcept
{
    return data_ != nullptr ? data_ : "";
}

void String::swap(String& other) noexcept
{
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
}

void String::resetCounters() noexcept
{
    counters_ = Counters{};
}

String::Counters String::counters() noexcept
{
    return counters_;
}

bool operator==(const String& lhs, const String& rhs) noexcept
{
    return lhs.size() == rhs.size()
        && std::memcmp(lhs.c_str(), rhs.c_str(), lhs.size()) == 0;
}

bool operator!=(const String& lhs, const String& rhs) noexcept
{
    return !(lhs == rhs);
}

void swap(String& lhs, String& rhs) noexcept
{
    lhs.swap(rhs);
}

std::ostream& operator<<(std::ostream& os, const String& value)
{
    return os << value.c_str();
}
