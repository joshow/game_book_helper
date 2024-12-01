#include "ActionHistoryManager.h"

#include <cassert>

ActionHistoryManager::ActionHistoryManager(game_state_t* state) : state_(state)
{
    assert(state != nullptr);
}

void ActionHistoryManager::reset()
{
    undoHistory_.clear();
    redoHistory_.clear();
}

void ActionHistoryManager::pushAction(const action_t& action)
{
    undoHistory_.push_front(action);
    doAction_(action);
    redoHistory_.clear();
}

bool ActionHistoryManager::undo()
{
    if (undoHistory_.empty())
    {
        return false;
    }

    undoAction_(undoHistory_.front());
    undoHistory_.splice_after(undoHistory_.begin(), redoHistory_);
    return true;
}

bool ActionHistoryManager::redo()
{
    if (redoHistory_.empty())
    {
        return false;
    }

    doAction_(redoHistory_.front());
    redoHistory_.splice_after(redoHistory_.begin(), undoHistory_);
    return true;
}
