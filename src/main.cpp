#include "shortcuts_file.hpp"

static void convertImageFormats(const string& path)
{
    for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
    {
        string outPath = path + "/" + entry.path().stem().string() + ".png";

        if (entry.path().extension() != ".png" && !filesystem::exists(outPath))
        {
            system(format("magick \"%s\" \"%s\"",
                (entry.path().string() + (toLower(entry.path().extension()) == ".ico" ? "[-1]" : "")).c_str(),
                outPath.c_str()
            ).c_str());
        }
    }
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3 || (argc == 3 && string(argv[2]) != "convert-images"))
    {
        cerr << "usage: " << argv[0] << " config-path [convert-images]" << endl;
        return 1;
    }

    string configPath = argv[1];

    while (configPath.ends_with("/"))
    {
        configPath = configPath.substr(0, configPath.size() - 1);
    }

    if (!getenv("HOME"))
    {
        cerr << "environment variable 'HOME' is not defined, this shouldn't happen" << endl;
        return 1;
    }

    if (!getenv("STEAM_USER_ID"))
    {
        cerr << "environment variable 'STEAM_USER_ID' must be defined" << endl;
        return 1;
    }

    if (!getenv("STEAM_INSTALL_PATH"))
    {
        cerr << "environment variable 'STEAM_INSTALL_PATH' is not defined, Steam install path will be assumed to be " << getenv("HOME") << "/.local/share/Steam" << endl;
    }

    if (argc == 3)
    {
        convertImageFormats(configPath + "/" + heroPath);
        convertImageFormats(configPath + "/" + gridPath);
        convertImageFormats(configPath + "/" + logoPath);
        convertImageFormats(configPath + "/" + iconPath);
    }

    YAML::Node root = YAML::LoadFile(configPath + "/" + definitionsPath);

    vector<Game> games;

    for (const YAML::Node& game : root)
    {
        games.push_back(Game::fromYaml(game));
    }

    ShortcutsFile file;

    if (filesystem::exists(steamConfigPath() + "/shortcuts.vdf"))
    {
        size_t index = 0;

        file = ShortcutsFile::fromVdf(getFile(steamConfigPath() + "/shortcuts.vdf"), &index);
    }

    int nextIndex = -1;
    set<unsigned> appIds;

    for (const auto& [ index, shortcut ] : file.shortcuts)
    {
        nextIndex = max(nextIndex, index);
        appIds.insert(shortcut.appid);
    }

    nextIndex++;

    map<string, int> gameIdToAppId;

    for (const Game& game : games)
    {
        bool foundExisting = false;

        for (auto& [ index, shortcut ] : file.shortcuts)
        {
            if (shortcut.AppName != game.name)
            {
                continue;
            }

            cout << "updating existing entry for '" << game.name << "'" << endl;

            shortcut.update(game, configPath, appIds);

            foundExisting = true;
            gameIdToAppId[game.id] = shortcut.appid;
            break;
        }

        if (foundExisting)
        {
            continue;
        }

        cout << "creating new entry for '" << game.name << "'" << endl;

        Shortcut shortcut(game, configPath, appIds);

        file.shortcuts[nextIndex++] = shortcut;
        gameIdToAppId[game.id] = shortcut.appid;
    }

    for (const auto& [ gameId, appId ] : gameIdToAppId)
    {
        if (string path = heroInputPath(configPath, gameId); filesystem::exists(path))
        {
            filesystem::copy(path, heroOutputPath(appId), filesystem::copy_options::overwrite_existing);
        }
        else
        {
            cerr << "hero for '" << gameId << "' is missing" << endl;
        }

        if (string path = gridInputPath(configPath, gameId); filesystem::exists(path))
        {
            filesystem::copy(path, gridOutputPath(appId), filesystem::copy_options::overwrite_existing);
        }
        else
        {
            cerr << "grid for '" << gameId << "' is missing" << endl;
        }

        if (string path = logoInputPath(configPath, gameId); filesystem::exists(path))
        {
            filesystem::copy(path, logoOutputPath(appId), filesystem::copy_options::overwrite_existing);
        }
        else
        {
            cerr << "logo for '" << gameId << "' is missing" << endl;
        }
    }

    setFile(steamConfigPath() + "/shortcuts.vdf", file.toVdf());

    return 0;
}
