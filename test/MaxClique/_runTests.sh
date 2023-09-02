# @echo off

isFaild=0;

rm *.test &>/dev/null

while IFS=$' ' read -r xmlFile report; do
    if [ "$2" == "-debug" ]; then
        echo "$exePath -debug -mc ${xmlFile} -report ${report}"
    fi

    $exePath -mc ${xmlFile} -report ${report} > ${xmlFile}.test

    if diff --ignore-all-space ${xmlFile}.res ${xmlFile}.test >/dev/null ; then
      continue;
    else
      isFaild=1;
      echo "${xmlFile} failed."
      break;
    fi
done < "testList.txt"

if [ $isFaild -eq 1 ]; then
  echo "Failed";
  exit 1;
else
  echo "OK"
  rm *.test &>/dev/null
fi

