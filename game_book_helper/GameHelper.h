#pragma once

#include "GameState.h"

#include <string>
#include <chrono>

class GameHelper
{
public:
    GameHelper();
    ~GameHelper();

    int play();

private:
    void err(std::string_view msg);
    void init(int n);
    void go(int to);
    void add(int from, int to);
    void backlog(const std::string& clue);
    void addBacklog(int from, const std::string& to);
    void clue();
    void setClue(const std::string& clue, int x);
    void memo(int id);
    void addMemo(int id, const std::string& desc);
    void show(int id, const std::string& prefix = {}, bool isLast = true);
    void showAll();
    int findRoot(int id);
    void load();

    const std::chrono::system_clock::time_point startedAt;
    std::string saveFileName;

    game_state_t state_;
};
