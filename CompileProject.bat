@echo off

:: Due to an issue with my system PATH length (despite multiple supposed fixes not working), 
:: Visual Studio is still getting a compiler crash: 
::	"error MSB4018: The 'SetEnv' task failed unexpectedly. 
:: 	System.ArgumentException: Environment variable name or value is too long."
:: This script calls the UnrealBuildTool Directly, bypassing Visual Studio and MSBuild to avoid this crash.

:: ======================== !!! WARNING !!! =====================================
:: UNREAL ENGINE MUST BE CLOSED BEFORE RUNNING THIS SCRIPT!
:: (The editor locks the .dll files. Visual Studio can usually stay open).
:: ==============================================================================

echo.
echo =======================================================
echo   WARNING: MAKE SURE UNREAL ENGINE IS CLOSED!
echo =======================================================
echo.
echo Building WaveFunctionCollapseEditor...
echo.

call "C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" WaveFunctionCollapseEditor Win64 Development "C:\Users\Owner\Documents\GitHub\WaveFunctionCollapse\WaveFunctionCollapse.uproject" -waitmutex

echo.
pause