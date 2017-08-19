@echo off
chcp
SET cil=".\DATA\chenom.bison.y"
SET cesta=".\DATA\chenom.bison.y.$$$"
chcp 852

echo ---------------------------------- HLAVI¨KA  ----------------------------------
echo %%{ > %cesta%
echo		#include "lexval.hpp" >> %cesta%
echo		#include "err.hpp" >> %cesta%
echo		#include "main.hpp" >> %cesta%
echo	/**/ >> %cesta%
echo	    /* semantic value */ >> %cesta%
echo	    #define YYSTYPE lexval >> %cesta%
echo	/**/ >> %cesta%
echo		/* locations */ >> %cesta%
echo		#define YYLTYPE unsigned >> %cesta%
echo		#define YYLLOC_DEFAULT(cur, rhs, n)	do { if(n) (cur)=(rhs)[1]; else (cur)=(rhs)[0]; } while(0) >> %cesta%
echo	/**/ >> %cesta%
echo		/* error stuff */ >> %cesta%
echo	    #define YYERROR_VERBOSE 1 >> %cesta%
echo		static void yyerror(unsigned *loc, lexval val, const char *m); >> %cesta%
echo	/**/ >> %cesta%
echo	    /* hack */ >> %cesta%
echo	    #define YY_HACK >> %cesta%
echo	    #ifdef YY_HACK >> %cesta%
echo			//#define short long >> %cesta%
echo			#define YYINCLUDED_STDLIB_ >> %cesta%
echo			#define YYMALLOC >> %cesta%
echo			#define YYFREE >> %cesta%
echo	    #endif >> %cesta%
echo	/**/ >> %cesta%
echo		//#define YYDEBUG 1 >> %cesta%
echo	/**/ >> %cesta%
echo %%} >> %cesta%

echo ---------------------------------- TERMINµLY ----------------------------------
cat ".\DATA\terminaly.gra" | sed "s/^[^'].*$/ /" | sed "s;'\([^']*\)';%token TOKEN_\1;g" | sed "s/\\'/'/g" >> %cesta%
echo %%%% >> %cesta%

echo ---------------------------------- GRAMATIKA ----------------------------------
rem							nahrad rovn°tka   | oprav koment†˝e							 | slouüen° tokenÖ | p˝epis termin†lÖ na identifik†tory
rem cat ".\DATA\gramatika.gra" | sed "s/:==/  :/" | sed "s/^\([ \n\t]*\)--\(.*\)$/\1\/\/\2/" | sed "s/+/ /g" | sed "s;\'\(\([^\'\\]*\(\\\'\)*\)*\)\';TOKEN_\1;g" >> %cesta%
cat ".\DATA\gramatika.gra" | sed "s/:==/  :/" | sed "s/^\([ \n\t]*\)--\(.*\)$/\1\/\/\2/" | sed "s/+/ /g" | sed "s;'\([^']*\)';TOKEN_\1;g" | sed "s/\\'/'/g" >> %cesta%
rem																												s= ' ( ([^ ' \]* ( \ ' )* )* ) '=TOKEN_\1
echo %%%% >> %cesta%

echo -------------------------------- ZµV¨RE¨NÌ K‡D --------------------------------

echo -------------------------------- POST-EDITING  --------------------------------
cat %cesta% | sed "s:TOKEN_<\([^/>]*\)>:TOKEN_XMLz_\1:g;            s:TOKEN_<\([^/>]*\)/>:TOKEN_XMLs_\1:g;         s:TOKEN_</\([^/>]*\)>:TOKEN_XMLk_\1:g;" | tr "†ü‘Çÿ°Â¢˝Áú£ÖÏß" "ACDEFINORSTUVYZ" >%cil%
rm -f %cesta%

@echo on
