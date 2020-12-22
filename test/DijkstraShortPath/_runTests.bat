@echo off

set exePath="../../bin/Windows/Release/GraphOffline.exe"

cd /D "%~dp0"

FOR /F "tokens=1-5" %%A IN (testList.txt) DO (
	%exePath% %%A %%B -start %%C -finish %%D -report %%E > %%B.test
	fc %%B.test %%B.res > nul
	if errorlevel 1 @echo %%I\%%B && goto faild
	del %%B.test
)

cd ..

@echo OK
exit /B

:faild
cd ..
@echo "Faild"
exit 1