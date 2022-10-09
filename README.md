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

In root dir a `build_ogre_*` script (from [here](https://github.com/OGRECave/ogre-next/tree/master/Scripts/BuildScripts/output)) was used to get ogre-next and build it inside `Ogre/` (there is `ogre-next/` inside).  
This repo was cloned into `mygui-next/`, branch **ogre3**.

## Build

Whole build process in root dir:
```
./build_ogre_linux_c++latest.sh

git clone https://github.com/cryham/mygui-next --branch ogre3 --single-branch
cd mygui-next

mkdir build
cd build

cmake ..
make -j6
```
