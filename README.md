# Ars Timoris

## Description

Ars Timoris is a CLI roguelike RPG with basic modding support. All data files can be found in the Data folder, and end with the .data file extension. A custom interpreted scripting language is also used for this project.

## Requirements For Building Source

### git

I'm pretty sure any version of git published since 2011 will work.

### CMake

Must support at least CMake version 3.10.0.

### C++ Compiler

Must support compiling C++ standard 23.

### C Compiler

Must support compiling C standard 17.

## Cloning

To clone the repo and obtain the source code locally do the following, depending on your OS and terminal.

### Linux (Bash)

1. Clone the project

    ```bash
    git clone --recurse-submodules https://www.github.com/OttersbachD0987/ArsTimoris
    ```

2. Go to the project directory

    ```bash
    cd ArsTimoris
    ```

### Windows (Batch)

1. Clone the project

    ```bat
    git clone --recurse-submodules https://www.github.com/OttersbachD0987/ArsTimoris
    ```

2. Go to the project directory

    ```bat
    cd ArsTimoris
    ```

### Windows (Powershell)

1. Clone the project

    ```powershell
    git clone --recurse-submodules https://www.github.com/OttersbachD0987/ArsTimoris
    ```

2. Go to the project directory

    ```powershell
    cd ArsTimoris
    ```

## Environment Variables

To build this project it depends on the following environment variables:

Name    | Value
:-------|:------
**CXX** | The path to your C++ compiler.
**CC**  | The path to your C compiler.

By default the instructions assume you do not have these set, if you do, then you can skip certain sections.

### Linux (Bash) - Environment GCC/G++

You can either run `scripts/environment.sh` or do it by hand:

```bash
export CXX=g++
export CC=gcc
```

### Windows (Batch) - Environment MSVC

You can either run `scripts/environment.bat` or do it by hand:

If you are using Visual Studio Build Tools then you must run the following to get access to MSVC:

```bat
set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
"%VCVARS%"
```

Then run the following to build it:

```bat
set CXX=cl
set CC=cl
```

### Windows (Powershell) - Environment MSVC

You can either run `scripts/environment.ps1` or do it by hand:

If you are using Visual Studio Build Tools then you must run the following to get access to MSVC:

```powershell
$env:VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
& $env:VCVARS
```

Then run the following to build it:

```powershell
$env:CXX='cl.exe'
$env:CC='cl.exe'
```

## Building

In all operating systems you will require **CMake**, and due to how I have set it up, you must have **CXX** & **CC** as environment variables pointing to your **C++** & **C** compiler respectively. Please do note, this assumes your working directory is the same as this **README.md** file, you should observe the following file structure:

```nohighlight
ArsTimoris/
├─ ArsTimoris/
│  ├─ Assets/
│  ├─ include/
│  ├─ src/
│  └─ CMakeLists.txt
├─ scripts/
│  ├─ build.bat
│  ├─ build.ps1
│  ├─ build.sh
│  ├─ debug.bat
│  ├─ debug.ps1
│  ├─ debug.sh
│  ├─ environment.bat
│  ├─ environment.ps1
│  ├─ environment.sh
│  ├─ release.bat
│  ├─ release.ps1
│  └─ release.sh
├─ vcpkg/
├─ .gitignore
├─ CMakeLists.txt
├─ CMakePresets.json
├─ README.md
└─ vcpkg.json
```

### Linux (Bash) - Build

You can either run `scripts/build.sh` or do it by hand:

```bash
cmake --workflow --preset x64-all
```

### Windows (Batch) - Build

You can either run `scripts/build.bat` or do it by hand:

```bat
cmake --workflow --preset x64-all
```

### Windows (Powershell) - Build

You can either run `scripts/build.ps1` or do it by hand:

```powershell
cmake --workflow --preset x64-all
```

***

This will build both a Debug build and a Release build, found in: `out/build/x64`.

- Debug: `out/build/x64/Debug/ArsTimoris.exe`
- Release: `out/build/x64/Release/ArsTimoris.exe`

## Running

### Linux (Bash) - Run Release

You can either run `scripts/release.sh` or do it by hand:

```bash
(cd ./out/build/x64/Release; ./ArsTimoris)
(cd ./out/build/x64/Release; ./ArsTimorisConsole)
```

***

### Linux (Bash) - Run Debug

You can either run `scripts/debug.sh` or do it by hand:

```bash
(cd ./out/build/x64/Debug; ./ArsTimoris)
(cd ./out/build/x64/Debug; ./ArsTimorisConsole)
```

***

### Windows (Batch) - Run Release

You can either run `scripts/release.bat` or do it by hand:

```bat
START "" /D ".\out\build\x64\Release" ".\out\build\x64\Release\ArsTimoris.exe"
START "" /D ".\out\build\x64\Release" ".\out\build\x64\Release\ArsTimorisConsole.exe"
```

***

### Windows (Batch) - Run Debug

You can either run `scripts/debug.bat` or do it by hand:

```bat
START "" /D ".\out\build\x64\Debug" ".\out\build\x64\Debug\ArsTimoris.exe"
START "" /D ".\out\build\x64\Debug" ".\out\build\x64\Debug\ArsTimorisConsole.exe"
```

***

### Windows (Powershell) - Run Release

You can either run `scripts/release.ps1` or do it by hand:

```powershell
Start-Process -FilePath ".\out\build\x64\Release\ArsTimoris.exe" -WorkingDirectory ".\out\build\x64\Release"
Start-Process -FilePath ".\out\build\x64\Release\ArsTimorisConsole.exe" -WorkingDirectory ".\out\build\x64\Release"
```

***

### Windows (Powershell) - Run Debug

You can either run `scripts/debug.ps1` or do it by hand:

```powershell
Start-Process -FilePath ".\out\build\x64\Debug\ArsTimoris.exe" -WorkingDirectory ".\out\build\x64\Debug"
Start-Process -FilePath ".\out\build\x64\Debug\ArsTimorisConsole.exe" -WorkingDirectory ".\out\build\x64\Debug"
```

***
