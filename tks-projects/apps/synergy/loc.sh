#!/bin/sh

total=0

total=0
num=0
nummod=0

echo "\nnative:"

# yac
num=`wc -l ../../yac/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<yac>=$num"

# tks
num=`wc -l ../../tks-source/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tks>=$num"

# libanalogrytm
num=`wc -l ../../libanalogrytm/*.{c,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<libanalogrytm>=$num"

# tkanalogrytm
num=`wc -l ../../tkanalogrytm/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkanalogrytm>=$num"

# tkclap
num=`wc -l ../../tkclap/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkclap>=$num"

# tkfileutils
num=`wc -l ../../tkfileutils/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkfileutils>=$num"

# tkfreetype2
num=`wc -l ../../tkfreetype2/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkfreetype2>=$num"

# tkmath
num=`wc -l ../../tkmath/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkmath>=$num"

# tkmidi (win32)
num=`wc -l ../../tkmidi/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkmidi_win32>=$num"

# tkmidi_alsa (Linux)
num=`wc -l ../../tkmidi_alsa/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkmidi_alsa>=$num"

# tkmidi_portmidi (macOS)
num=`wc -l ../../tkmidi_portmidi/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkmidi_portmidi>=$num"

# tkmidipipe
num=`wc -l ../../tkmidipipe/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkmidipipe>=$num"

# tkopengl
num=`wc -l ../../tkopengl/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tkopengl>=$num"

# tksdl2
num=`wc -l ../../tksdl2/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tksdl2>=$num"

# tktriangulate
num=`wc -l ../../tktriangulate/*.{cpp,h} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tktriangulate>=$num"

totalnative=$total


# ---- script ----
echo "\nscript:"
total=0

# app
num=`wc -l *.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<app.base>=$num"

# app msp
num=`wc -l midi_synth_profiles/*.msp 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<app.msp>=$num"

# app script presets
num=`wc -l script_preset_source_archive/*.{tks,xfm} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<app.script_presets>=$num"

# ui
num=`wc -l ../../tkui/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<UI>=$num"

# localmidi
num=`wc -l ../localmidi/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<localmidi>=$num"

# librs
num=`wc -l ../librs/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
echo "total<librs>=$num"
total=$((total + num))

# modularcv
num=`wc -l ../modularcv/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<modularcv>=$num"

# nodes
for nodedir in `find . -type d -name node_\*` ; do
 num=`wc -l ${nodedir}/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
 total=$((total + num))
 echo "total<$nodedir>=$num"
done

# tksampler (WavIO)
num=`wc -l ../../tksampler/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
total=$((total + num))
echo "total<tksampler>=$num"

# synth editors
for synthdir in `find . -type d -name syntheditor_\*` ; do
 num=`wc -l ${synthdir}/*.{tks,xfm,tkp} 2>/dev/null | grep -v total | awk '{print $1}' | awk '{s+=$1} END {print s}'`
 total=$((total + num))
 echo "total<$synthdir>=$num"
done

totalscript=$total

# ---- total
echo "\ntotal:"
echo "total<native>=$totalnative"
echo "total<script>=$totalscript"
total=$((totalscript + totalnative))
echo "total=$total"
