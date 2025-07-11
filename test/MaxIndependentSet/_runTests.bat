@echo off

if "%~1"=="" (
    set exePath="../../bin/Windows/Release/GraphOffline.exe"
) else (
    set exePath="%1"
)

cd /D "%~dp0"
set COUNTER=0

FOR /F "tokens=1-10" %%A IN (testList_full.txt) DO (
	%exePath% -%%A %%B %%C %%D %%E %%F %%G %%H %%I %%J > %%B.test
	fc %%B.test %%B.res > nul
	if errorlevel 1 @echo %%I\%%B && goto faild
	del %%B.test
	set /A COUNTER=COUNTER+1
)

cd ..

@echo OK - %COUNTER% tests
exit /B

:faild
cd ..
@echo "Faild"
exit 1