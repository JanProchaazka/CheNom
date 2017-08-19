#!/bin/sh
vstup=input
vystup="outs/$@.wrl"

echo $@ >$vstup

# ------------------
# necachovat modely:
# rm -f "outs/*.wrl"
# ------------------

if [ ! -e $vystup ]
then
	if ! ( ./chenom DATA/utf8/settings.ini -e <$vstup >$vystup 2>error.log )
	then
		# ------------------------
		# podrobny chybovy vystup:
		echo ======================== >> error.log
		cat $vystup >> error.log
		# ------------------------
		rm $vstup
		rm $vystup
		exit 3
	fi
fi
rm $vstup

