sed '1d' $1 | sort --field-separator=',' -n -k 1,1 -k 2,2 -k 3,3 -k 4,4 > TEMP.csv
sed '1d' $2 | sort --field-separator=',' -n -k 1,1 -k 2,2 -k 3,3 -k 4,4 > temp.csv
cmp --silent temp.csv TEMP.csv && echo '### SUCCESS: Files Are Identical! ###' || echo '### WARNING: Files Are Different! ###'
rm -rf TEMP.csv
rm -rf temp.csv


# ./check.sh filename columnNumber provide1IfDESC
# ./check.sh student.csv 1 1 (checks if file is sorted DESC on first column)
# ./check.sh student.csv 2 (checks if file is sorted ASC on second column)
# echo $1 $2 $3
sed '1d' $1 | cut -d, -f $2 > tempXxxX.csv
sort -n ${3:+-r} tempXxxX.csv > TEMPXxxX.csv
cmp --silent tempXxxX.csv TEMPXxxX.csv && echo '### SUCCESS: Files is sorted! ###' || echo '### WARNING: File is not sorted! ###'
rm -rf TEMPXxxX.csv
rm -rf tempXxxX.csv
