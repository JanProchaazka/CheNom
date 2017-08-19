#ifndef CHENOM_PARSER_H_
#define CHENOM_PARSER_H_
	#include "lexval.hpp"
	#include "scanner.hpp"
	#include "err.hpp"
	#include "utils.hpp"
	#include <iostream>
	#include <string>
	#include <vector>
	#include <list>
	#include <algorithm>
	#include <fstream>

	namespace parser{
		//------------ inicializace
			std::string kod2str(int kod);

			struct Cesta{
				std::list<int>	der;
				std::string		sema;
			};

			struct neterm{
				std::string jmeno;
				std::vector< Cesta > cesty;
				size_t pocet_cest;
				neterm();
			};

			struct pravidlo{
				int c_neterm;
				size_t delka;
				Cesta *cesta;
				bool operator<(const pravidlo b) const;
			};

			size_t pridej_najdi(std::string s);
			std::string odescapuj(std::string s);
			int cislo_tokenu(std::string s);
			int inicializace(std::istream &is);

		//--------- prevod na strom
			struct token{
				int				val;
				lexval			param;
				unsigned int	cislo;
				token();
			};

			class derivacni_strom{
			//-- podtridy
			private:
				struct uz_hr{
					size_t uzlik;
					size_t hranik;
				};
				struct hrana{
					size_t z,k;									//z a do ktereho uzlu hrana vede
					int val;									//hodnota terminal / POSUN-neterminal
					std::vector<pravidlo>::const_iterator prav; //iterator na pouzite pravidlo kterym byla tato hrana vytvorena
					std::vector<uz_hr> syni;					//seznam nasledniku (hrany, z nichz tato vznikla
					bool ok;									//patri do stromu, neboj je to falesna hrana
					hrana(size_t ze, size_t ke, const std::vector<pravidlo>::const_iterator &it, int v, const std::vector<uz_hr> &naslednici);
					hrana(size_t ze, size_t ke, const std::vector<pravidlo>::const_iterator &it, int v);
				};
				struct uzel{
					std::vector<hrana> hrany;
					uzel();
					uzel(const hrana &h);
				};
			public:
				struct derivat{
					std::vector<pravidlo>::const_iterator prav;
					int hodnota;
					std::vector<derivat*> potomci;
					size_t z, k;
				};
			//-- polozky
			private:
				size_t				delka_cesty;
				std::vector<uzel>	uzly;
			public:
				const std::vector<pravidlo>	&gram;
			//-- metody
			private:
				bool je_hrana(size_t z, size_t k, int val);
				bool zkrat_cestu(const std::vector<pravidlo>::const_iterator &it_prav, 
								 const std::list<int>::const_iterator cesta /*teda jeji zbyvajici cast*/, 
								 const size_t &start, 
								 size_t pozice,
								 std::vector<uz_hr> &pouzite_hrany
				);
				void tiskni_strom(std::vector<hrana>::const_iterator koren, int indent) const;
				void tiskni_strom(std::vector<uz_hr>::const_iterator uh, int indent) const;
				derivat* preved_podstrom(std::vector<hrana>::const_iterator hr);
				derivat* preved_podstrom(hrana hr);
			public:
				static void smaz_Dstrom(derivat *koren);
				//*kvuli warningu:*/	derivacni_strom & operator=( const derivacni_strom & );
				derivacni_strom(const std::vector<token> &terminaly, const std::vector<pravidlo> &gramat);
				bool build();
				derivat* preved();
				void tiskni_hrany();
				int tiskni_stromy(bool jenjeden) const;
			};
		//------
		extern std::vector<pravidlo> gramatika;

		int inicializace(std::istream&);
		derivacni_strom::derivat *yyparse();
	}
#endif
