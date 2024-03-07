#!/bin/bash

CURRENT_DIR="$(dirname "$(realpath "$0")")"

# Team (Mac => Replace with your path autotester.exe path)
AUTOTESTER_BINARY="$CURRENT_DIR/../../Code09/cmake-build-release/src/autotester/autotester"

TESTS_FOLDER="."

for MILESTONE_FOLDER in "$TESTS_FOLDER"/milestone*/; do
  MILESTONE_NUMBER=$(basename "$MILESTONE_FOLDER" | sed 's/milestone//')

  for ((i=1; ; i++)); do
    SOURCE_FILE="$MILESTONE_FOLDER/milestone${MILESTONE_NUMBER}_source_$i.txt"
    QUERIES_FILE="$MILESTONE_FOLDER/milestone${MILESTONE_NUMBER}_queries_$i.txt"

    # Check if the source file exists, exit loop if not
    if [ ! -e "$SOURCE_FILE" ]; then
      break
    fi

    OUTPUT_FILE="$MILESTONE_FOLDER/milestone${MILESTONE_NUMBER}_out_$i.xml"
    "$AUTOTESTER_BINARY" "$SOURCE_FILE" "$QUERIES_FILE" "$OUTPUT_FILE"

    echo "Autotester completed for Milestone $MILESTONE_NUMBER - Source $i and Queries $i"
  done
done
