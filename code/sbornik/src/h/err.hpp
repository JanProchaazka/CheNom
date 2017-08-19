#ifndef CHENOM_ERR_H_
#define CHENOM_ERR_H_
	#include <iostream>
	#include "utils.hpp"

//	#define DEBUG
//	#define DEBUG_PRINTS
	//scanner:
		//#define DEBUG_PRINTS_DATABASE
		#define DEBUG_PRINTS_TOKENS
	//parser:
		//#define DEBUG_PRINTS_GRAMMAR
		//#define DEBUG_PRINTS_EDGES
		#define DEBUG_PRINTS_DTREE
	//semantika(grafer):
		//#define DEBUG_PRINTS_ATOMS
		//#define DEBUG_PRINTS_SKELET
		#define DEBUG_PRINTS_COMMANDS
	//modelovani:
		//#define DEBUG_PRINTS_MODEL
		//#define PLANAR_MODEL
		#define SPHERE_MODEL
		#define MODEL_BIND_LEN
	//main:
		//#define DEBUG_LEXONLY
		//#define DEBUG_SYNONLY
		//#define DEBUG_SEMONLY

		//#define DEBUG_CIN

		//#define DEBUG_VSTUP "kyselina<space/>10-methyl-2'a,5\"b,3c-diethyltritetrakosanová"
		//#define DEBUG_VSTUP "kyselina<space/>"
		//#define DEBUG_VSTUP "buta-2,3-dien"
		//#define DEBUG_VSTUP "penta-2,3-dien"
		//#define DEBUG_VSTUP "1,3-dimethylpropa-3,40-dien"
		//#define DEBUG_VSTUP "1,3-dimethyltritetrakonta-3,20-dien"
		//#define DEBUG_VSTUP "1,1,6,6-tetrafenylhexaaza-2,4-dien"
		//#define DEBUG_VSTUP "ethan"
		//#define DEBUG_VSTUP "&alpha;-chloroctová&nbsp;kyselina"
		#define DEBUG_VSTUP "10-chlorpentadekan"
	//---------------------------------------------------------------------------*/
	extern int was_error;

	enum CHENOM_ERROR_NUMBER{
		WARNING=100,
		ERR_BAD_DATA_INPUT,
		ERR_BAD_SYNTAX_INPUT,

		ERROR=500,
		ERR_UNKNOWN_TOKEN,
		ERR_SYNTAX,
			ERR_UNKNOWN_TERMINAL,
			ERR_UNKNOWN_NONTERMINAL,
		ERR_SEMANTIKA,
			ERR_UNKNOWN_COMMAND,
			ERR_NO_COMMAND,
			ERR_NO_ATOM_INDEX,
			ERR_NO_ATOM_NAME,
			ERR_NOEXIST_BIND,
			ERR_UNKNOWN_BIND,
			ERR_NO_FREE_BIND,
			ERR_TOO_MANY_BIND,
			ERR_NO_TORZO,
			ERR_DIF_BIND,
			ERR_BIG_CHARGE,
			ERR_NO_BRIDGE,
			ERR_TOO_MANY_ARGS,
			ERR_INCOR_COMMAND,
			ERR_UNKNOWN_ATOM,
			ERR_NUM_ARG_EXPECT,
			ERR_STR_ARG_EXPECT,
			ERR_ZDO_ARG_EXPECT,
			ERR_MOL_ARG_EXPECT,
			ERR_NO_DATA_INDEX,
			ERR_INCOR_ARG,
			ERR_LONG_ARRAY,
			ERR_CMPX_SUB,

		FATAL_ERROR	=600,
		ERR_NO_DATA_INPUT,
		ERR_BAD_SETTINGS_FILE
	};

	void error(CHENOM_ERROR_NUMBER cislo_chyby, size_t cislo_tokenu, ... );
#endif
