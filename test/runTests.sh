# @echo off

if [ "$1" == "-linux" ]; then
    export exePath=../../bin/Linux/Release/GraphOffline;
else
    export exePath=../../bin/Mac/Release/GraphOffline;
fi

totalResult=0

find . -maxdepth 1 -mindepth 1 -type d | while read d; do
    cd "$d"
    if [[ $2 != "-debug" ]]
    then
        ./_runTests.sh $1 $2 > /dev/null
    else
        ./_runTests.sh $1 $2
    fi
    rc=$?

    printf $d
    if [[ $rc != 0 ]]
    then
        echo " Failed"
        totalResult = $rc
    else
        echo " Ok"
    fi

    cd ..
done

if [[ $totalResult != 0 ]]
then
    echo "-- FAILED --"
else
    echo "-- OK --"
fi

exit $totalResult


# find . -type d -exec sh -c '(cd {} && ./_runTests.sh)' ';'
