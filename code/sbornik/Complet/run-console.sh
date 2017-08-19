#!/bin/sh
echo $@ >input
vstup=input
vystup="outs/$@.wrl"
if [ ! -e $vystup ]
then
	if ! ( ./chenom DATA/utf8/settings.ini <$vstup >$vystup 2>error.log )
	then
		rm $vstup
		rm $vystup
		exit 3
	fi
fi
rm $vstup

