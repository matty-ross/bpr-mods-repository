# Burnout Paradise Remastered Mods Repository

![](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white)
![](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

This is a repository of my mods for the PC version of Burnout Paradise Remastered.


## Building
1. Clone this repository
2. Open it in Visual Studio 2022
3. Set the platform to `x86`
4. Hit `Build Solution`

## Usage
Currently, you must use Bo98's Mod Loader found [here](https://bpr.bo98.uk/).  
After installing it, run the [install.ps1](https://github.com/matty-ross/bpr-mods-repository/blob/master/install.ps1) script to copy the built DLL files into correct directories.

## Dependencies
- [Dear ImGui](https://github.com/ocornut/imgui) (docking branch)
- [Microsoft Detours](https://github.com/microsoft/Detours)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
