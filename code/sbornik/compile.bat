rm -f report.txt
make -f core-Makefile 2>>report.txt
make -f server-Makefile 2>>report.txt
rm -f *.o
echo finished
