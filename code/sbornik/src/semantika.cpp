#include "h/semantika.hpp"

namespace semantika{

	void inicializace(std::istream &is){
		prvky_init(is);		//nacti prvky
		return;
	}

//----------------------------------
	std::string pop_prikaz(std::string &prikazy){
		size_t delim;
		bool instr=false;
		std::string prikaz;

		for(delim=0; delim<prikazy.length(); ++delim){
			if(instr){
				if(prikazy[delim]=='~')
					instr=false;
			}else{
				switch(prikazy[delim]){
					case '~':
						instr=true;
						break;
					case ';':	//to jsem hledal
						prikaz=prikazy.substr(0,delim);
						prikazy.erase(0,delim+1);
						return prikaz;
						break;
					default:
						;//...
				}
			}
		}

		error(ERR_INCOR_COMMAND, 0, prikazy.c_str());
		prikazy="";
		return "";
	};

	bool parse_prikaz(std::string prikaz, std::string &dest, std::string &instr, std::string &params){
		std::string cele=prikaz;
		/* DEST */
		if(prikaz.length()<3){
			error(ERR_INCOR_COMMAND, 0, cele.c_str());
			return false;
		};
		dest=prikaz.substr(0,3);
		prikaz.erase(0,3);

		/* :: */
		if(prikaz.length()<2 || prikaz[0]!=':' || prikaz[1]!=':'){
			error(ERR_INCOR_COMMAND, 0, cele.c_str());
			return false;
		}
		prikaz.erase(0,2);

		/* INSTR */
		if(prikaz.length()<3){
			error(ERR_INCOR_COMMAND, 0, cele.c_str());
			return false;
		}
		instr=prikaz.substr(0,3);
		prikaz.erase(0,3);

		/* PARAM */
		if(prikaz.length()<2 || prikaz[0]!='(' || prikaz[prikaz.length()-1]!=')'){
			error(ERR_INCOR_COMMAND, 0, cele.c_str());
		}
		params=prikaz.substr(1,prikaz.length()-2);

		/* ---- */
		return true;
	}

	void zpracuj_prikaz_INT(const std::string &instr, std::string params, std::vector<ntika> &DATA){
		if(instr=="NEW"){	//vytvor/kopiruj NEW(int_expr);
			INT::NEW(params, DATA); return;
		}
		if(instr=="ADD"){	//pricti		ADD(DEST,SRC); DEST,SRC=int_expr
			INT::ADD(params, DATA); return;
		}
		if(instr=="MUL"){	//prenasob		MUL(DEST,SRC); DEST,SRC=int_expr
			INT::MUL(params, DATA); return;
		}
		error(ERR_UNKNOWN_COMMAND, 0, std::string("INT::"+instr).c_str());
		return;
	}
	void zpracuj_prikaz_STR(const std::string &instr, std::string params, std::vector<ntika> &DATA){
		if(instr=="NEW"){	//vytvor
			STR::NEW(params, DATA); return;
		}
		if(instr=="APP"){	//spoji (2) parametry a da je do korene
			STR::APP(params, DATA); return;
		}
		error(ERR_UNKNOWN_COMMAND, 0, std::string("STR::"+instr).c_str());
		return;
	}
	void zpracuj_prikaz_MOL(const std::string &instr, std::string params, std::vector<ntika> &DATA){
		if(instr=="ZDO"){	//vytvor lokant/dvojici lokantu
			MOL::ZDO(params, DATA); return;
		}
		if(instr=="LAP"){	//append seznamy lokantu
			MOL::LAP(params, DATA); return;
		}
		if(instr=="NWA"){	//vytvor novy atom
			MOL::NWA(params, DATA); return;
		}
		if(instr=="NEW"){	//vytvor novy skelet
			MOL::NEW(params, DATA); return;
		}
		if(instr=="CPY"){	//kopiruj skelet
			MOL::CPY(params, DATA); return;
		}
		if(instr=="ION"){	//pridej atomu naboj
			MOL::ION(params, DATA); return;
		}
		if(instr=="ADD"){	//sluc neuplne skelety
			MOL::ADD(params, DATA); return;
		}
		if(instr=="APP"){	//pripoj pres vodiky
			MOL::APP(params, DATA); return;
		}
		if(instr=="CON"){	//pripoj pres volne vazby
			MOL::CON(params, DATA); return;
		}
		if(instr=="COI"){	//pripoj pres volnou vazbu
			MOL::COI(params, DATA); return;
		}
		if(instr=="CHN"){	//zretez (CON) - kolikrat
			MOL::CHN(params, DATA); return;
		}
		if(instr=="LIN"){	//zretez (APP) - kolikrat
			MOL::LIN(params, DATA); return;
		}
		if(instr=="MVV"){	//presun vazbu
			MOL::MVV(params, DATA); return;
		}
		if(instr=="SUB"){	//zamen atom za atom
			MOL::SUB(params, DATA); return;
		}
		if(instr=="DLA"){	//smaz atom z cyklu/retezce
			MOL::DLA(params, DATA); return;
		}
		if(instr=="DLX"){	//smaz atom, ale vazby nech volne
			MOL::DLX(params, DATA); return;
		}
		if(instr=="DLV"){	//smaz vazbu
			MOL::DLV(params, DATA); return;
		}
		if(instr=="DEC"){	//sniz aritu vazby
			MOL::DEC(params, DATA); return;
		}
		if(instr=="INC"){	//zvys aritu vazby
			MOL::INC(params, DATA); return;
		}
		if(instr=="APL"){	//aplikuj vse platne ze skeletu
			MOL::APL(params, DATA); return;
		}
		if(instr=="FIN"){	//doplni chybejici vodiky a provede zaverecne testy
			MOL::FIN(params, DATA); return;
		}
		error(ERR_UNKNOWN_COMMAND, 0, std::string("MOL::"+instr).c_str());
		return;
	}

	void zpracuj_prikaz(const std::string &dest, const std::string &instr, std::string params, std::vector<ntika> &DATA){
		if(dest=="INT"){
			zpracuj_prikaz_INT(instr, params, DATA);
			return;
		}
		if(dest=="STR"){
			zpracuj_prikaz_STR(instr, params, DATA);
			return;
		}
		if(dest=="MOL"){
			zpracuj_prikaz_MOL(instr, params, DATA);
			return;
		}
		error(ERR_UNKNOWN_COMMAND, 0, std::string(dest+"::"+instr).c_str());
		return;
	}

	ntika vyhodnot(parser::derivacni_strom::derivat *koren){
		std::vector<ntika> polozky;
		polozky.push_back(ntika());	//sam sebe na pozici 0

		if(!koren){
			return ntika();
		}

		if(koren->prav==parser::gramatika.end()){									//prazdno - list .. list nema pravidlo
			return ntika();
		}

		if(koren->potomci.begin()==koren->potomci.end()){	//prazdno - zadny potomek = list .. list nema pravidlo (toto by melo spadnout do predchoziho pravidla
			return ntika();
		}

		if(koren->prav->cesta->sema.length()==0){		//zatim nema implementovany vyklad
			error(ERR_NO_COMMAND, 0, parser::kod2str(koren->hodnota).c_str());
			//return ntika();	//zatim ne, abych se dozvedel zbytek
		}
#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_COMMANDS
		else std::cout<<koren->prav->cesta->sema<<std::endl;
#endif

		for(std::vector<parser::derivacni_strom::derivat*>::const_iterator it=koren->potomci.begin(); it!=koren->potomci.end(); ++it){
			////if((*it)->potomci.size()==0){}
			ntika n=vyhodnot(*it);
			polozky.push_back(n);
			//polozky.push_back(vyhodnot(*it));
		}

		//---- zpracuj prikaz:
		std::string prikazy=koren->prav->cesta->sema;	//sekvence prikazu u daneho semantickeho pravidla
		std::string prikaz;								//prave zpracovavany prikaz v sekvenci
		std::string dest;								//cil prikazu (polozka)
		std::string instrukce;							//aktualne provadena instrukce
		std::string parametry;							//parametry prikazu

		while(prikazy.length()!=0){
			prikaz=pop_prikaz(prikazy);
			parse_prikaz(prikaz,dest,instrukce,parametry);
			zpracuj_prikaz(dest, instrukce, parametry, polozky);
		}
		return polozky[0];
	}

//------------------------------------
	skelet yysem(){			// vlastni zpracovani
		parser::derivacni_strom::derivat *DTree=parser::yyparse();

		skelet molekula=vyhodnot(DTree).m;

		parser::derivacni_strom::smaz_Dstrom(DTree);

#ifdef DEBUG_PRINTS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#endif
		#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_SKELET
			std::cout<<"MOLEKULA /skelet/:"<<std::endl;
			std::cout<<"------------------"<<std::endl;
			std::cout<<molekula.tisk();
			std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
		#endif

		return molekula;
	}
}
