# Command Runner

<img src="src/command_runner.png" alt="Command Runner Logo" width="150"/>

Command Runner is a program for running and saving commands from a nice GUI.
Currently you need to have gnome-terminal installed to run commands in a terminal.

# Usage

First, download an AppImage from the Releases tab. Then, make sure you set it's permissions to be executable, before running it.
```sh
chmod +x ./Command_Runner-x86_64.AppImage
./Command_Runner-x86_64.AppImage
```
When you first boot it up, make sure you put it in a location where you want to keep it permanently. When the program first boots up
it will ask you if you want to create a desktop file to be able to find it in your start menu and pin it to your task manager. This stores the location
of the AppImage in the desktop file. 

When you have moved your AppImage to a different location, delete your .desktop file in your ~/.local/share/applications and re-open the program
let it create the desktopfile again. If it doesn't ask you, you can always go to File -> Install desktop file to create it.

# Building

Clone the repository with its submodules:

```sh
git clone --recurse-submodules https://github.com/StijnKitsvanHeijningen/commandrunner.git
```
Then, cd into the repo and download the linux deploy AppImage and make it executable:
```sh
cd commandrunner
curl -OL https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20240109-1/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
```
From here u can use the ./build.sh script to build the AppImage and run it.
```sh
./build.sh
./build/Command_Runner-x86_64.AppImage
```

# License
[MIT License](./LICENSE.md)
