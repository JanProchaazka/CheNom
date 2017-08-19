@echo off

SET termy=".\terminaly.gra"
SET grama=".\gramatika.gra"
SET cesta=".\gramatika.bison.y.$$$"

rem echo ---------------------------------- HLAVI�KA  ----------------------------------

rem echo ---------------------------------- TERMIN�LY ----------------------------------
rem odstran komentare:
	type %termy%  | sed "s/^[^'].*$//" > temp.$$$
rem rozsir o urezane "a":
	type temp.$$$ | sed "s/^'\(..*\)a'$/'\1a' '\1'/" > tmp2.$$$
rem rozsir o pridane "o":
	type tmp2.$$$ | sed "s/^'\(.*[^o]\)'$/'\1' '\1o'/" >temp.$$$
rem obal definici:
	type temp.$$$ | sed "s;'\(.[^']*\)';%%token TOKEN_\1;g;" >> %cesta%
rem uklid:
	del temp.$$$ tmp2.$$$
echo %%%% >> %cesta%

rem echo ---------------------------------- GRAMATIKA ----------------------------------
rem nahrad rovn�tka:
	type %grama%  | sed "s/:==/  :/" > temp.$$$
rem oprav koment��e:
	type temp.$$$ | sed "s:--://:g; s://-:///:g" > tmp2.$$$
rem slou�en� token�:
	type tmp2.$$$ | sed "s/+/ /g" >> %cesta%
rem uklid:
	del temp.$$$ tmp2.$$$

echo %%%% >> %cesta%

rem echo -------------------------------- Z�V�RE�N� K�D --------------------------------

rem echo -------------------------------- POST-EDITING  --------------------------------
rem narodni znaky - carky:
	type %cesta%  | sed "s:�:_carka_a_:g; s:�:_carka_e_:g; s:�:_carka_i_:g; s:�:_carka_o_:g; s:�:_carka_u_:g; s:�:_carka_y_:g" > tmp2.$$$
rem narodni znaky - krouzek:
	type tmp2.$$$ | sed "s:�:_krouzek_u_:g" > temp.$$$
rem narodni znaky - hacky:
	type temp.$$$ | sed "s:�:_hacek_c_:g; s:�:_hacek_d_:g; s:�:_hacek_e_:g; s:�:_hacek_n_:g; s:�:_hacek_r_:g; s:�:_hacek_s_:g; s:�:_hacek_t_:g; s:�:_hacek_z_:g" > tmp2.$$$


rem prepis terminalu na identifikatory:


	type tmp2.$$$ | sed "s;'\([^' 	][^' 	]*\)';TOKEN_\1;g" > %cesta%
rem zacatek tagu:
	type %cesta%  | sed "s:TOKEN_<\([^/>]*\)>:TOKEN_XMLz_\1:g" > temp.$$$
rem konec tagu:
	type temp.$$$ | sed "s:TOKEN_</\([^/>]*\)>:TOKEN_XMLk_\1:g" > tmp2.$$$
rem html entita:
	type tmp2.$$$ | sed "s:TOKEN_&\([^;]*\);:TOKEN_XMLs_\1:g" > temp.$$$
rem zavorky:
	type temp.$$$ | sed "s:TOKEN_(:TOKEN_zav1L:g; s:TOKEN_):TOKEN_zav1R:g; s:TOKEN_\[:TOKEN_zav2L:g; s:TOKEN_\]:TOKEN_zav2R:g; s:TOKEN_{:TOKEN_zav3L:g; s:TOKEN_}:TOKEN_zav3R:g" > tmp2.$$$
rem oddelovace:
	type tmp2.$$$ | sed "s:TOKEN_->:TOKEN_arrow:g; s:TOKEN_-:TOKEN_dash:g; s:TOKEN_\.:TOKEN_dot:g; s:TOKEN_\,:TOKEN_comma:g; s:TOKEN_\::TOKEN_colon:g; s:TOKEN_\;:TOKEN_semicolon:g; s:TOKEN_':TOKEN_apostrophe:g" >temp.$$$
rem apostrof:
	type temp.$$$ | sed "s:''':TOKEN_apostrophe:g" > tmp2.$$$




rem semanticky kod:
	type tmp2.$$$ | sed "s:\#\([0-9]*\):\1:g;  s:$0:$$:g;  s:~:\":g;"

del temp.$$$ tmp2.$$$
del %cesta%

@echo on
