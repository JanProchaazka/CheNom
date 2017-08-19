#ifndef CHENOM_SCANNER_H_
#define CHENOM_SCANNER_H_

	#include <string>
	#include <iostream>
	#include <fstream>

	#include <vector>
	#include <stack>
	#include "err.hpp"
	#include "lexval.hpp"

	namespace scanner{
		//tridy
		//class flags;
		//class typeoftoken;
		//class node;
		//class nodex;

		void inicializace(std::istream&);
		void inicializace_intervalu(std::istream&);
		int hodnota(std::string);
		std::string text(int);
		void parse(std::string);
		int yylex(lexval *param, unsigned int &token);

		//globální deklarace
	}
#endif
