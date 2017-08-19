cat DATA/gramatika.gra | sed -n "s/^\([^ \t]*\)[ \t]*:==.*$/\1/p"
