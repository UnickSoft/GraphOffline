@echo off

set exePath="../Debug/GraphOffline.exe"

FOR /F "tokens=1-3" %%A IN (testList.txt) DO (
	%exePath% -sp %%A -start %%B -finish %%C > %%A.test
	fc %%A.test %%A.res > nul
	if errorlevel 1 @echo %%A && goto faild
	del %%A.test
)

@echo OK
exit

:faild
@echo "Faild"
		          