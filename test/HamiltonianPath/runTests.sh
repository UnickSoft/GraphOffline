# @echo off

if [ "$1" == "-linux" ]; then
    export exePath=../../bin/Linux/Release/GraphOffline;
else
    export exePath=../../bin/Mac/Release/GraphOffline;
fi

./_runTests.sh $1 $2

