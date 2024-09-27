#pragma once

#include "types.hpp"
#include "game.hpp"
#include "tools.hpp"

#define WRITE_STRING(_x) \
raw += '\1'; \
raw += #_x; \
raw += '\0'; \
raw += _x; \
raw += '\0';

#define WRITE_QUOTED_STRING(_x) \
raw += '\1'; \
raw += #_x; \
raw += '\0'; \
if (!_x.empty() && !_x.starts_with("\"")) { raw += '"'; } \
raw += _x; \
if (!_x.empty() && !_x.ends_with("\"")) { raw += '"'; } \
raw += '\0';

#define WRITE_INT(_x) \
raw += '\2'; \
raw += #_x; \
raw += '\0'; \
raw += toVdf(_x);

struct Shortcut
{
    Shortcut()
    {

    }

    Shortcut(const Game& game, const string& configPath, const set<unsigned>& appIds)
    {
        appid = randomAppId(appIds);

        AppName = game.name;

        string path = iconInputPath(configPath, game.id);

        if (!filesystem::exists(path))
        {
            cerr << "icon for '" + game.id + "' is missing" << endl;
        }

        icon = path;
        Exe = game.command.front();

        for (size_t i = 1; i < game.command.size(); i++)
        {
            LaunchOptions += '"' + game.command[i] + '"';

            if (i + 1 != game.command.size())
            {
                LaunchOptions += ' ';
            }
        }

        if (game.workingDirectory)
        {
            StartDir = *game.workingDirectory;
        }
        else
        {
            StartDir = getenv("HOME");
        }
    }

    void update(const Game& game, const string& configPath, const set<unsigned>& appIds)
    {
        Shortcut other(game, configPath, appIds);

        if (appid == 0)
        {
            appid = randomAppId(appIds);
        }
        AppName = other.AppName;
        icon = other.icon;
        Exe = other.Exe;
        LaunchOptions = other.LaunchOptions;
        StartDir = other.StartDir;
    }

    static Shortcut fromVdf(const string& raw, size_t* index)
    {
        Shortcut shortcut;

        char c;

        while ((c = raw[(*index)++]) != '\b')
        {
            char typeChar = c;

            string fieldName;

            while ((c = raw[(*index)++]) != '\0') { fieldName += c; }

            switch (typeChar)
            {
            default :
                throw runtime_error("unexpected byte '" + to_string(static_cast<int>(typeChar)) + "' in shortcut");
            case '\0' :
                if (fieldName == "tags")
                {
                    while ((c = raw[(*index)++]) != '\b')
                    {
                        if (c != '\1')
                        {
                            throw runtime_error("unexpected byte '" + to_string(static_cast<int>(typeChar)) + "' in tags");
                        }

                        string tagName;

                        while ((c = raw[(*index)++]) != '\0') { tagName += c; }

                        string tagValue;

                        while ((c = raw[(*index)++]) != '\0') { tagValue += c; }

                        shortcut.tags[tagName] = tagValue;
                    }
                }
                else
                {
                    cerr << "unknown mapping field name '" << fieldName << "'" << endl;
                }
                break;
            case '\1' :
            {
                string fieldValue;

                while ((c = raw[(*index)++]) != '\0') { fieldValue += c; }

                if (fieldName == "AppName")
                {
                    shortcut.AppName = fieldValue;
                }
                else if (fieldName == "Exe")
                {
                    shortcut.Exe = fieldValue;
                }
                else if (fieldName == "StartDir")
                {
                    shortcut.StartDir = fieldValue;
                }
                else if (fieldName == "icon")
                {
                    shortcut.icon = fieldValue;
                }
                else if (fieldName == "ShortcutPath")
                {
                    shortcut.ShortcutPath = fieldValue;
                }
                else if (fieldName == "LaunchOptions")
                {
                    shortcut.LaunchOptions = fieldValue;
                }
                else if (fieldName == "FlatpakAppID")
                {
                    shortcut.FlatpakAppID = fieldValue;
                }
                else if (fieldName == "DevkitGameID")
                {
                    shortcut.DevkitGameID = fieldValue;
                }
                else
                {
                    cerr << "unknown string field name '" << fieldName << "'" << endl;
                }
                break;
            }
            case '\2' :
            {
                int fieldValue = *reinterpret_cast<const int*>(raw.data() + *index);
                *index += sizeof(int);

                if (fieldName == "appid")
                {
                    shortcut.appid = fieldValue;
                }
                else if (fieldName == "IsHidden")
                {
                    shortcut.IsHidden = fieldValue;
                }
                else if (fieldName == "AllowDesktopConfig")
                {
                    shortcut.AllowDesktopConfig = fieldValue;
                }
                else if (fieldName == "AllowOverlay")
                {
                    shortcut.AllowOverlay = fieldValue;
                }
                else if (fieldName == "OpenVR")
                {
                    shortcut.OpenVR = fieldValue;
                }
                else if (fieldName == "Devkit")
                {
                    shortcut.Devkit = fieldValue;
                }
                else if (fieldName == "LastPlayTime")
                {
                    shortcut.LastPlayTime = fieldValue;
                }
                else if (fieldName == "DevkitOverrideAppID")
                {
                    shortcut.DevkitOverrideAppID = fieldValue;
                }
                else
                {
                    cerr << "unknown integer field name '" << fieldName << "'" << endl;
                }
                break;
            }
            }
        }

        return shortcut;
    }

    string toVdf() const
    {
        string raw;

        WRITE_INT(appid);
        WRITE_STRING(AppName);
        WRITE_QUOTED_STRING(Exe);
        WRITE_QUOTED_STRING(StartDir);
        WRITE_STRING(icon);
        WRITE_STRING(ShortcutPath);
        WRITE_STRING(LaunchOptions);
        WRITE_INT(IsHidden);
        WRITE_INT(AllowDesktopConfig);
        WRITE_INT(AllowOverlay);
        WRITE_INT(OpenVR);
        WRITE_INT(Devkit);
        WRITE_STRING(DevkitGameID);
        WRITE_INT(LastPlayTime);

        raw += string("\0tags\0", 6);

        for (const auto& [ key, value ] : tags)
        {
            raw += '\1';
            raw += key;
            raw += '\0';
            raw += value;
            raw += '\0';
        }

        raw += "\b";

        return raw;
    }

    unsigned appid = 0;
    string AppName = "";
    string Exe = "";
    string StartDir = "";
    string icon = "";
    string ShortcutPath = "";
    string LaunchOptions = "";
    string FlatpakAppID = "";
    int IsHidden = 0;
    int AllowDesktopConfig = 1;
    int AllowOverlay = 1;
    int OpenVR = 0;
    int Devkit = 0;
    string DevkitGameID = "";
    int DevkitOverrideAppID = 0;
    int LastPlayTime = 0;
    map<string, string> tags;

private:
    string toVdf(int x) const
    {
        return string(reinterpret_cast<const char*>(&x), sizeof(int));
    }

    unsigned randomAppId(const set<unsigned>& appIds) const
    {
        random_device device;
        mt19937 engine(device());
        uniform_int_distribution<unsigned> distribution(pow<unsigned>(2, 31), pow<unsigned>(2, 32) - 1);

        unsigned appId;

        do
        {
            appId = distribution(engine);
        } while (appIds.contains(appId));

        return appId;
    }
};

#undef WRITE_INT
#undef WRITE_QUOTED_STRING
#undef WRITE_STRING
