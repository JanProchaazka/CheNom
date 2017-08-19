#ifndef CHENOM_SEMANTIKA_H_
#define CHENOM_SEMANTIKA_H_
	#include <vector>
	#include <string>
	#include "parser.hpp"
	#include "instrukce.hpp"

	namespace semantika{
		void inicializace(std::istream &is);	//pripravy tabulku prvku, atd..
		skelet yysem();
	}
#endif
