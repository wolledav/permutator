#ifndef TYPES_H
#define TYPES_H

/* macro functions to transform time horizons into index and vice versa */
#define TO_INDEX(N) (N - 1)
#define TO_VALUE(N) (N + 1)

/* custom type names to make code shorter */
typedef unsigned int uint_t;
typedef unsigned int uint;
typedef unsigned long int ulint_t;
typedef unsigned long long int ullint_t;
typedef double fitness_t_; /* change to float or long double to affect performance or precision */
typedef double half_fitness_t_;
typedef std::tuple<fitness_t_, uint_t, uint_t> tuple_t;

#endif
