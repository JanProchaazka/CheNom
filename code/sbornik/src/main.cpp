#include "h/main.hpp"

std::string ctiINI(char* knihovna, std::string pole, std::string klic){
	std::ifstream is_knihovna;
	is_knihovna.open(knihovna);
	if(!is_knihovna.good()){
		error(ERR_NO_DATA_INPUT, 0, knihovna);
		return std::string("");
	}
	std::string s, s1, s2;
	pole="["+pole+"]";
	//POLE
	while(s!=pole){
		if(is_knihovna.eof()){
			error(ERR_BAD_SETTINGS_FILE, 0, pole.c_str(), klic.c_str());
			return std::string("");
		}
		is_knihovna>>s;
	}
	//KLIC
	while(s!=klic){
		is_knihovna>>s>>s1>>s2;
		if(s1!="=" || is_knihovna.eof()){
			error(ERR_BAD_SETTINGS_FILE, 0, pole.c_str(), klic.c_str());
			return "";
		}
	}
	is_knihovna.close();
	return s2;
}

std::string adr(std::string path){
	size_t idx=path.find_last_of(PATHDELIM,path.length());
	if(idx==std::string::npos){
		return std::string(".")+PATHDELIM;
	}else{
		return path.substr(0,idx+1);
	}
}

int main(int argc, char** argv)
{
	was_error=0;
#if defined WINDOWS && defined DEBUG
	setlocale( LC_ALL, ".1250"); //zmena kodovani na cp1250 - tj. kodovani ve VS -> citelnost i pri ladeni
#endif

    if(argc<2){
		error(ERR_BAD_SYNTAX_INPUT, 0);
		return was_error;
	}
	std::string s;
	std::string settings_adr=adr(argv[1]);
	std::ifstream	is_tree, is_inter, is_gram, is_sema, is_graf;

	//konstant - [0;9], [a;z], ..
	s=settings_adr+ctiINI(argv[1], "Soubory", "Konstanty");
	is_inter.open(s.c_str());
	if(!is_inter.good()){
		error(ERR_NO_DATA_INPUT, 0, s.c_str());
		return was_error;
	}
	scanner::inicializace_intervalu(is_inter);
	is_inter.close();

	//postav lexikalni strom
	s=settings_adr+ctiINI(argv[1], "Soubory", "Terminaly");
	is_tree.open(s.c_str());
	if(!is_tree.good()){
		error(ERR_NO_DATA_INPUT, 0, s.c_str());
		return was_error;
	}
	scanner::inicializace(is_tree);
	is_tree.close();

	//nacti gramaticka pravidla
	s=settings_adr+ctiINI(argv[1], "Soubory", "Gramatika");
	is_gram.open(s.c_str());
	if(!is_gram.good()){
		error(ERR_NO_DATA_INPUT, 0, s.c_str());
		return was_error;
	}
	if(parser::inicializace(is_gram)){
		return was_error;
	}
	is_gram.close();

	//nacti tabulku prvku
	s=settings_adr+ctiINI(argv[1], "Soubory", "Prvky");
	is_sema.open(s.c_str());
	if(!is_sema.good()){
		error(ERR_NO_DATA_INPUT, 0, s.c_str());
		return was_error;
	}
	semantika::inicializace(is_sema);
	is_sema.close();

	//nacti sablonu vystupu
	s=settings_adr+ctiINI(argv[1], "Soubory", "Vystup");
	is_graf.open(s.c_str());
	if(!is_graf.good()){
		error(ERR_NO_DATA_INPUT, 0, s.c_str());
		return was_error;
	}
	grafika::inicializace(is_graf);
	is_graf.close();

#ifdef DEBUG_PRINTS
	std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
	std::cout<<"Zadejte vstup:"<<std::endl;
#endif
	std::string jmeno_mol;
/**/ #define DEBUG_COUT /**/
#ifdef DEBUG_CIN
	std::cout<<(jmeno_mol=DEBUG_VSTUP);
#else
	std::cin>>jmeno_mol;
if(jmeno_mol.length()>1 && jmeno_mol[0]=='"' && jmeno_mol[jmeno_mol.length()-1]=='"'){
	jmeno_mol.erase(jmeno_mol.length()-1);
	jmeno_mol.erase(0,1);
}

if(argc==3){		//pokud je pouzit option -e(xtendet), tak se jmeno odkoduje
	if(std::string(argv[2])!=std::string("-e")){
		error(ERR_BAD_SYNTAX_INPUT, 0);
		return was_error;
	}
	jmeno_mol=urldecode(jmeno_mol);
}

#endif
#ifdef DEBUG_PRINTS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#endif

	scanner::parse(jmeno_mol);
#ifdef DEBUG_LEXONLY
	//jen lexikalni:
	lexval *ggg=new lexval;
	size_t start=0;
	int yyret=1;
	unsigned int cislotokenu=0;
	while(yyret){
		ggg->clear();
		yyret=scanner::yylex(ggg,cislotokenu);
		#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_TOKENS
			if(yyret){
				std::cout<<jmeno_mol.substr(start,ggg->_len)<<
					(jmeno_mol.substr(start,ggg->_len).length()>6?":\t":":\t\t");
			}else{
				std::cout<<"<<EOF>>:\t";
			}
				std::cout<<yyret<<"\t"<<*ggg<<std::endl;
				start+=ggg->_len;
		#endif
	}
	delete ggg;
#else
	#ifdef DEBUG_SYNONLY
		//jen syntax:
		parser::derivacni_strom::smaz_Dstrom(parser::yyparse());
		std::cout<<std::endl;
	#else
		#ifdef DEBUG_SEMONLY
			//jen semantiku:
			semantika::skelet molekula=semantika::yysem();
		#else
			if(was_error>WARNING) return was_error;
			//i modelovani:
			model::yymodel(semantika::yysem());
		#endif
	#endif
#endif

#ifdef DEBUG
	#ifdef WINDOWS
 		system("pause");
	#else
		system("echo \"Pokraèujte stisknutím klávesy enter...\"; read");
	#endif
#endif
	return was_error;
}
