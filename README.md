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
Before you start Command Runner for the first time, make sure you put it in the correct location. On first startup Command Runner
will ask you if it can create a .desktop file so you can find Command Runner in your start menu and pin it to your task manager. This stores the current location
of the AppImage in the .desktop file. 

When you have moved your AppImage to a different location, delete your .desktop file in your ~/.local/share/applications folder and re-open the program to
let it create the .desktop file again. Alternatively, you can go to "File-> Install desktop file" to install the .desktop file.

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
