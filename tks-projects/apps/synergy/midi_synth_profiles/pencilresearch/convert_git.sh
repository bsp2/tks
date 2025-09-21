#!/bin/bash
GIT_MIDI_PATH=/Users/bsp/git/midi
IFS=§
find "$GIT_MIDI_PATH" -name \*.csv >git_files.txt
IFS=$'\n'
for d in `cat git_files.txt`; do 
  echo ""
  echo "converting \"${d}\""
  tks csv_to_msp.tks "${d}"
done
