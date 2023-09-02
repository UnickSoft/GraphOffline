# @echo off

if [ "$1" == "-linux" ]; then
    export exePath=../../bin/Linux/Release/GraphOffline;
else
    export exePath=../../bin/Mac/Release/GraphOffline;
fi

totalResult=0

while read d; do
    cd "$d"

    if [[ $2 != "-debug" ]]
    then
        ./_runTests.sh $1 $2 > /dev/null
    else
        ./_runTests.sh $1 $2 > $d.stdout 2> $d.stderr
    fi
    rc=$?

    printf $d
    if [[ $rc != 0 ]]
    then
        echo " Failed"
        echo $d.stderr
        tail $d.stderr
        echo $d.stdout
        tail $d.stdout
        totalResult=$rc
    else
        echo " Ok"
    fi
    cd ..
done <<< "$(find . -maxdepth 1 -mindepth 1 -type d)"

if [[ $totalResult != 0 ]]
then
    echo "-- FAILED --"
else
    echo "-- OK --"
fi

exit $totalResult


# find . -type d -exec sh -c '(cd {} && ./_runTests.sh)' ';'
