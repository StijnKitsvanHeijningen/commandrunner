#!/bin/bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_QT_UI=OFF  -DCMAKE_INSTALL_PREFIX=/usr 
make
make install DESTDIR=AppDir
export LDAI_UPDATE_INFORMATION="gh-releases-zsync|StijnKitsvanHeijningen|commandrunner|latest|Command_Runner-*x86_64.AppImage.zsync"
../linuxdeploy-x86_64.AppImage --appdir AppDir -i ../src/command_runner.png --output appimage -d ../src/command_runner.desktop --plugin gtk
