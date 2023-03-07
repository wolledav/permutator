#ifndef HASHING_H
#define HASHING_H

#include <cstddef>
#include <cstdarg>
#include <limits>
#include "util.hpp"
#include "types.hpp"

namespace hashing {

    /* key for unordered map that consists of 2 unsigned ints */
    struct pair_key {
        uint_t a;
        uint_t b;
    };

    /* hash pair key into one value */
    struct pair_hash {
        size_t operator()(const pair_key &key) const noexcept {
            ulint_t hash = key.a;
            return (hash << 16) + key.b;
        }
    };

    /* key for unordered map that consists of 4 unsingned ints */
    struct quad_key {
        uint_t a;
        uint_t b;
        uint_t c;
        uint_t d;
    };

    /* hash quad key into one value */
    struct quad_hash {
        size_t operator()(const quad_key &key) const noexcept {
            ulint_t hash = key.a;
            return (((((hash << 16) + key.b) << 16) + key.c) << 16) + key.d;
        }
    };

}

/* overloads for eqality functions for pair and quad keys*/
namespace std {
    bool operator==(const hashing::pair_key &first, const hashing::pair_key &second);
    bool operator==(const hashing::quad_key &first, const hashing::quad_key &second);
}

#endif
