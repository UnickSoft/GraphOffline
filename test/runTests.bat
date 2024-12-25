@echo off

for /D %%S in (*) do ( 
  @echo Call %%S
  call "%%S\_runTests.bat" "..\%1"
  if errorlevel 1 goto :faild
)

@echo OK
exit

:faild
@echo Faild