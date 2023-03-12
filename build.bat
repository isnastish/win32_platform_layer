@echo off

::-F      set stack size
::-Fe     renames the executable file
::-Fm     create a map file
::-I<dir> searches a directory for include files
::-w      disable all warnings
::-Od     disables optimizations
::-O1      create small code
::-O2      create fast code

set compiler_options=-Zi -nologo -FC -Fe:PlatformLayer.exe  
set linker_options=user32.lib kernel32.lib

if not exist ..\..\build (mkdir ..\..\build)

pushd ..\..\build
cl %compiler_options% ..\win32_platform_layer\code\win32.cpp /link %linker_options%
popd