#!/bin/bash
cp [path]/powerdetect/log/general.txt [path]/powerdetect/backup/general.txt.old
cp [path]/powerdetect/log/detected.txt [path]/powerdetect/backup/detected.txt.old
cp [path]/powerdetect/log/output.txt [path]/powerdetect/backup/output.txt.old
rm -f [path]/powerdetect/log/general.txt
rm -f [path]/powerdetect/log/detected.txt
rm -f [path]/powerdetect/log/output.txt
