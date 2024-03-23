export exePathTest=./bin/Linux/Release/GraphOfflineTest;
export exePath=./bin/Linux/Release/GraphOffline;

export TIMEFORMAT=%E

TIME_TEST_BIN=0
TIME_TEST=0

if [ -e $exePathTest ]
then
    echo "Testing Test"
    TIME_TEST_BIN=$( { time $exePathTest $@ > /dev/null; } 2>&1 )
else
    echo "Error: Test not found: $exePathTest"
fi

if [ -e $exePath ]
then
    echo "Testing Bin"
    TIME_TEST=$( { time $exePath $@ > /dev/null; } 2>&1 )
else
    echo "Error: Bin not found: $exePath"
fi

echo "Test: $TIME_TEST_BIN sec"
echo "Bin:  $TIME_TEST sec"

