# Steam Static Shortcuts

An application that adds non-Steam games to Steam based on a local configuration folder. Written in C++20.

## Why?

I created this because I wanted to put lots of non-Steam games into my Steam Deck's library but couldn't find a good way to do it. Adding them in by hand takes dozens of clicks per-game and forces you to interact with Steam's file browser (which is very bad). And if you have to reset Steam or move to a new PC you have to do it all over again (or remember to preserve an obscure file hidden deep in Steam's internals).

I used [Steam ROM Manager](https://github.com/SteamGridDB/steam-rom-manager) for this in the past and it was pretty effective but it seems to be unstable: what images are available on [SteamGridDB](https://steamgriddb.com) can change from one run to the next, updates to the scanners can change the detection behavior and it broke entirely for me after I updated my years-old copy to the newest version.

I wanted something that was static and reproducible, pulling the data from a fixed set of files on my computer. It takes more effort than Steam ROM manager, the configuration folder has to be constructed by hand, but once that's done I can use that same folder with any Steam installation on any PC and instantly add the same shortcuts to my library far into the future. I can also prepare the folder on a laptop or desktop and then copy it to the Steam Deck rather than having to do everything on the Steam Deck directly.

In short: if you want an automated solution you should keep using Steam ROM Manager. If you want a manual one use this.

## Platform Support

This application only does basic file I/O operations and should work on any operating system, although by default it assumes a Linux-style Steam install and the instructions are written assuming a Linux operating system. Only a Linux binary is provided.

## Dependencies

- [yaml-cpp](https://github.com/jbeder/yaml-cpp)

## Installing

If you just want to use the application go to the Releases page and download the AppImage.

## Building

To build the application from source clone the repository and then run the following commands starting in the repository's root directory.

```sh
mkdir -p build
cd build
cmake ..
make
```

The finished executable will be created at `bin/steam-static-shortcuts`.

## Building the AppImage

To build the application from source clone the repository as before and then run this command in the repository's root directory:

```sh
./build-appimage
```

The finished AppImage will be created in the `build` directory. You must have [appimagetool](https://appimage.github.io/appimagetool/) installed for this to work. appimagetool in turn requires [desktop-file-validate](https://www.freedesktop.org/wiki/Software/desktop-file-utils/) to work.

Make sure you build the AppImage on a Linux distribution with a libc version ≤ the one currently used by SteamOS or the AppImage will fail to execute on SteamOS.

## How to Use

The application works by reading the configuration for the shortcuts you want to add to Steam from a directory on your computer. The directory structure is as follows:

```
config/
    games.yml
    art/
        hero/
            minetest.png
            the-battle-for-wesnoth.png
            xonotic.png
        grid/
            minetest.png
            the-battle-for-wesnoth.png
            xonotic.png
        icon/
            minetest.png
            the-battle-for-wesnoth.png
            xonotic.png
        logo/
            minetest.png
            the-battle-for-wesnoth.png
            xonotic.png
```

The names are **case-sensitive** and the directory names (as well as the name of `games.yml`) **must be exactly as shown here**. For each game you want to add to Steam you should supply four images. The images are optional and warnings will be printed in the console if they are absent or misnamed.  The four image categories are:

- **Icon:** This is the small icon seen in the game list on the left-hand side of the Steam library.
- **Grid:** This is the large portrait aspect-ratio image displayed in the grid in the center of the Steam library.
- **Hero:** This is the wide landscape aspect-ratio banner image displayed at the top of the Steam library when the game is selected.
- **Logo:** This is the game's logo on a transparent background. This is displayed floating on top of the hero image at the top of the Steam library when the game is selected.

The images should be supplied as `.png` with a lowercase extension. If you have [ImageMagick](https://imagemagick.org/) installed you can supply non-PNG images and then tell the application to use ImageMagick to convert them to the proper format with a special command-line argument described below. You can get community-provided images for almost any game on [SteamGridDB](https://steamgriddb.com).

Once the images have been provided the only other thing that's required is an entry describing your game in `games.yml`. The format of this entry is as follows:

```yml
- id: "minetest"
  name: "Minetest"
  command:
    - "/usr/bin/minetest"
    - "--quiet"
  workingDirectory: "/home/user"
```

The fields are as follows:

- `id`: This field is mandatory. This is a simplified version of the game's name chosen by the user which is used to identify which images to use. For example if the game's full title is `.hack//INFECTION` (many thanks to a friend for suggesting this particularly extreme example) you might use the ID `dot-hack-infection`. Then you would place the hero image at `config/art/hero/dot-hack-infection.png`. In this way you can avoid both the leading dot (which makes files invisible on Linux/MacOS) and the slashes (which are disallowed in file names). This value is only for the organization of the `config` directory and does not interact with Steam in any way.
- `name`: This field is mandatory. This is the display name of the game as seen inside Steam.
- `command`: This field is mandatory. This is the command to run when the game is launched. This is a list of strings and must always contain at least one string: the path to the executable. Further strings will be interpreted as arguments to be passed to this executable.
- `workingDirectory`: This field is optional. This is the directory the game runs in. If omitted it will default to the value of the `HOME` environment variable e.g. the user's home directory.

Once the `config` directory has been filled with all your required files you can run the application. To do this you might have to set some environment variables. The two important environment variables are:

- `STEAM_USER_ID`: This environment variable is mandatory. This is (one of the phrasings of) your Steam user ID. You can use [Steam ID Finder](https://www.steamidfinder.com/) to find out yours, where it's the long number inside the square brackets in the `steamID3` field. Alternatively you can also find this by going into your Steam directory and going to the `userdata` directory. There will be a directory inside named after your steam ID (possibly multiple if you have multiple logins).
- `STEAM_INSTALL_PATH`: This environment variable is optional. If set this will be interpreted as the path to the root of the Steam installation directory. If omitted it will default to `~/.local/share/Steam`.

Then close Steam and invoke the application like so, passing in the path to your configuration directory as the first and only argument:

```sh
export STEAM_USER_ID=1234567890
export STEAM_INSTALL_PATH=/opt/steam
./steam-static-shortcuts /home/user/Documents/config
```

Or the same thing with the AppImage:

```sh
export STEAM_USER_ID=1234567890
export STEAM_INSTALL_PATH=/opt/steam
./SteamStaticShortcuts.AppImage /home/user/Documents/config
```

If you want to use the image conversion feature mentioned above supply the special `convert-images` argument:

```sh
export STEAM_USER_ID=1234567890
export STEAM_INSTALL_PATH=/opt/steam
./steam-static-shortcuts /home/user/Documents/config convert-images
```

Once the application has finished open Steam again and your games should have been added.

You can re-run the application at any time as you add new games or modify existing ones. Updated entries are paired up with existing Steam shortcuts by their names. For example if you have a game called "Minetest" and you change the launch command, it will update the shortcut in Steam also called "Minetest". If you change the name to "Testmine" it will create a new entry on Steam called "Testmine" and you will need to manually delete the old "Minetest" entry in Steam if you don't want it anymore. Images associated with updated shortcuts will be overwritten by the new versions from the configuration directory, where applicable.

Make sure you keep the configuration directory on your PC in the same place it was when you last ran the application. Steam game icons work via paths to files on the disk and these paths point to the configuration directory.

## Known Issues

Every so often Steam adds a new field to the `shortcuts.vdf` file that this application modifies. The values stored in these new fields will be lost when this application is run because it loads only the values it knows about into internal storage to be updated before writing them back out again to the file.

If this occurs the application will print out `unknown [string|integer|mapping] field name 'NAME'`. If you see this message please report it as a bug and I will add support for the new field as soon as possible.

## Credit & License

Developed by Amini Allight. Licensed under the GPL 3.0.

This tool is not affiliated with or endorsed by Valve Corporation.
