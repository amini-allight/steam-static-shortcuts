#pragma once

#include "types.hpp"

struct Game
{
    Game()
    {

    }

    static Game fromYaml(const YAML::Node& node)
    {
        Game game;

        game.id = node["id"].as<string>();

        if (game.id.empty())
        {
            throw runtime_error("game id cannot be empty");
        }

        game.name = node["name"].as<string>();

        if (game.name.empty())
        {
            throw runtime_error("game name cannot be empty");
        }

        game.command = node["command"].as<vector<string>>();

        if (game.command.empty())
        {
            throw runtime_error("game command cannot be empty");
        }

        if (node["workingDirectory"])
        {
            game.workingDirectory = node["workingDirectory"].as<string>();

            if (game.workingDirectory->empty())
            {
                throw runtime_error("game working directory cannot be empty");
            }
        }

        return game;
    }

    string id;
    string name;
    vector<string> command;
    optional<string> workingDirectory;
};
