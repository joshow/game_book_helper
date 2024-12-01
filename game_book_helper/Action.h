#pragma once

#include "GameState.h"

#include <optional>
#include <vector>

enum class EActionCommand
{
    go,
    add,
    addBacklog,
    addClue,
    addMemo,
};

struct action_t {
    EActionCommand command;
    std::vector<std::string> params;
};

std::optional<EActionCommand> convertToCommandType(const std::string& cmdStr);

bool doAction(game_state_t& state, const action_t& action);
bool doAction_go(game_state_t& state, const std::vector<std::string>& params);
bool doAction_add(game_state_t& state, const std::vector<std::string>& params);
bool doAction_addBacklog(game_state_t& state, const std::vector<std::string>& params);
bool doAction_addClue(game_state_t& state, const std::vector<std::string>& params);
bool doAction_addMemo(game_state_t& state, const std::vector<std::string>& params);





