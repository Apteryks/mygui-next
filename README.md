![MyGUI logo](http://mygui.info/images/MyGUI_Logo.png)

MyGUI is a cross-platform library for creating graphical user interfaces (GUIs) for games and 3D applications.

* Website: http://mygui.info/
* OgreWiki: https://wiki.ogre3d.org/MyGUI


Support for Ogre-next 3.0 from master, is on [branch ogre3](https://github.com/cryham/mygui-next/tree/ogre3).

It only builds MyGUIEngine and MyGUI.Ogre2Platform, nothing more. This is enough to use MyGUI in own projects.

## Folders

Root directory structure:
* mygui-next/
* Ogre/
  * ogre-next/
  * ogre-next-deps/
* build_ogre_scripts-master.7z

In root_dir a `build_ogre_*` script (from [here](https://github.com/OGRECave/ogre-next/tree/master/Scripts/BuildScripts/output)) was used to get ogre-next and build it inside `Ogre/` (there is `ogre-next/` inside it).  
This repo was cloned into `mygui-next/`, branch **ogre3**.

## Build

### Linux

Whole build process in root dir:
```
./build_ogre_linux_c++latest.sh

git clone https://github.com/cryham/mygui-next --branch ogre3 --single-branch
cd mygui-next

mkdir build
cd build
```
Now to build (RelWithDebInfo configuration by default) here:  
```
cmake ..
make -j6
```

Or e.g. to get both Debug and Release builds, in own dirs:
```
mkdir Debug
cd Debug
cmake ./../.. -DCMAKE_BUILD_TYPE="Debug"
make -j6

cd ..
mkdir Release
cd Release
cmake ./../.. -DCMAKE_BUILD_TYPE="Release"
make -j6
```

### Windows

First build Ogre-Next using chosen `build_ogre_Visual_Studio_*.bat` in `root_dir\`.

Then in `root_dir\`, to get these sources:
```
git clone https://github.com/cryham/mygui-next --branch ogre3 --single-branch
cd mygui-next
```
Now _important_, we need to rename/replace cmake files with _Windows in name to such without.  
So e.g. rename:  
`CMake\Packages\FindFreetype_Windows.cmake` to:  
`CMake\Packages\FindFreetype.cmake`  

then rename (for Release build):  
`CMake\Packages\FindOGRE_next_WindowsRelease.cmake`  
`CMake\Packages\FindOGRE_next.cmake`  

Next start CMake-Gui app and set sources dir to `root_dir\mygui-next`  
and build dir (for Release) e.g. as `root_dir\mygui-next\build`.

Click ok to create it, then press Configure twice, press Generate.  
Open and build the created `MYGUI.sln` using VS (if that was chosen) from inside `build\Release\` dir.

When successful, it will create in `root_dir\mygui-next\build\Release\`:  
`bin\release\MyGUIEngine.dll`  
`lib\Release\MyGUIEngine.lib`  
`lib\Release\MyGUI.Ogre2Platform.lib`  

#### Debug

If you wish to build Debug config now too, then rename:  
`CMake\Packages\FindOGRE_next_WindowsDebug.cmake` to:  
`CMake\Packages\FindOGRE_next.cmake`  

Then start CMake-Gui app and use same sources dir `root_dir\mygui-next`  
but different build dir e.g. as `root_dir\mygui-next\buildDebug`.
