#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

class Buffer {
public:
    Buffer() noexcept = default;

    explicit Buffer(std::size_t size)
        : data_(size == 0 ? nullptr : new int[size]()), size_(size) {}

    Buffer(std::initializer_list<int> values)
        : data_(values.size() == 0 ? nullptr : new int[values.size()]),
          size_(values.size()) {
        std::copy(values.begin(), values.end(), data_);
    }

    Buffer(const Buffer& other)
        : data_(other.size_ == 0 ? nullptr : new int[other.size_]),
          size_(other.size_) {
        if (size_ != 0) {
            std::copy(other.data_, other.data_ + size_, data_);
        }
    }

    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            Buffer copy(other);
            swap(copy);
        }
        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~Buffer() {
        delete[] data_;
    }

    int& operator[](std::size_t index) noexcept { return data_[index]; }
    const int& operator[](std::size_t index) const noexcept { return data_[index]; }

    int& at(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Buffer: index out of range");
        }
        return data_[index];
    }

    const int& at(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Buffer: index out of range");
        }
        return data_[index];
    }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    int* data() noexcept { return data_; }
    const int* data() const noexcept { return data_; }

    int* begin() noexcept { return data_; }
    const int* begin() const noexcept { return data_; }
    int* end() noexcept { return data_ == nullptr ? nullptr : data_ + size_; }
    const int* end() const noexcept { return data_ == nullptr ? nullptr : data_ + size_; }

    void swap(Buffer& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

private:
    int* data_ = nullptr;
    std::size_t size_ = 0;
};

inline void swap(Buffer& lhs, Buffer& rhs) noexcept {
    lhs.swap(rhs);
}

#endif
