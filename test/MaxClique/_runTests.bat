@echo off

if "%~1"=="" (
    set exePath="../../bin/Windows/Release/GraphOffline.exe"
) else (
    set exePath="%1"
)

cd /D "%~dp0"

FOR /F "tokens=1-7" %%A IN (testList.txt) DO (
	%exePath% -mc %%A -report %%B > %%A.test
	fc %%A.test %%A.res > nul
	if errorlevel 1 @echo %%A && goto faild
	del %%A.test
)

cd ..

@echo OK
exit /B

:faild
cd ..
@echo "Faild"
exit 1