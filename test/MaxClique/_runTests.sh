# @echo off

isFaild=0;

rm *.test &>/dev/null

while IFS=$' ' read -r xmlFile lb ub algo expected report; do
    if [ "$2" == "-debug" ]; then
        echo "$exePath -mc ${xmlFile} -lb ${lb} -ub ${ub} -algo ${algo} -expected ${expected} -report ${report}"
    fi

    $exePath -debug -mc ${xmlFile} -lb ${lb} -ub ${ub} -algo ${algo} -expected ${expected} -report ${report}

    if [ $? -eq 1 ]; then
        echo "${xmlFile} OK"
    else
        isFaild=1;
        echo "${xmlFile} KO"
    fi
done < "testList.txt"

if [ $isFaild -eq 1 ]; then
    echo "Test Failed";
    exit 1;
else
    echo "Test Passed"
fi
