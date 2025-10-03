Command list and test results:

1. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -blf -start S
Ok
2. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -concomp -strong false
Ok
3. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -concomp -strong true
Ok
4. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -elloop
200
5. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -elpath
45
6. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -hamloop
30
7. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -hampath
30
8. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -mc
Ok
9. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -mfpr -source S -drain F
Ok
10. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -mis
90
11. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -dsp -start S -finish F
Ok
12. python .\run_profiling.py ..\build\vscode\cmake\Release\GraphOffline.exe -prnpaths -start S -finish F
15
