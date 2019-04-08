@echo off
echo Cleaning, a moment please...

rd /s /q .vs
rd /s /q debug
rd /s /q release
rd /s /q x64

attrib *.suo -s -r -h

del /f /s /q *.exp
del /f /s /q *.log
del /f /s /q *.ncb
del /f /s /q *.obj
del /f /s /q *.pdb
del /f /s /q *.sdf
del /f /s /q *.suo
del /f /s /q *.user
del /f /s /q *.vc.db

echo Cleaning done!
