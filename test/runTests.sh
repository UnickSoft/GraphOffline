# @echo off

exePath=../bin/Mac/Release/GraphOffline;
isFaild=0;

rm *.test

while IFS=$' ' read -r command xmlFile startGraph finishGraph ; do

  if [ "$1" == "-debug" ]; then
    echo "$exePath ${command} ./${xmlFile} -start ${startGraph} -finish ${finishGraph} > ${xmlFile}.test"
  fi
  $exePath ${command} ./${xmlFile} -start ${startGraph} -finish ${finishGraph} > ${xmlFile}.test

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
  rm *.test
fi

