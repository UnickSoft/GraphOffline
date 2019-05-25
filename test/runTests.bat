@echo off

for /D %%S in (.\*) do ( 
  call "%%S\_runTests.bat"
  if errorlevel 1 goto faild
)

@echo OK
exit

:faild
@echo Faild
		          