#!/bin/bash

YEAR=$(pwd | grep -o '20..-.')
DATA=/home/matheus/Documentos/Matheus/UFES/8p/compiladores/trabCminus/AST
IN=$DATA/in
OUT=$DATA/out4

EXE=./parser

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    dotfile=$OUT/${base/.cm/.dot}
    pdffile=$OUT/${base/.cm/.pdf}
    echo Running $base
    $EXE < $infile 1> $outfile 2> $dotfile
    dot -Tpdf $dotfile -o $pdffile
done
