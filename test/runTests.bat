@echo off

set exePath="../bin/Windows/Release/GraphOffline.exe"

FOR /F "tokens=1-4" %%A IN (testList.txt) DO (
	%exePath% %%A %%B -start %%C -finish %%D > %%B.test
	fc %%B.test %%B.res > nul
	if errorlevel 1 @echo %%B && goto faild
	del %%B.test
)

@echo OK
exit

:faild
@echo "Faild"
		          