#pragma once
#include <initializer_list>

template <typename T>
class vector {
public:
    vector() : m_data(nullptr), m_size(0), m_capacity(0) {}

    explicit vector(unsigned long long n) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (n > 0) {
            m_capacity = m_size = n;
            m_data = new T[n];
            for (unsigned long long i = 0; i < n; ++i) new (m_data + i) T();
        }
    }

    vector(const T* arr, unsigned long long count) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (count > 0) {
            m_capacity = m_size = count;
            m_data = new T[count];
            for (unsigned long long i = 0; i < count; ++i) new (m_data + i) T(arr[i]);
        }
    }

    vector(std::initializer_list<T> init) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (init.size() > 0) {
            m_capacity = m_size = init.size();
            m_data = new T[m_capacity];
            unsigned long long i = 0;
            for (const T& value : init) {
                new (m_data + i) T(value);
                ++i;
            }
        }
    }

    vector(const vector& other) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (other.m_size > 0) {
            m_capacity = m_size = other.m_size;
            m_data = new T[m_capacity];
            for (unsigned long long i = 0; i < m_size; ++i) new (m_data + i) T(other.m_data[i]);
        }
    }

    vector(vector&& o) noexcept : m_data(o.m_data), m_size(o.m_size), m_capacity(o.m_capacity) {
        o.m_data = nullptr; o.m_size = o.m_capacity = 0;
    }

    ~vector() {
        for (unsigned long long i = 0; i < m_size; ++i) m_data[i].~T();
        delete[] m_data;
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            clear();
            if (other.m_size > 0) {
                m_capacity = m_size = other.m_size;
                m_data = new T[m_capacity];
                for (unsigned long long i = 0; i < m_size; ++i) new (m_data + i) T(other.m_data[i]);
            }
        }
        return *this;
    }

    vector& operator=(vector&& o) noexcept {
        if (this != &o) {
            clear();
            m_data = o.m_data; m_size = o.m_size; m_capacity = o.m_capacity;
            o.m_data = nullptr; o.m_size = o.m_capacity = 0;
        }
        return *this;
    }

    vector& operator=(std::initializer_list<T> init) {
        clear();
        if (init.size() > 0) {
            m_capacity = m_size = init.size();
            m_data = new T[m_capacity];
            unsigned long long i = 0;
            for (const T& value : init) {
                new (m_data + i) T(value);
                ++i;
            }
        }
        return *this;
    }

    void reserve(unsigned long long newCap) {
        if (newCap <= m_capacity) return;
        T* newBuf = new T[newCap];
        for (unsigned long long i = 0; i < m_size; ++i) {
            new (newBuf + i) T(static_cast<T&&>(m_data[i]));
            m_data[i].~T();
        }
        delete[] m_data;
        m_data = newBuf;
        m_capacity = newCap;
    }

    void push_back(const T& value) {
        if (m_size >= m_capacity) reserve(m_capacity ? m_capacity * 3 / 2 : 1);
        new (m_data + m_size) T(value);
        ++m_size;
    }

    void push_back(T&& value) {
        if (m_size >= m_capacity) reserve(m_capacity ? m_capacity * 3 / 2 : 1);
        new (m_data + m_size) T(static_cast<T&&>(value));
        ++m_size;
    }

    T* erase(T* pos) {
        if (pos < m_data || pos >= m_data + m_size) return end();
        pos->~T();
        for (T* it = pos + 1; it != m_data + m_size; ++it) {
            new (it - 1) T(static_cast<T&&>(*it));
            it->~T();
        }
        --m_size;
        return pos;
    }

    void erase(unsigned long long index) {
        if (index >= m_size) return;
        erase(m_data + index);
    }

    void clear() {
        for (unsigned long long i = 0; i < m_size; ++i) m_data[i].~T();
        delete[] m_data;
        m_data = nullptr; m_size = m_capacity = 0;
    }

    void resize(unsigned long long new_size) {
        if (new_size < m_size) {
            for (unsigned long long i = new_size; i < m_size; ++i) m_data[i].~T();
            m_size = new_size;
        } 
        else if (new_size > m_size) {
            reserve(new_size);
            for (unsigned long long i = m_size; i < new_size; ++i) new (m_data + i) T();
            m_size = new_size;
        }
    }

    void resize(unsigned long long new_size, const T& value) {
        if (new_size < m_size) {
            for (unsigned long long i = new_size; i < m_size; ++i) m_data[i].~T();
            m_size = new_size;
        } 
        else if (new_size > m_size) {
            reserve(new_size);
            for (unsigned long long i = m_size; i < new_size; ++i) new (m_data + i) T(value);
            m_size = new_size;
        }
    }

    bool empty() const { return m_size == 0; }

    constexpr unsigned long long size() const { return m_size; }
    constexpr T& operator[](unsigned long long i) { return m_data[i]; }
    constexpr const T& operator[](unsigned long long i) const { return m_data[i]; }
    constexpr T* data() { return m_data; }
    constexpr const T* data() const { return m_data; }

    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    const T* begin() const { return m_data; }
    const T* end() const { return m_data + m_size; }

private:
    T* m_data;
    unsigned long long m_size;
    unsigned long long m_capacity;
};