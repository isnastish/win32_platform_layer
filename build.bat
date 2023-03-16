@echo off

::-F      set stack size
::-Fe     renames the executable file
::-Fm     create a map file
::-I<dir> searches a directory for include files
::-w      disable all warnings
::-Od     disables optimizations
::-O1      create small code
::-O2      create fast code

set compile_options=-Zi -nologo -FC -Fe:PlatformLayer.exe  
set link_options=user32.lib kernel32.lib

if not exist ..\..\build (mkdir ..\..\build)

pushd ..\..\build

::Application
cl -nologo -Zi ..\win32_platform_layer\code\application.cpp /LD /link

::Win32 Platform Layer
cl %compile_options% ..\win32_platform_layer\code\win32.cpp /link %link_options%

popd