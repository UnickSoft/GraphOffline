@echo off

set exePath="../../bin/Windows/Release/GraphOffline.exe"

cd /D "%~dp0"

FOR /F "tokens=1-4" %%A IN (testList.txt) DO (
	%exePath% %%A %%B -start %%C -finish %%D > %%B.test
	fc %%B.test %%B.res > nul
	if errorlevel 1 @echo %%I\%%B && goto faild
	del %%B.test
)

cd ..

@echo OK
exit

:faild
cd ..
@echo "Faild"
exit 1