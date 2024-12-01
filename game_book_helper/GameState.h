#pragma once

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct node_t {
    int id{};
    int parent{};
    std::set<int> childs;
    std::string desc;
};

struct game_state_t
{
    int cursor;
    std::vector<node_t> nodes;
    std::unordered_map<std::string, std::optional<int>> clues;
    std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> backlogs;
};
