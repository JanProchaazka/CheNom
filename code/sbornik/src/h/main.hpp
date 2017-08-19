#ifndef CHENOM_MAIN_H_
#define CHENOM_MAIN_H_
	#include <string>
	#include <iostream>
	#include <fstream>

	#include "err.hpp"
	#include "scanner.hpp"
	#include "parser.hpp"
	#include "semantika.hpp"
	#include "model.hpp"

	#ifdef WIN32
		#define WINDOWS
	#endif

	#ifdef WINDOWS
		#define PATHDELIM "\\"
	#else
		#define PATHDELIM "/"
	#endif
#endif