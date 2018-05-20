#!/bin/sh

#rm -f /net/cs/104/0416042/rwg/baozi
#rm -f /net/cs/104/0416042/rwg/ls.txt
#rm -f /net/cs/104/0416042/rwg/test1.txt
#rm -f /net/cs/104/0416042/rwg/test2.txt

#rm -f /net/cs/104/0416042/rwg1/baozi
#rm -f /net/cs/104/0416042/rwg1/ls.txt
#rm -f /net/cs/104/0416042/rwg1/test1.txt
#rm -f /net/cs/104/0416042/rwg1/test2.txt


if [ $# -ne 3 ] ; then
  echo "Usage : ./test.sh {IP} {port} {id}"
  exit 0
fi


ip="$1"
port="$2"
id="$3"

for i in `seq 1 7` ; do
  ./client "$ip" "$port" test/test$i.txt > output$id/output$i
  diff -Z output$id/output$i ans/ans$i
  echo "Test case $i finish"
done
