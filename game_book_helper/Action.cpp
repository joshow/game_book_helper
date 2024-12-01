#include "Action.h"

#include <cassert>

std::optional<EActionCommand> convertToCommandType(const std::string& cmdStr)
{
    if (cmdStr == "go")
    {
        return EActionCommand::go;
    }
    else if (cmdStr == "add")
    {
        return EActionCommand::add;
    }
    else if (cmdStr == "addBacklog")
    {
        return EActionCommand::addBacklog;
    }
    else if (cmdStr == "addclue")
    {
        return EActionCommand::addClue;
    }
    else if (cmdStr == "addmemo")
    {
        return EActionCommand::addMemo;
    }
    return std::nullopt;
}

bool doAction(game_state_t& state, const action_t& action)
{
    switch (action.command)
    {
    case EActionCommand::go:
        return doAction_go(state, action.params);
    case EActionCommand::add:
        return doAction_add(state, action.params);
    case EActionCommand::addBacklog:
        return doAction_addBacklog(state, action.params);
    case EActionCommand::addClue:
        return doAction_addClue(state, action.params);
    case EActionCommand::addMemo:
        return doAction_addMemo(state, action.params);
    };

    assert(false); // maybe new action added
    return false;
}

bool doAction_go(game_state_t& state, const std::vector<std::string>& params)
{
    return false;
}

bool doAction_add(game_state_t& state, const std::vector<std::string>& params)
{
    return false;
}

bool doAction_addBacklog(game_state_t& state, const std::vector<std::string>& params)
{
    return false;
}

bool doAction_addClue(game_state_t& state, const std::vector<std::string>& params)
{
    return false;
}

bool doAction_addMemo(game_state_t& state, const std::vector<std::string>& params)
{
    return false;
}
