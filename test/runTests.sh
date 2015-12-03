# @echo off

exePath=../bin/Mac/Release/GraphOffline;
isFaild=0;

while IFS=$' ' read -r command xmlFile startGraph finishGraph ; do
#echo $exePath ${command} ./${xmlFile} -start ${startGraph} -finish ${finishGraph}

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
fi
