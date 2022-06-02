conan --version

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Conan not found
    EXIT 1
)

conan config set storage.path=%CD%\..\..\.conan\data

conan install ..\conanfile.txt -g visual_studio --install-folder ..\..\.conan\x64\Debug --build=outdated -s build_type=Debug -s arch=x86_64 -s compiler="Visual Studio" -s compiler.version=17 -s compiler.runtime=MDd --update