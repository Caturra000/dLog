#ifndef __DLOG_SIM_HASH_H__
#define __DLOG_SIM_HASH_H__
#include <bits/stdc++.h>
namespace dlog {

// a simple implement
// http://www.caturra.cc/2020/01/28/%e5%b1%80%e9%83%a8%e6%95%8f%e6%84%9f%e7%9a%84%e5%93%88%e5%b8%8c-simhash/

class Simhash {
public:
    static Simhash& instance() {
        static Simhash simhash;
        return simhash;
    }

    bool operator()(const std::string &s1, const std::string &s2, int n = 3);
    bool operator()(const char *s1, size_t len1,
                    const char *s2, size_t len2, int n = 3);

    Simhash(const Simhash &) = delete;
    Simhash& operator=(const Simhash &) = delete;

private:
    std::array<int, 0x100> _bitmask;
    std::array<int, 0x80> _transfer;
    std::mt19937 _roll;

    void initBitmask();
    void initTransfer();
    int bitcount(unsigned int bit);
    int getFingerprint(const char *str, size_t len);
    int getFingerprint(const std::string &str);

    Simhash();
};

inline bool Simhash::operator()(const std::string &s1, const std::string &s2, int n) {
    return bitcount(getFingerprint(s1) ^ getFingerprint(s2)) < n;
}

inline bool Simhash::operator()(const char *s1, size_t len1,
                const char *s2, size_t len2, int n) {
    return bitcount(getFingerprint(s1, len1) ^ getFingerprint(s2, len2)) < n;
}

inline void Simhash::initBitmask() {
    for(int i = 0xff; i; --i) {
        if(_bitmask[i]) continue;
        for(int j = i; j; j -= j&-j) {
            ++_bitmask[i];
        }
        for(int j = i, k = _bitmask[i]; j;  j -= j&-j) {
            _bitmask[j] = k--;
        }
    }
}

inline void Simhash::initTransfer() {
    std::for_each(_transfer.begin(),_transfer.end(),
        [&](int &that) { that = _roll(); });
}

inline int Simhash::bitcount(unsigned int bit) {
    return bit ? _bitmask[bit & 0xff] + bitcount(bit>>8) : 0;
}

inline int Simhash::getFingerprint(const char *str, size_t len) {
    int fingerprint = 0;
    for(int i = 0; i < 32; ++i) {
        int iCount = 0;
        for(int j = 0; j < len; ++j) {
            int feature = str[j];
            int iWeight = _transfer[feature] >> i;
            if(iWeight &1) ++iCount;
            else --iCount;
        }
        if(iCount >= 0) fingerprint |= 1<<i;
    }
    return fingerprint;
}

inline int Simhash::getFingerprint(const std::string &str) {
    return getFingerprint(str.c_str(), str.length());
}

inline Simhash::Simhash()
    : _bitmask{0},
      _transfer{0},
      _roll{19260817} {
    initBitmask();
    initTransfer();
}

} // dlog
#endif