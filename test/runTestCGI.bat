@echo off

set exePath="../Debug/GraphOffline.exe"

rem FOR /F "tokens=1-3" %%A IN (testList.txt) DO (
rem 	set REQUEST_METHOD="POST"
rem 	set CONTENT_LENGTH=312
rem 	set QUERY_STRING=%%Bto%%C
rem 	type %%A | %exePath%
rem )


 	set REQUEST_METHOD="POST"
 	set CONTENT_LENGTH=1587
 	set QUERY_STRING=n0ton10
 	type graph_shortPath_3.xml | %exePath%


@echo OK
exit

:faild
@echo "Faild"
	       
