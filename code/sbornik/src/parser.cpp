#include "h/parser.hpp"

#define BEZZDROJE std::string::npos
#define POSUN -10

namespace parser{
	unsigned int cislo_radky;
	std::vector<neterm> neterminaly;
	std::vector<pravidlo> gramatika;
	std::vector<token> rozparsovano;

	std::string kod2str(int kod){
		if(kod<=POSUN)
			return neterminaly[POSUN-kod].jmeno;
		else{
			return "'"+scanner::text(kod)+"'";
		}
	}
	neterm::neterm(){
		pocet_cest=0;
	};

	bool pravidlo::operator<(const pravidlo b) const{
		std::list<int>::const_iterator it, jt;
		it=cesta->der.begin();
		jt=b.cesta->der.begin();
		while(it!=cesta->der.end() && jt!=b.cesta->der.end() && *it==*jt){
			++it; ++jt;
		};
		if(it==cesta->der.end() || jt==b.cesta->der.end())
			return delka<b.delka;
		return *it<*jt;
	}

	size_t pridej_najdi(std::string s){
		std::vector<neterm>::iterator it;
		neterm n;

		//najdi:
		size_t i=0;
		for(it=neterminaly.begin(); it!=neterminaly.end(); ++i, ++it){
			if(it->jmeno==s){
				return i;
			}
		}

		//pridej:
		n.jmeno=s;
		neterminaly.push_back(n);
		return neterminaly.end()-neterminaly.begin()-1;
	};

	std::string odescapuj(std::string s){
		size_t na, od=0;

		//if(s=="\\0")	//lambda
		//	return std::string("");

		//apostrof
		while(od<s.length()){
			na=s.find("\\'",od);
			if(na==std::string::npos)
				od=s.length();
			else{
				s.erase(na,1);
				od=na+1;
			}
		}
		return s;
	}

	int cislo_tokenu(std::string s){
		int cislo;
		bool term=s.length()>2 && s[0]=='\'' && s[s.length()-1]=='\'';
		if(term){	//je to terminal
			cislo=scanner::hodnota(odescapuj(s.substr(1,s.length()-2)));
			if(cislo<0){
				error(ERR_UNKNOWN_TERMINAL, cislo_radky, s.substr(1,s.length()-2).c_str());
			}
		}else{		//je to neterminal (ty se udavaji jako POSUN-(index_ve_vektoru)) (tech 9 je pro spec. ucely)
			cislo=POSUN-(int)pridej_najdi(s);
		}
		return cislo;
	}

	int inicializace(std::istream &is){
		size_t delim, zdroj;
		cislo_radky=0;
		zdroj=BEZZDROJE;
		std::string TextRadku, s;
		is>>std::noskipws;

		while(!is.eof()){
			TextRadku="";
			char c=' ';
			//nacti radek:
			while((c != 10)&&(c != 13)&&!is.eof()){
				TextRadku += c;
				is>>c;
			}
			++cislo_radky;
			TextRadku=trimWS(TextRadku);	//odstraò okolní bílé znaky

			if(TextRadku.length()<=0)
				continue;

			switch(TextRadku[0]){
				case '|':	//nova vetev stejneho neterminalu
					if(zdroj==BEZZDROJE){
						error(ERR_BAD_DATA_INPUT, cislo_radky, 1);
						return -1;
					}
					TextRadku.erase(0,1);
					neterminaly[zdroj].cesty.push_back(Cesta() );
					++neterminaly[zdroj].pocet_cest;
					break;
				case ':':	//jmeno neterminalu nesmi zacinat :(==)
					error(ERR_BAD_DATA_INPUT, cislo_radky, 1);
					return -1;
					break;
				case ';':	//konec pravidel pro dany neterminal
					if(zdroj==BEZZDROJE){
						error(ERR_BAD_DATA_INPUT, cislo_radky, 1);
						return -1;
					}else{
						zdroj=BEZZDROJE;
					}
				case '-':	//komentar
					continue;
					break;
				default:	//novy neterminal
					delim=TextRadku.find(":==");
					if(delim==std::string::npos){
						error(ERR_BAD_DATA_INPUT, cislo_radky, 2);
						return -1;
					}
					if(zdroj!=BEZZDROJE){
						error(ERR_BAD_DATA_INPUT,cislo_radky, 1);
						return -1;
					}
					std::string jmeno=trimWS(TextRadku.substr(0,delim));
					TextRadku=trimWS(TextRadku.substr(delim+std::string(":==").length()));
					//pøidej/najdi neterminál (jmeno)
					zdroj=pridej_najdi(jmeno);
					//vytvor dalsi cestu: !!!
					neterminaly[zdroj].cesty.push_back(Cesta() );
					++neterminaly[zdroj].pocet_cest;
			}

			while(TextRadku.length()){	// rozeber retezec potomku podle oddelovace + (a posledni je gramatika ve {})
				delim=TextRadku.find("+");
				if(delim!=std::string::npos){
					s=trimWS(TextRadku.substr(0,delim));
					TextRadku=trimWS(TextRadku.substr(delim+std::string("+").length()));
				}else{	//skoro konec -> odøízni komentáø (pøípadný)
					delim=TextRadku.find("//");
					if(delim!=std::string::npos){
						s=trimWS(TextRadku.substr(0,delim));
					}else{
						s=trimWS(TextRadku);
					}
					TextRadku="";
				};
				if(s.length()>0 && s[0]=='{' && s[s.length()-1]=='}' && TextRadku==""){	//je to semantika
					neterminaly[zdroj].cesty[neterminaly[zdroj].pocet_cest-1].sema=s.substr(1,s.length()-2);
				}else{
					neterminaly[zdroj].cesty[neterminaly[zdroj].pocet_cest-1].der.push_back(cislo_tokenu(s));
				}
			}
		}

		//prehod to do rychlejsi struktury:
		size_t delka=neterminaly.end()-neterminaly.begin();
		pravidlo p;
		for(size_t i=0;i<delka;++i){
			p.c_neterm=POSUN-(int)i;
			for(size_t j=0;j<neterminaly[i].pocet_cest;++j){
				p.cesta=&neterminaly[i].cesty[j];
				p.delka=p.cesta->der.size();
				gramatika.push_back(p);
			}
		}

		std::sort< std::vector<pravidlo>::iterator >(gramatika.begin(),gramatika.end());

#ifdef DEBUG_PRINTS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#endif
		#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_GRAMMAR
			std::cout<<"GRAMATIKA:"<<std::endl;
			std::cout<<"----------"<<std::endl<<std::endl;
			for(std::vector<neterm>::const_iterator it=neterminaly.begin(); it!=neterminaly.end(); ++it){
				std::cout<<it->jmeno<<":"<<std::endl;
				for(std::vector< Cesta >::const_iterator itt=it->cesty.begin(); itt!=it->cesty.end(); ++itt){
					for(std::list<int>::const_iterator ittt=itt->der.begin(); ittt!=itt->der.end(); ++ittt){
						if(*ittt<=POSUN)
							std::cout<<neterminaly[POSUN-*ittt].jmeno<<" ";
						else
							std::cout<<*ittt<<" ";
					}
					std::cout<<std::endl;
				}
				std::cout<<"----------------------------------"<<std::endl;
			}
		#endif
			for(std::vector<parser::neterm>::iterator it=neterminaly.begin(); it!=neterminaly.end(); ++it){
				if(!(it->pocet_cest)){
					error(ERR_UNKNOWN_NONTERMINAL, 0, it->jmeno.c_str());
				}
			}
		return 0;
	}


	//-----------------------------------------------------------------
	//---------------------------- PARSUJ! ----------------------------
	//-----------------------------------------------------------------
	token::token(){
		val=0;
		cislo=0;
		//param;
	}
	//*kvuli warningu:*/	derivacni_strom & derivacni_strom::operator=( const derivacni_strom & ) {};
	derivacni_strom::derivacni_strom(const std::vector<token> &terminaly, const std::vector<pravidlo> &gramat):gram(gramat){
		delka_cesty=0;
		for(std::vector<token>::const_iterator it=terminaly.begin(); it!=terminaly.end(); ++it){
			uzly.push_back(uzel(hrana(delka_cesty, delka_cesty+1, gram.end(), it->val
				          )     )     );
			++delka_cesty;

		}
		//posledni uzel neni potreba - z nej nic nevede
	}

	derivacni_strom::hrana::hrana(size_t ze, size_t ke, const std::vector<pravidlo>::const_iterator &it, int v, const std::vector<uz_hr> &naslednici){
		z=ze;
		k=ke;
		val=v;
		prav=it;
		syni=naslednici;
		ok=false;
	};
	derivacni_strom::hrana::hrana(size_t ze, size_t ke, const std::vector<pravidlo>::const_iterator &it, int v){
		z=ze;
		k=ke;
		val=v;
		prav=it;
		ok=false;
	};

	derivacni_strom::uzel::uzel(){};
	derivacni_strom::uzel::uzel(const derivacni_strom::hrana &h){
		hrany.push_back(h);
	}
	bool derivacni_strom::je_hrana(size_t z, size_t k, int val){
		if(z>=delka_cesty)
			return false;
		for(std::vector<hrana>::const_iterator it=uzly[z].hrany.begin(); it!=uzly[z].hrany.end(); ++it){
			if(it->k==k && it->val==val) return true;
		}
		return false;
	}

	bool derivacni_strom::zkrat_cestu(
		const std::vector<pravidlo>::const_iterator &it_prav,
		const std::list<int>::const_iterator cesta /*teda jeji zbyvajici cast*/,
		const size_t &start,
		size_t pozice,
		std::vector<uz_hr> &pouzite_hrany
	){
		if(cesta==it_prav->cesta->der.end()){	//nalezeno
			if(je_hrana(start,pozice, it_prav->c_neterm))	//nic noveho
				return false;
			uzly[start].hrany.push_back(hrana(start, pozice, it_prav, it_prav->c_neterm, pouzite_hrany));
			return true;
		}
		if(pozice>=delka_cesty)	//nelze
			return false;
		//pres vsechny hrany v uzlu:
		bool toret=false;
		uz_hr uh;
		uh.uzlik=pozice;
		for(size_t it=0; it<uzly[pozice].hrany.size(); ++it){
			if(uzly[pozice].hrany[it].val==*cesta){
				std::list<int>::const_iterator cesta1=cesta;
				uh.hranik=it;
				pouzite_hrany.push_back(uh);
				toret=toret || zkrat_cestu(it_prav, ++cesta1, start, uzly[pozice].hrany[it].k, pouzite_hrany);
				pouzite_hrany.pop_back();
			}
		}
		return toret;
	}

	bool derivacni_strom::build(){ // O(|G|*h) - to je celkem dost - chce to efektivneji prorezavat
		bool zmeny=true;
		std::vector<uz_hr> pouzite_hrany; //pomocna pro rekurzi -> seznam hran, ze kterych se prave odvozuje
		while(zmeny){
			zmeny=false;
			//pro kazde pravidlo:
			for(std::vector<pravidlo>::const_iterator it_pravidlo=gram.begin(); it_pravidlo!=gram.end(); ++it_pravidlo){
				//zkus start v kazdem uzlu:
				for(size_t it_uzel=0; it_uzel<delka_cesty; ++it_uzel){
					//uz se tam pravidla nemohou vejit (lambda je zakazana)
					if(it_pravidlo->delka>delka_cesty-it_uzel) break; //prorezani
					zmeny=zmeny||zkrat_cestu(it_pravidlo, it_pravidlo->cesta->der.begin(), it_uzel, it_uzel, pouzite_hrany);
				}
			}
		}
		return je_hrana(0,delka_cesty,POSUN); //byl nalezen pocatecni neterminal?
	};

	derivacni_strom::derivat* derivacni_strom::preved(){
		if(delka_cesty==0) return 0;
		for(std::vector<hrana>::const_iterator hr=uzly[0].hrany.begin(); hr!=uzly[0].hrany.end(); ++hr){
			if(hr->val==POSUN && hr->k==delka_cesty){
				return preved_podstrom(hr);
			}
		}
		return 0;
	}

	derivacni_strom::derivat* derivacni_strom::preved_podstrom(std::vector<hrana>::const_iterator hr){
		derivat *koren=new derivat;
		koren->z=hr->z;
		koren->k=hr->k;
		koren->hodnota=hr->val;
		koren->prav=hr->prav;
		for(std::vector<uz_hr>::const_iterator it=hr->syni.begin(); it!=hr->syni.end(); ++it){
			koren->potomci.push_back(preved_podstrom(uzly[it->uzlik].hrany[it->hranik]));
		}
		return koren;
	}
	derivacni_strom::derivat* derivacni_strom::preved_podstrom(hrana hr){
		derivat *koren=new derivat;
		koren->z=hr.z;
		koren->k=hr.k;
		koren->hodnota=hr.val;
		koren->prav=hr.prav;
		for(std::vector<uz_hr>::const_iterator it=hr.syni.begin(); it!=hr.syni.end(); ++it){
			koren->potomci.push_back(preved_podstrom(uzly[it->uzlik].hrany[it->hranik]));
		}
		return koren;
	}

	void derivacni_strom::tiskni_hrany(){
		for(std::vector<uzel>::const_iterator uz=uzly.begin(); uz!=uzly.end(); ++uz){
			for(std::vector<hrana>::const_iterator hr=uz->hrany.begin(); hr!=uz->hrany.end(); ++hr){
				std::cout<<"("<<hr->z<<"->"<<hr->k<<") - "<<kod2str(hr->val)<<std::endl;
			}
		}

		return;
	}

	void derivacni_strom::tiskni_strom(std::vector<hrana>::const_iterator koren, int indent=0) const{
		for(int i=0;i<indent;++i)
			std::cout<<"    ";
		std::cout<<"("<<koren->z<<"->"<<koren->k<<") - "<<kod2str(koren->val)<<std::endl;
		for(std::vector<uz_hr>::const_iterator syn=koren->syni.begin(); syn!=koren->syni.end(); ++syn){
			tiskni_strom(syn, indent+1);
		}
	};

	void derivacni_strom::tiskni_strom(std::vector<uz_hr>::const_iterator uh, int indent=0) const{
		hrana koren=uzly[uh->uzlik].hrany[uh->hranik];
		for(int i=0;i<indent;++i)
			std::cout<<"    ";
		std::cout<<"("<<koren.z<<"->"<<koren.k<<") - "<<kod2str(koren.val)<<std::endl;
		for(std::vector<uz_hr>::const_iterator syn=koren.syni.begin(); syn!=koren.syni.end(); ++syn){
			tiskni_strom(syn, indent+1);
		}
	}
	int derivacni_strom::tiskni_stromy(bool jenjeden=false) const{
		int pocet=0;
		//int pocet2=0;
		for(std::vector<uzel>::const_iterator uz=uzly.begin(); uz!=uzly.end(); ++uz){
			for(std::vector<hrana>::const_iterator hr=uz->hrany.begin(); hr!=uz->hrany.end(); ++hr){
				if(hr->val==POSUN && (!jenjeden || (hr->k==delka_cesty && hr->z==0))){
					tiskni_strom(hr);
					++pocet;
					std::cout<<std::endl;
				}
		//		if(hr->val==POSUN)
		//			++pocet2;
			}
		}
		return pocet;
		//return pocet2;
	}

	void derivacni_strom::smaz_Dstrom(derivacni_strom::derivat *koren){
		if(!koren) return;
		for(std::vector<derivat*>::const_iterator it=koren->potomci.begin(); it!=koren->potomci.end(); ++it){
			smaz_Dstrom(*it);
		}
		delete koren;
	}
//-----
	derivacni_strom::derivat *yyparse(){
		token t;

		while((t.val=scanner::yylex(&t.param, t.cislo))!=0){ //pro blby prekladace
			rozparsovano.push_back(t);
		};
		derivacni_strom DT(rozparsovano, gramatika);
#ifdef DEBUG_PRINTS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#endif
#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_EDGES
		std::cout<<"SLOZKOVY STROM /hrany/:"<<std::endl;
		std::cout<<"-----------------------"<<std::endl;
		if(DT.build())
			std::cout<<"Koren '"<<neterminaly[0].jmeno<<"' nalezen!"<<std::endl<<std::endl;
		else
			std::cout<<"Koren '"<<neterminaly[0].jmeno<<"' nenalezen!"<<std::endl<<std::endl;
		DT.tiskni_hrany();
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#else
		if(!DT.build()){
			error(ERR_SYNTAX,0);
			DT.tiskni_hrany();
		}
#endif
#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_DTREE
		std::cout<<"SLOZKOVY STROM /strom/:"<<std::endl;
		std::cout<<"-----------------------"<<std::endl;
		int i=DT.tiskni_stromy(true);
		std::cout<<std::endl<<"-------------------"<<std::endl<<"Pocet stromu je: "<<i<<std::endl;
#endif
		return DT.preved();
	}
}
