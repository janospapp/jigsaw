@echo off
:: This calls the lib update scripts and copies the respective dlls here.

set pwd=%cd%
set sfml_dir=C:\dev\tools\lib\SFML
set tgui_dir=C:\dev\tools\lib\TGUI
set res_dir=C:\dev\projects\Puzzle\resources\dll

:: This also updates SFML as well.
call %tgui_dir%\updateTGUI.cmd

copy %sfml_dir%\lib\sfml-system-d-2.dll %res_dir%\debug
copy %sfml_dir%\lib\sfml-graphics-d-2.dll %res_dir%\debug
copy %sfml_dir%\lib\sfml-window-d-2.dll %res_dir%\debug
copy %tgui_dir%\lib\tgui-d.dll %res_dir%\debug

copy %sfml_dir%\lib\sfml-system-2.dll %res_dir%\release 
copy %sfml_dir%\lib\sfml-graphics-2.dll %res_dir%\release 
copy %sfml_dir%\lib\sfml-window-2.dll %res_dir%\release 
copy %tgui_dir%\lib\tgui.dll %res_dir%\release
