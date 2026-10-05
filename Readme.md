# Win-GUI-Steg

A Windows GUI application as camouflage of steganographic malware for executing malicious code. In purpose of performing security research.

Mainly aims to test the effect of steganography on antivirus evasion. 

## Prerequisites

- Visual Studio 2022
- FLTK 1.3.9
- Windows 10 or later

## Important Notes
- Project is built with Visual Studio 2022 and CMake integration
- Project uses C++14 standard only
- Dependencies are managed through VCPKG (integrated with Visual Studio)

## For Docs

1. See `docs/prompt.md` for current develop task.
2. `docs/Refer.md` has introduction of reference code. content within "0_Refer-code" should never modified.
3. `docs/design.md` is design blue print of current project.

# Function of each parts

- file purpose explains below:

    1. main.cpp should include two main part:
        a. `GUIwindow` as camouflage of stegware.
        b. `steg` Function used to perform stegnagarphy.
    the trigger or not of this function will decided at the end of project.

    2. current project uses some other feature offer by legacy code, which situated at "win-gui-steg/0_Refer-code"



# Develop Progress

1. first phase is finish GUI part. this phase will execute shellcode in binary format which 

## Testing Setup

### Prerequisites
1. Google Test Framework
2. Visual Studio 2022
3. Test images in resources/test_images/
