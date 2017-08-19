# z      ..            kazdy svuj radek  netiskni  mezi 1. a posl. apostrofem vypis  do ..
cat DATA/gramatika.gra | tr "+" "\n" | sed -n     "s/^[^']*\('.*'\)[^']*$/\1/p"    > DATA/terminaly.gra
