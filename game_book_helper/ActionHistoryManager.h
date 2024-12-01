#pragma once

#include "Action.h"
#include "GameState.h"

#include <forward_list>
#include <string>

class ActionHistoryManager
{
public:
    // game_state_t 객체의 수명은 반드시 ActionHistoryManager 객체보다 길어야 한다.
    ActionHistoryManager(game_state_t* state);

    void reset();

    void pushAction(const action_t& action);
    bool undo();
    bool redo();

private:
    void doAction_(const action_t& action);
    void undoAction_(const action_t& action);

    game_state_t* state_;

    std::forward_list<action_t> undoHistory_;
    std::forward_list<action_t> redoHistory_;
};
