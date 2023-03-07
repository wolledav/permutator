#include "hashing.hpp"

/* compare two pair keys and return true if they are equal */
bool std::operator==(const hashing::pair_key &first, const hashing::pair_key &second) {
    return (first.a == second.a) && (first.b == second.b);
}

/* compare two quad keys and return true if they are equal */
bool std::operator==(const hashing::quad_key &first, const hashing::quad_key &second) {
    return (first.a == second.a) &&
           (first.b == second.b) &&
           (first.c == second.c) &&
           (first.d == second.d);
}
