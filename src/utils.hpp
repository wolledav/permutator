#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <system_error>

#include "config.hpp"

#define LOG(x) (std::cout << x << std::endl)

using uint = unsigned int;

inline std::string itos(int i) {std::stringstream s; s << i; return s.str(); }
inline std::string uitos(uint i) {std::stringstream s; s << i; return s.str(); }

inline std::string getFilename(const std::string& file_path) {
    std::vector<std::string> split_path;
    boost::split(split_path, file_path, boost::is_any_of("/"));
    return split_path[split_path.size() - 1];
}

inline std::string generateName(const std::string& file_path, const std::string& type) {
    std::string filename = getFilename(file_path);
    return str(boost::format("%1%-%2%") % type % filename);
}

inline static nlohmann::json readJson(const std::string& path) {
    nlohmann::json data;
    std::ifstream ifs(path, std::ifstream::in);
    if (!ifs)
        throw std::system_error(ENOENT, std::system_category(), path);
    ifs >> data;
    return data;
}

struct coords {
    double x;
    double y;
};

template <typename StringType>
inline size_t levenshtein_distance(const StringType& s1, const StringType& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    if (m == 0)
        return n;
    if (n == 0)
        return m;
    std::vector<size_t> costs(n + 1);
    std::iota(costs.begin(), costs.end(), 0);
    size_t i = 0;
    for (auto c1 : s1) {
        costs[0] = i + 1;
        size_t corner = i;
        size_t j = 0;
        for (auto c2 : s2) {
            size_t upper = costs[j + 1];
            costs[j + 1] = (c1 == c2) ? corner
                : 1 + std::min(std::min(upper, corner), costs[j]);
            corner = upper;
            ++j;
        }
        ++i;
    }
    return costs[n];
}
