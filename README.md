# Spellforce_ASI

## Compilation procedure
1) Install 32-bit MinGW on your system. Note: path to mingw should not contain spaces.
2) Add mingw32-make to you PATH variable. For Windows 10, a quick access is to enter "Edit the system environment variables" in the Start Search of Windows and click the button "Environment Variables". Change the PATH variable (double-click on it or Select and Edit), and add the path where your MinGW-w64 has been installed to e.g., C:\mingw\mingw32\bin. This folder should contain a number of .exe-files that you can see in your Explorer.
3) Run cmd. Using cd command traverse your file system to root of this repository.
4) Run mingw32-make to compile .asi file. File should be found in /bin directory.
