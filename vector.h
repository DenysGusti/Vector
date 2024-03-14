#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <initializer_list>
#include <utility>
#include <stdexcept>
#include <iostream>

// C++17 Vector uni implementation
template<class T = double>
class Vector {
public:
    class ConstIterator;

    class Iterator;

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = Iterator;
    using const_iterator = ConstIterator;

    Vector() = default;

    // no {}, garbage for speed
    Vector(const Vector<T> &other) : sz{other.sz}, max_sz{other.max_sz}, values{new value_type[other.max_sz]} {
        for (size_type i = 0; i < other.sz; ++i)
            values[i] = other.values[i];
    }

    // other than in STL vector!
    explicit Vector(const size_type n) : sz{0}, max_sz{n}, values{new value_type[n]} {}  // no {}, garbage for speed

    // no {}, garbage for speed
    Vector(const std::initializer_list<value_type> il) : sz{il.size()}, max_sz{il.size()},
                                                         values{new value_type[il.size()]} {
        size_type i = 0;
        for (const_reference el: il)
            values[i++] = el;
    }

    ~Vector() {
        delete[] values;
    }

    Vector<T> &operator=(Vector<T> other) {
        std::swap(sz, other.sz);
        std::swap(max_sz, other.max_sz);
        std::swap(values, other.values);
        return *this;
    }

    [[nodiscard]] size_type size() const {
        return sz;
    }

    [[nodiscard]] bool empty() const {
        return sz == 0;
    }

    void clear() {
        sz = 0;
    }

    void reserve(const size_type n) {
        if (n > max_sz)
            reallocate(n);
    }

    void shrink_to_fit() {
        if (sz < max_sz)
            reallocate(sz);
    }

    void push_back(const_reference el) {
        if (sz >= max_sz)
            reallocate(max_sz + max_sz / 2 + 1);
        values[sz++] = el;
    }

    void pop_back() {
        if (sz == 0)
            throw std::runtime_error{"Vector is empty"};
        --sz;
    }

    reference operator[](const size_type index) {
        if (index >= sz)
            throw std::runtime_error{"Vector out of bounds"};
        return values[index];
    }

    const_reference operator[](const size_type index) const {
        if (index >= sz)
            throw std::runtime_error{"Vector out of bounds"};
        return values[index];
    }

    [[nodiscard]] size_type capacity() const {
        return max_sz;
    }

    friend std::ostream &operator<<(std::ostream &out, const Vector<T> &vec) {
        out << '[';
        if (!vec.empty()) {
            for (size_type i = 0; i < vec.sz - 1; ++i)
                out << vec.values[i] << ", ";
            out << vec.values[vec.sz - 1];
        }
        out << ']';
        return out;
    }

    [[nodiscard]] iterator begin() {
        return iterator{values};
    }

    [[nodiscard]] iterator end() {
        return iterator{values + sz};
    }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator{values};
    }

    [[nodiscard]] const_iterator end() const {
        return const_iterator{values + sz};
    }

    iterator insert(const_iterator pos, const_reference val) {
        auto diff = pos - begin();
        if (diff < 0 || static_cast<size_type>(diff) > sz)
            throw std::runtime_error{"Iterator out of bounds"};
        auto current = static_cast<size_type>(diff);
        if (sz >= max_sz)
            reallocate(max_sz + max_sz / 2 + 1);
        for (size_t i = sz; i > current; --i)
            values[i] = values[i - 1];
        values[current] = val;
        ++sz;
        return iterator{values + current};
    }

    iterator erase(const_iterator pos) {
        auto diff = pos - begin();
        if (diff < 0 || static_cast<size_type>(diff) >= sz)
            throw std::runtime_error{"Iterator out of bounds"};
        auto current = static_cast<size_type>(diff);
        for (size_t i = current; i < sz; ++i)
            values[i] = values[i + 1];
        --sz;
        return iterator{values + current};
    }

private:
    size_type sz = 0;
    size_type max_sz = 0;
    pointer values = nullptr;

    void reallocate(const size_type new_size) {
        if (new_size < sz)
            return;

        auto tmp_array = new value_type[new_size];  // no {}, garbage for speed

        for (size_type i = 0; i < sz; ++i)
            tmp_array[i] = values[i];

        delete[] values;
        values = tmp_array;
        max_sz = new_size;
    }

public:
    class Iterator {
    public:
        using value_type = Vector<T>::value_type;
        using reference = Vector<T>::reference;
        using pointer = Vector<T>::pointer;
        using difference_type = Vector<T>::difference_type;
        using iterator_category = std::forward_iterator_tag;

        Iterator() = default;

        explicit Iterator(pointer ptr_) : ptr{ptr_} {}

        reference operator*() const {
            return *ptr;
        }

        pointer operator->() const {
            return ptr;
        }

        iterator &operator++() {
            ++ptr;
            return *this;
        }

        iterator operator++(int) { // NOLINT(*-dcl21-cpp)
            iterator old;
            operator++();
            return old;
        }

        // not global: to satisfy the std::equality_comparable; linker error!
        bool operator==(const const_iterator &other) const {
            return ptr == other.ptr;
        }

        // not global: to satisfy the std::equality_comparable; linker error!
        bool operator!=(const const_iterator &other) const {
            return ptr != other.ptr;
        }

        // we allow unintentional implicit conversions, so not explicit
        operator const_iterator() const { // NOLINT(google-explicit-constructor)
            return const_iterator{ptr};
        }

    private:
        pointer ptr = nullptr;
    };

    class ConstIterator {
    public:
        using value_type = Vector<T>::value_type;
        using reference = Vector<T>::const_reference;
        using pointer = Vector<T>::const_pointer;
        using difference_type = Vector<T>::difference_type;
        using iterator_category = std::forward_iterator_tag;

        ConstIterator() = default;

        explicit ConstIterator(pointer ptr_) : ptr{ptr_} {}

        reference operator*() const {
            return *ptr;
        }

        pointer operator->() const {
            return ptr;
        }

        bool operator==(const const_iterator &other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const const_iterator &other) const {
            return ptr != other.ptr;
        }

        const_iterator &operator++() {
            ++ptr;
            return *this;
        }

        const_iterator operator++(int) { // NOLINT(*-dcl21-cpp)
            const_iterator old;
            operator++();
            return old;
        }

        friend bool iterator::operator==(const const_iterator &other) const;

        friend bool iterator::operator!=(const const_iterator &other) const;

        friend difference_type operator-(const const_iterator &lop, const const_iterator &rop) {
            return lop.ptr - rop.ptr;
        }

    private:
        pointer ptr = nullptr;
    };
};

#endif
