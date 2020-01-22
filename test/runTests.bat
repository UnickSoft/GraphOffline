@echo off

for /D %%S in (*) do ( 
  @echo Call %%S
  call "%%S\_runTests.bat"
  if errorlevel 1 goto :faild
)

@echo OK
exit

:faild
@echo Faild