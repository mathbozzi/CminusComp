#!/bin/bash

YEAR=$(pwd | grep -o '20..-.')
DATA=/home/matheus/Documentos/Matheus/UFES/8p/compiladores/trabCminus/Parser+Scanner
IN=$DATA/in
OUT=$DATA/out2

EXE=./parser

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    $EXE < $infile > $outfile
done
