#!/bin/bash

YEAR=$(pwd | grep -o '20..-.')
DATA=/home/matheus/Documentos/Matheus/UFES/8p/compiladores/trabCminus/GeradorCodigo
IN=$DATA/in
OUT=$DATA/out5

EXE=./parser

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    $EXE < $infile > $outfile
done
