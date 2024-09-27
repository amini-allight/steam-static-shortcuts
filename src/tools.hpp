#pragma once

#include "constants.hpp"

static string getFile(const string& path)
{
    ifstream file(path);

    if (!file.good())
    {
        throw runtime_error("file '" + path + "' not found");
    }

    return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

static void setFile(const string& path, const string& data)
{
    ofstream file(path);

    file << data;
}

static string steamConfigPath()
{
    const char* steamInstallPathOverride = getenv("STEAM_INSTALL_PATH");

    string steamInstallPath;

    if (steamInstallPathOverride)
    {
        steamInstallPath = steamInstallPathOverride;
    }
    else
    {
        steamInstallPath = string(getenv("HOME")) + "/.local/share/Steam";
    }

    return steamInstallPath + "/userdata/" + string(getenv("STEAM_USER_ID")) + "/config";
}

static string iconInputPath(const string& configPath, const string& id)
{
    return filesystem::absolute(configPath).string() + "/" + iconPath + "/" + id + ".png";
}

static string heroInputPath(const string& configPath, const string& id)
{
    return filesystem::absolute(configPath).string() + "/" + heroPath + "/" + id + ".png";
}

static string gridInputPath(const string& configPath, const string& id)
{
    return filesystem::absolute(configPath).string() + "/" + gridPath + "/" + id + ".png";
}

static string logoInputPath(const string& configPath, const string& id)
{
    return filesystem::absolute(configPath).string() + "/" + logoPath + "/" + id + ".png";
}

static string heroOutputPath(unsigned appId)
{
    return steamConfigPath() + "/grid/" + to_string(appId) + "_hero.png";
}

static string logoOutputPath(unsigned appId)
{
    return steamConfigPath() + "/grid/" + to_string(appId) + "_logo.png";
}

static string gridOutputPath(unsigned appId)
{
    return steamConfigPath() + "/grid/" + to_string(appId) + "p.png";
}

template<typename... Args>
string format(const char* format, Args... args)
{
    int size = snprintf(nullptr, 0, format, args...) + 1;

    char* buffer = new char[size];

    snprintf(buffer, size, format, args...);

    string result(buffer);

    delete[] buffer;

    return result;
}

static string toLower(string s)
{
    for (char& c : s)
    {
        c = tolower(c);
    }

    return s;
}
