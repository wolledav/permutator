//
// Created by honza on 21.11.21.
//

#pragma once

#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <boost/format.hpp>

inline std::string itos(int i) {std::stringstream s; s << i; return s.str(); }
inline std::string uitos(uint i) {std::stringstream s; s << i; return s.str(); }

inline std::string get_filename(const string& file_path) {
    std::vector<string> split_path;
    boost::split(split_path, file_path, boost::is_any_of("/"));
    return split_path[split_path.size() - 1];
}

inline std::string generate_name(const string& file_path, const string& type) {
    std::string filename = get_filename(file_path);
    return str(boost::format("%1%-%2%") % type % filename);
}
