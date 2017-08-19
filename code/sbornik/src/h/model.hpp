#ifndef CHENOM_MODEL_H_
#define CHENOM_MODEL_H_
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "err.hpp"
#include "semantika.hpp"
#include "utils.hpp"

namespace model{
	struct bod{	//souradnice bodu
		double x;
		double y;
		double z;
		bod(double a, double b, double c);
		bod operator+(const bod &b)const;
		bod operator-(const bod &b)const;
		bod operator*(const double b)const;
		bod operator/(const double b)const;
	};

	struct atom{	//polozka skeletu
		std::vector<semantika::Prvek>::const_iterator	jadro;	//pointer na prvek (znacka, hybridizace, barva, ..)
		char								naboj;	//naboj atomu
		std::vector<std::string>			jmena;	//vsechna alternativni jmena (1,<alpha/>,1',..)
		std::vector<semantika::vazba>		vazby;	//vsechny z tohoto atomu vychazejici vazby - kam,kolik .. co chybi = doplnit -H
		bod									poloha;	//vektor polohy atomu
		bod									smer;	//vektor rychlosti atomu
		bod									F;		//vektor sily na atom
		atom(semantika::atom a);					//c-tor z atomu ze semantiky
	};

	struct skelet{
		std::vector<atom> atomy;

		skelet(semantika::skelet mol);
		void optimalize();
	};

	void inicializace();
	double norma(const bod &X);
	void yymodel(semantika::skelet graf);
}
#include "grafika.hpp"
#endif
