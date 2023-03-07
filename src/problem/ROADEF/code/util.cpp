#include <bitset>
#include "util.hpp"

using namespace std;

/* converts number represented as string to unsigned integer */
uint_t util::convert_to_int(string s) {
    uint_t num = 0;
    for (char c : s) {
        if (c < '0' || c > '9') throw_err("convert from string to int failed");
        num = num * 10 + c - '0';
    }
    return num;
}

/* prints message to stderr and exits the program */
void util::throw_err(string msg) {
    cerr << msg << endl;
    exit(1);
}

/* concatenates strings in passed vector into one string (sorts them beforehand) */
string util::combine_strings(vector<string> strings) {
    /* sort strings to have always same output even in different order */
    stable_sort(strings.begin(), strings.end());
    string res;
    for (string s : strings) {
        res += s;
    }
    return res;
}

std::vector<uint_t> util::decode_insertions(long long dec1, long long dec2) {
    std::vector<uint_t> bit_vector1;
    auto bs1 = bitset<24>(dec1);
    for (int i = 0; i < bs1.size(); i++) {
        bit_vector1.push_back(bs1[i]);
    }
    std::reverse(bit_vector1.begin(), bit_vector1.end());

    std::vector<uint_t> bit_vector2;
    auto bs2 = bitset<23>(dec2);
    for (int i = 0; i < bs2.size(); i++) {
        bit_vector2.push_back(bs2[i]);
    }
    std::reverse(bit_vector2.begin(), bit_vector2.end());

    bit_vector1.insert(bit_vector1.end(), bit_vector2.begin(), bit_vector2.end());

    return bit_vector1;
}

std::vector<uint_t> util::decode_removals(long long int dec) {
    auto bs = bitset<11>(dec);
    std::vector<uint_t> bit_vector(11);
    for (int i = 0; i < bs.size(); i++) {
        bit_vector[i] = bs[i];
    }
    std::reverse(bit_vector.begin(), bit_vector.end());
    return bit_vector;
}

std::vector<uint_t> util::decode_ls(long long int dec) {
    auto bs = bitset<3>(dec);
    std::vector<uint_t> bit_vector(3);
    for (int i = 0; i < bs.size(); i++) {
        bit_vector[i] = bs[i];
    }
    std::reverse(bit_vector.begin(), bit_vector.end());
    return bit_vector;
}
