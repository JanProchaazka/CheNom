rm -f report.txt
make -f core-Makefile 2>>report.txt
make -f server-Makefile 2>>report.txt
rm -f *.o
echo compiled
cd Complet
chmod 744 run.sh
chmod 744 run-console.sh
chmod 744 server
chmod 744 chenom
echo finished
