#ifndef CHENOM_ERR_
#define CHENOM_ERR_
#include <cstdio>
#include <cstdarg>
#include "h/err.hpp"
#include <string>

	int was_error;
//	#pragma warning(disable: 4793)
	void error(CHENOM_ERROR_NUMBER cislo_chyby, size_t cislo_tokenu, ... ){
		char* s, *s2;
		std::string ZPRAVA;
		int i;
		size_t t, t1;
		va_list data;
		va_start(data, cislo_tokenu);

		if(was_error==0) was_error=cislo_chyby;

		switch(cislo_chyby){
			case ERR_NO_DATA_INPUT:
				s= va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Cannot open the inputdatafile '%s'!", cislo_chyby, s);
				break;
			case ERR_BAD_DATA_INPUT:
				i= va_arg(data, int);
				ZPRAVA=string_printf("Warning %d: Database incorrect (item number %d on line %d)!", cislo_chyby, i, cislo_tokenu);
				break;
			case ERR_BAD_SYNTAX_INPUT:
				ZPRAVA=string_printf("Error %d: Bad syntax ('chenom &lt;settings_file&gt; [-e]')!", cislo_chyby);
				break;
			case ERR_BAD_SETTINGS_FILE:
				s= va_arg(data, char*);
				s2=va_arg(data, char*);
				ZPRAVA=string_printf("Warning %d: Settings file incorrect!(Item %s-%s not found.)", cislo_chyby, s, s2);
				break;
			case ERR_UNKNOWN_TOKEN:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Unknown token starting on position %d ('%s')!", cislo_chyby, cislo_tokenu, s);
				break;
			case ERR_SYNTAX:
				ZPRAVA=string_printf("Error %d: Bad syntax!", cislo_chyby);
				break;
			case ERR_UNKNOWN_TERMINAL:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Unknown terminal ('%s') on line number %d!",cislo_chyby, s, cislo_tokenu);
				break;
			case ERR_UNKNOWN_NONTERMINAL:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: No rule for the nonterminal '%s' used in the grammer file!",cislo_chyby, s);
				break;
			case ERR_UNKNOWN_COMMAND:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Unknown command %s in the syntax file!",cislo_chyby, s);
				break;
			case ERR_NO_COMMAND:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Warning %d: No commands for the rule '%s'!", cislo_chyby, s);
				break;
			case ERR_NO_ATOM_INDEX:
				t = va_arg(data, size_t);
				ZPRAVA=string_printf("Error %d: No atom index '%d'!", cislo_chyby, t);
				break;
			case ERR_NO_ATOM_NAME:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: No atom name '%s'!", cislo_chyby, s);
				break;
			case ERR_NOEXIST_BIND:
				ZPRAVA=string_printf("Error %d: Impossible type of binding!", cislo_chyby);
				break;
			case ERR_UNKNOWN_BIND:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Unknown type of binding ('%s')!",cislo_chyby, s);
				break;
			case ERR_NO_FREE_BIND:
				ZPRAVA=string_printf("Error %d: No bindable atom!", cislo_chyby);
				break;
			case ERR_TOO_MANY_BIND:
				t = va_arg(data, size_t);
				t1 = va_arg(data, size_t);
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Warning %d: Too many bindings on '%s' (%d < %d)!", cislo_chyby, s, t, t1);
				break;
			case ERR_NO_TORZO:
				ZPRAVA=string_printf("Error %d: No torso, too many atoms!", cislo_chyby);
				break;
			case ERR_DIF_BIND:
				ZPRAVA=string_printf("Error %d: Numbers of bindings are different!", cislo_chyby);
				break;
			case ERR_NO_BRIDGE:
				ZPRAVA=string_printf("Error %d: Undeletable atom, too many bindings!", cislo_chyby);
				break;
			case ERR_TOO_MANY_ARGS:
				ZPRAVA=string_printf("Error %d: Too many args!", cislo_chyby);
				break;
			case ERR_INCOR_COMMAND:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Incorrect command '%s'!", cislo_chyby, s);
				break;
			case ERR_UNKNOWN_ATOM:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Warning %d: Unknown atom!", cislo_chyby);
				break;
			case ERR_NUM_ARG_EXPECT:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Numeric argument expected ('%s')!", cislo_chyby, s);
				break;
			case ERR_STR_ARG_EXPECT:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: String argument expected ('%s')!", cislo_chyby, s);
				break;
			case ERR_ZDO_ARG_EXPECT:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Lokant-list argument expected ('%s')!", cislo_chyby, s);
				break;
			case ERR_MOL_ARG_EXPECT:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d: Skelet argument expected ('%s')!",cislo_chyby, s);
				break;
			case ERR_BIG_CHARGE:
				i = va_arg(data, int);
				ZPRAVA=string_printf("Error %d: Too big charge '%d'!", cislo_chyby, i);
				break;
			case ERR_NO_DATA_INDEX:
				t = va_arg(data, size_t);
				ZPRAVA=string_printf("Error %d: No data item number %d!", cislo_chyby, t);
				break;
			case ERR_INCOR_ARG:
				s = va_arg(data, char*);
				ZPRAVA=string_printf("Error %d. Incorrect argument format '%s'!", cislo_chyby, s);
				break;
			case ERR_LONG_ARRAY:
				ZPRAVA=string_printf("Error %d: Too long array!", cislo_chyby);
				break;
			case ERR_CMPX_SUB:
				t = va_arg(data, size_t);
				t1= va_arg(data, size_t);
				ZPRAVA=string_printf("Error %d: Bad complex substitution (%d subs. on %d atoms)!", cislo_chyby, t, t1);
				break;
			default:
				ZPRAVA=string_printf("Error %d: An unknown error in the token number %d.", cislo_chyby, cislo_tokenu);
		}
		std::cerr<<ZPRAVA<<std::endl;

		va_end(data);
	}
//	#pragma warning(default: 4793)
#endif
