#pragma once

#include "shortcut.hpp"

static const string vdfShortcutsPrefix("\0shortcuts\0", 11);

struct ShortcutsFile
{
    ShortcutsFile()
    {

    }

    static ShortcutsFile fromVdf(const string& raw, size_t* index)
    {
        ShortcutsFile file;

        *index += vdfShortcutsPrefix.size();

        char c;

        while ((c = raw[(*index)++]) != '\b')
        {
            if (c != '\0')
            {
                throw runtime_error("unexpected byte '" + to_string(static_cast<int>(c)) + "' in toplevel");
            }

            string indexName;

            while ((c = raw[(*index)++]) != '\0') { indexName += c; }

            file.shortcuts[stoi(indexName)] = Shortcut::fromVdf(raw, index);
        }

        return file;
    }

    string toVdf() const
    {
        string raw = "";

        raw += vdfShortcutsPrefix;

        for (const auto& [ index, shortcut ] : shortcuts)
        {
            raw += '\0' + to_string(index) + '\0';

            raw += shortcut.toVdf();

            raw += '\b';
        }

        raw += '\b';
        // seems to need an undocumented extra backspace character as a terminator
        raw += '\b';

        return raw;
    }

    map<int, Shortcut> shortcuts;
};
