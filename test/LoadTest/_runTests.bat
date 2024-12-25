@echo off

if "%~1"=="" (
    set exePath="../../bin/Windows/Release/GraphOffline.exe"
) else (
    set exePath="%1"
)

cd /D "%~dp0"

FOR /F "tokens=1-3" %%A IN (testList.txt) DO (
	%exePath% %%A %%B -report %%C > %%B.test
	fc %%B.test %%B.res > nul
	if errorlevel 1 @echo %%B && goto faild
	del %%B.test
)

cd ..

@echo OK
exit /B

:faild
cd ..
@echo "Faild"
exit 1