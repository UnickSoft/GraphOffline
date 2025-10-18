#!/bin/bash

# Initialize failure flag
isFailed=0

# Remove old test output files
rm -f *.test &>/dev/null

# Read each line from testList.txt
while read -r line; do
  # Split the line into an array of arguments
  args=($line)

  command="${args[0]}"
  xmlFile=""
  extraArgs=()

  # Find the first argument ending with ".xml"
  for ((i=1; i<${#args[@]}; i++)); do
    if [[ "${args[i]}" == *.xml ]]; then
      xmlFile="${args[i]}"
      # Everything after the XML file is considered extra arguments
      extraArgs=("${args[@]:i+1}")
      break
    fi
  done

  # If no XML file found, skip this line
  if [[ -z "$xmlFile" ]]; then
    echo "No XML file found in line: $line"
    continue
  fi

  # Print debug information if "-debug" flag is passed as script argument
  if [[ "$2" == "-debug" ]]; then
    echo "$exePath $command ./$xmlFile ${extraArgs[*]} > ${xmlFile}.test"
  fi

  # Execute the command and redirect output to a .test file
  $exePath "$command" "./$xmlFile" "${extraArgs[@]}" > "${xmlFile}.test"

  # Compare the result with the expected .res file
  if diff --ignore-all-space "${xmlFile}.res" "${xmlFile}.test" >/dev/null; then
    continue
  else
    isFailed=1
    echo "${xmlFile} failed."
    break
  fi
done < "testList.txt"

# Final status output
if [ $isFailed -eq 1 ]; then
  echo "Failed"
  exit 1
else
  echo "OK"
  rm -f *.test &>/dev/null
fi
