@echo off

set exePath="../bin/Windows/Release/GraphOffline.exe"

 	set REQUEST_METHOD="POST"
 	set CONTENT_LENGTH=1587
 	set QUERY_STRING=n0ton10
 	type graph_shortPath_3.xml | %exePath%

@echo OK
exit

:faild
@echo "Faild"
	       
