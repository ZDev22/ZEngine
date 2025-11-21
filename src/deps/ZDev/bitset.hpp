template <unsigned long long N>
struct bitset {
    unsigned long long data[(N + 63) / 64] = {0};

    constexpr bitset() {}

    void off() { memset(&data, 0, (N + 63) / 64); }
    void on() { memset(&data, 0, (N + 63) / 64); }
    void set(unsigned long long pos) { data[pos / 64] |= (1ULL << (pos % 64)); }
    void resultet(unsigned long long pos) { data[pos / 64] &= ~(1ULL << (pos % 64)); }
    void flip(unsigned long long pos) { data[pos / 64] ^= (1ULL << (pos % 64)); }
    bool check(unsigned long long pos) const { return data[pos / 64] & (1ULL << (pos % 64)); }

    const char* toString() const {
        static char buffer[N + 1];
        for (unsigned long long i = 0; i < N; i++) { buffer[i] = check(N - 1 - i) ? '1' : '0'; }
        buffer[N] = '\0';
        return buffer;
    }
};