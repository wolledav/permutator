#ifndef UTIL_H
#define UITL_H

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "types.hpp"

namespace util {

    /* converts number represented as string to unsigned integer */
    uint_t convert_to_int(std::string s);

    /* prints message to stderr and exits the program */
    void throw_err(std::string msg);

    /* concatenates strings in passed vector into one string (sorts them beforehand) */
    std::string combine_strings(std::vector<std::string> strings);

    std::vector<uint_t> decode_insertions(long long dec1, long long dec2);

    std::vector<uint_t> decode_removals(long long dec);

    std::vector<uint_t> decode_ls(long long dec);
}


#endif
