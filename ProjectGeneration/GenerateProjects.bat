IF NOT EXIST ..\vendor\premake-core\bin\release\premake5.exe (
    :: Change to the directory containing Bootstrap.bat
    cd /D ..\vendor\premake-core
    :: Call Bootstrap.bat from its own directory
    call Bootstrap.bat
    :: Return to the original directory
    cd /D ..\..\ProjectGeneration
    PAUSE
)

cd ..\

call  vendor\premake-core\bin\release\premake5.exe vs2022 --verbose
PAUSE