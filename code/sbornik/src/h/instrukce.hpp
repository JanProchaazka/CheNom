#ifndef CHENOM_INSTRUKCE_H_
#define CHENOM_INSTRUKCE_H_
	#include <vector>
	#include <string>
	#include "err.hpp"
	#include "utils.hpp"

namespace semantika{
	struct Prvek{	//vlastnosti prvku (jedne konkretni konfigurace)
		std::string Zn;		//znacka prvku
		size_t		Z;		//protonove cislo
		size_t		arita;	//pocet vazeb
		std::string	hybrid;	//hybridizace
		double		R;		//polomer
		double		m;		//hmotnost
		std::string Col;	//barva - #rrggbb - ale pro VRML bude potreba rozdelit na slozky a vydelit 255
	};
	struct vazba{	//vazba ve strukture atom
		unsigned char	cetnost;	//jednoducha/dvojna/trojna/..
		size_t			cil;		//druhy konec vazby (npos = volna vazba)
	};

	struct atom{	//polozka skeletu
		std::vector<Prvek>::const_iterator	jadro;	//pointer na prvek (znacka, hybridizace, barva, ..)
		char								naboj;	//naboj atomu
		std::vector<std::string>			jmena;	//vsechna alternativni jmena (1,<alpha/>,1',..)
		std::vector<vazba>					vazby;	//vsechny z tohoto atomu vychazejici vazby - kam,kolik .. co chybi = doplnit -H
		void posun_vazby(size_t o, bool smer);		//zvysi cil vsech vazeb o (smer?-o:+o)
		std::string tisk() const;					//vypise hezky naformatovany atom
	};

	class skelet{	//polotovar i final molekuly
		std::vector<atom>	atomy;	//atomy a vazby v skeletu
	public:
		skelet();																//vytvori 0.atom na volne vazby
		std::string tisk() const;												//vypise hezky naformatovanou molekulu
		size_t najdi_atom_idx(const std::string &Jmeno)const;					//najde prvni atom, jehoz jmeno je Jmeno a vrati jeho index
		size_t posl_atom_idx()const;											//najde posledni atom, ktery ma platne jmeno a vrati jeho index
		atom dej_atom(size_t idx)const;											//vrati idx-ty atom z atomy
		bool je_vazba(size_t Z, size_t K)const;									//je mezi Z-tym a K-tym atomem vazba
		void pridej_atom(const std::string &Znacka, const std::string &Jmeno);	//prida novy atom do seznamu
		void pridej_atom(const atom &a);										//pridej "stary" atom do seznamu :)
		void pridej_vazbu(const std::string &Z, const std::string &K, unsigned char cetnost);	//pridej cetnost-arni vazbu mezi atomy Z a K
		void pridej_vazbu(size_t idxZ, size_t idxK, unsigned char cetnost);						//pridej cetnost-arni vazbu mezi atomy idxZ a idxK
		void zvys_vazbu(size_t idxZ, size_t idxK, unsigned char cetnost);						//zvysi aritu vazby mezi idxZ a idxK o cetnost
		void zvys_vazbu(const std::string &Z, const std::string &K, unsigned char cetnost);		//zvysi aritu vazby mezi Z a K o cetnost
		void zrus_vazbu(const std::string &Z, const std::string &K);							//smaze vazbu mezi atomy Z a K
		void zrus_vazbu(size_t idxZ, size_t idxK);												//smaze vazbu mezi atomy idxZ a idxK
		void sniz_vazbu(const std::string &Z, const std::string &K, unsigned char cetnost);		//snizi aritu mezi atomy Z a K o cetnost, a pokud uz zmizi, tak se smaze
		void presun_vazbu(const std::string &Z1, const std::string &K1, const std::string &Z2, const std::string &K2);	//presune vazbu mezi Z1-K1 na vazbu mezi Z2-K2
		unsigned char najdi_cetnost(size_t idxZ,size_t idxK)const;				//vrati cetnost vazby mezi atomy idxZ a idxK (není = 0)
		size_t pocet_atomu()const;												//vrati delku vektoru atomy
		void pripoj(const skelet &co, bool zneplatni);							//pripoji skelet co (prida atomy, slouci pocatky a nastavi vazby jako korektni), pripojovanym atomum zneplatni jmena pokud neni toto vypnuto pomoci zneplatni = false
		void volne_vazby(const std::string &kam, size_t za);					//vezme vsechny volne vazby z 1 atomu za indexem za a pripoji je na atom cim
		void zamen_atom(const skelet &cim, const std::string &co);				//zameni atom za lokantem co prvnim atomem za lokantem cim (pokud jich je vic, hlasi error)
		void zamen_atom(const skelet &cim, size_t cil_idx);						//zameni atom cil_idx prvnim atomem za lokantem cim (pokud jich je vic, hlasi error)
		void smaz_atom(const std::string &co);									//smaze atom za lokantem co, pokud z nej vede 1 vazba, tak ji smaze, pokud 2 stejne arity, tak je propoji, jinak hlasi error
		void smaz_atom_bez_vazeb(const std::string &co);						//smaze atom za lokantem co, vazby z nej vedouci necha jako volne
		void nastav_naboj(size_t idxkde, char kolik);							//u atomu na pozici idxkde zmeni naboj prictenim kolik..
		void aplikuj(const std::string &kde, const skelet &co);					//aplikuje na kde vse co je platne v co (1 atom!)
		void aplikuj(size_t idxkde, const skelet &co);							//aplikuje na idxkde vse co je platne v co (1 atom!)
		void resetuj_jmena();													//smaze vsechna jmena a vlozi jmena odpovidajici pozici (tj. 1. atom ~1~, 2. atom ~2~, ...)
		void pricarkuj();														//vezme vsechna jmena a provede +="'"
		void finalizuj();														//doplò vodíky, a otestuj strukturu, zda je to ok molekula
	};

	class ntika{	//uzel semantickeho stromu
	public:
		size_t c;						//cislo .. urceno pro vycisleni vyrazu typu tetratriakonta
		std::string s;					//retezec .. urceno pro sestaveni lokantu
		std::vector<std::string> z,k;	//dvojice retezcu .. ruceno pro sestaveni skupiny vazeb
		skelet m;						//molekula .. skelet molekuly nad kterym se pracuje operacemi tak, aby vznikla finalni molekula

		ntika();
	};

	void prvky_init(std::istream &is);
	namespace INT{
		void NEW(std::string params, std::vector<ntika> &DATA);
		void ADD(std::string params, std::vector<ntika> &DATA);
		void MUL(std::string params, std::vector<ntika> &DATA);
	}
	namespace STR{
		void NEW(std::string params, std::vector<ntika> &DATA);
		void APP(std::string params, std::vector<ntika> &DATA);
	}
	namespace MOL{
		void ZDO(std::string params, std::vector<ntika> &DATA);
		void LAP(std::string params, std::vector<ntika> &DATA);
		void NWA(std::string params, std::vector<ntika> &DATA);
		void NEW(std::string params, std::vector<ntika> &DATA);
		void CPY(std::string params, std::vector<ntika> &DATA);
		void ION(std::string params, std::vector<ntika> &DATA);
		void APP(std::string params, std::vector<ntika> &DATA);
		void ADD(std::string params, std::vector<ntika> &DATA);
		void CON(std::string params, std::vector<ntika> &DATA);
		void COI(std::string params, std::vector<ntika> &DATA);
		void CHN(std::string params, std::vector<ntika> &DATA);
		void LIN(std::string params, std::vector<ntika> &DATA);
		void MVV(std::string params, std::vector<ntika> &DATA);
		void SUB(std::string params, std::vector<ntika> &DATA);
		void DLA(std::string params, std::vector<ntika> &DATA);
		void DLX(std::string params, std::vector<ntika> &DATA);
		void DLV(std::string params, std::vector<ntika> &DATA);
		void DEC(std::string params, std::vector<ntika> &DATA);
		void INC(std::string params, std::vector<ntika> &DATA);
		void APL(std::string params, std::vector<ntika> &DATA);
		void FIN(std::string params, std::vector<ntika> &DATA);
	}

	extern std::vector<Prvek> prvky;
}
#endif
