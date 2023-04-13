@echo off

::-F      set stack size
::-Fe     renames the executable file
::-Fm     create a map file
::-I<dir> searches a directory for include files
::-w      disable all warnings
::-Od     disables optimizations
::-O1      create small code
::-O2      create fast code

set compile_options=-Zi -nologo -FC -DINTERNAL_BUILD
set link_options=user32.lib kernel32.lib opengl32.lib gdi32.lib shlwapi.lib ole32.lib

if not exist build (mkdir build)
pushd build
::Build application dll
cl %compile_options% -Fe:app.dll ..\application.cpp /LD /link opengl32.lib

::Build (win32) platform layer
cl %compile_options% -Fe:win32_app_layer.exe ..\win32.cpp /link %link_options%
popd