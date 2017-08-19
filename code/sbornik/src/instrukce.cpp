#include "h/instrukce.hpp"
namespace semantika{

	std::vector<Prvek> prvky;

	//--------------------------------------------------------------------------------------
	//blbinky:
/**/void prvky_init(std::istream &is){		//nacti prvky
		Prvek p;			//prave nacitany prvek
		char sgn;			//uvodni dvojznak rozlisujici komentar (--) od dat (>>)
		std::string koment;	//komentar

		p.Zn="."; //pro volne vazby
		p.Z=0;
		p.arita=std::string::npos;
		p.hybrid="";
		p.R=0;
		p.m=0;
		p.Col="#000000";

		//prvky.push_back(p);

		while(is.good()){
			is>>sgn;koment=sgn;is>>sgn;	koment+=sgn;	//nacti prvni dva znaky
			if(koment==">>"){	//je to prvek
				is>>p.Zn>>p.Z>>p.arita>>p.hybrid>>p.R>>p.m>>p.Col;
				//zpracuj
				prvky.push_back(p);
			}else{			//je to koment
				is>>std::noskipws;
				sgn=' ';
				while(is.good() && sgn!=10 && sgn!=13){	//cti zbytek radku
					is>>sgn;
				}
				is>>std::skipws;
			}
		}
#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_ATOMS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
		for(std::vector<Prvek>::const_iterator it=prvky.begin(); it!=prvky.end(); ++it){
			std::cout<<it->Z<<" "<<it->Zn<<"\t"<<it->arita<<" "<<std::endl;
		}
#endif
#ifdef DEBUG_PRINTS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#endif
	}
	bool prazdny_seznam(const std::vector<std::string> &strs){	//vrati, jestli jsou vsechny retezce prazdne
		for(std::vector<std::string>::const_iterator it=strs.begin(); it!=strs.end(); ++it){
			if(it->length()!=0)
				return false;
		}
		return true;
	}

	//--------------------------------------------------------------------------------------
	//strukty:
/**/ntika::ntika(){	//prazdny ctor
		c=0;
		s="";
	}
/**/std::vector<Prvek>::const_iterator najdi_atom(const std::string &Znacka, size_t Arita=std::string::npos){
		if(Znacka==".")	//pro 0. atom na volne vazby
			return prvky.end();
		for(std::vector<Prvek>::const_iterator it=prvky.begin();it!=prvky.end();++it){
			if(it->Zn==Znacka && (Arita==std::string::npos || it->arita==Arita)){
				return it;
			}
		}
		error(ERR_NO_ATOM_NAME, 0, Znacka.c_str());
		return prvky.end();
	}
/**/std::string atom::tisk() const{
		std::string vystup;
		//[znacka_arita(naboj)]
		vystup+="[";
		if(jadro!=prvky.end())
			vystup+=jadro->Zn+(jadro->Zn.length()==1?"  ":" ")+int2str(jadro->arita)+"("+(naboj>=0?"+":"-")+int2str(abs(naboj))+")";
		else
			vystup+=" ------ ";
		vystup+="]";
		//{seznam|jmen}
		vystup+="\t{";
		for(std::vector<std::string>::const_iterator itN=jmena.begin(); itN!=jmena.end(); ++itN){
			vystup+=*itN+'|';
		}
		if(vystup[vystup.length()-1]=='|') vystup.erase(vystup.length()-1);
		vystup+="}";
		//[(-seznam)(=vazeb)]
		vystup+="\t[";
		for(std::vector<vazba>::const_iterator itV=vazby.begin(); itV!=vazby.end(); ++itV){
			vystup+="(";
			switch(itV->cetnost){
				case 1:
					vystup+="-";
					break;
				case 2:
					vystup+="=";
					break;
				case 3:
					vystup+="%";
					break;
				default:
					vystup+="-{"+int2str(itV->cetnost)+"}-";
					break;
			}
			vystup+=int2str(itV->cil);
			vystup+=")";
		}
		vystup+="]";
		//------------
		return vystup;
	}
	void atom::posun_vazby(size_t o, bool smer=false){
		for(std::vector<vazba>::iterator itV=vazby.begin(); itV!=vazby.end(); ++itV){
			if(itV->cil!=0)
				if(smer)
					itV->cil-=o;
				else
					itV->cil+=o;
		}
	}
	//--------------------------------------------------------------------------------------
	//skelet:
/**/skelet::skelet(){
		pridej_atom(".", "");	//pridej atom pro prazdne vazby
	}
/**/std::string skelet::tisk()const{
		std::string vystup;
		size_t i=0;
		for(std::vector<atom>::const_iterator itA=atomy.begin(); itA!=atomy.end(); ++itA, ++i){
			vystup+=int2str(i)+"\t";
			vystup+=itA->tisk();
			vystup+="\n";
		}
		return vystup;
	}
	bool skelet::je_vazba(size_t Z, size_t K)const{
		if(Z>=atomy.size())
			error(ERR_NO_ATOM_INDEX,0,Z);
		if(K>=atomy.size())
			error(ERR_NO_ATOM_INDEX,0,K);

		if(Z>=atomy.size() || K>=atomy.size()){
			return false;
		}

		for(std::vector<vazba>::const_iterator it=atomy[Z].vazby.begin(); it!=atomy[Z].vazby.end(); ++it){
			if(it->cil==K){
				//for(std::vector<vazba>::const_iterator it=atomy[K].vazby.begin(); it!=atomy[K].vazby.end(); ++it){
				//	if(it->cil==Z){
						return true;
				//	}
				//}
				// error(vazba vede jen zpet:D)
				//return false;
			}
		}

		return false;
	}
/**/size_t skelet::najdi_atom_idx(const std::string &Jmeno)const {
		size_t idx=0;

		for(std::vector<atom>::const_iterator it=atomy.begin(); it!=atomy.end(); ++it, ++idx){
			for(std::vector<std::string>::const_iterator itS=it->jmena.begin(); itS!= it->jmena.end(); ++itS){
				if(*itS == Jmeno){
					return idx;
				}
			}
			if(it->jadro!=prvky.end() && it->jadro->Zn == Jmeno){
				return idx;
			}
		}
		return std::string::npos;
	}
	size_t skelet::posl_atom_idx()const{
		for(size_t idx=atomy.size()-1;idx>0;--idx){
			for(std::vector<std::string>::const_iterator itS=atomy[idx].jmena.begin(); itS!= atomy[idx].jmena.end(); ++itS){
				if((*itS)[0] != '!'){
					return idx?idx:std::string::npos;
				}
			}
		}
		return std::string::npos;
	}
	atom skelet::dej_atom(size_t idx)const{
		if(idx>atomy.size()){
			error(ERR_NO_ATOM_INDEX, 0, idx);
			return atom();
		}
		return atomy[idx];
	}
/**/void skelet::pridej_atom(const std::string &Znacka, const std::string &Jmeno){
		atom a;
		a.jadro=najdi_atom(Znacka);
		//a.jmena.push_back(Znacka);
		a.naboj=0;
		if(Jmeno.length()!=0)
			a.jmena.push_back(Jmeno);
		atomy.push_back(a);
		return;
	}
/**/void skelet::pridej_atom(const atom &a){
		atomy.push_back(a);
	}
	void skelet::pridej_vazbu(const std::string &Z, const std::string &K, unsigned char cetnost){
		pridej_vazbu(najdi_atom_idx(Z),najdi_atom_idx(K),cetnost);
	}
/**/void skelet::pridej_vazbu(size_t idxZ, size_t idxK, unsigned char cetnost){
		if(idxZ>=atomy.size()){
			error(ERR_NO_ATOM_INDEX, 0, idxZ);
		}
		if(idxK>=atomy.size()){
			error(ERR_NO_ATOM_INDEX, 0, idxK);
		}
		if(idxZ>=atomy.size() || idxK>=atomy.size()){
			return;
		}
		//pridej hranu:
		vazba v;
		v.cetnost=cetnost;
		v.cil=idxK;
		atomy[idxZ].vazby.push_back(v);

		if(idxK!=idxZ){
			v.cil=idxZ;
			atomy[idxK].vazby.push_back(v);
		}
	}
	void skelet::zvys_vazbu(const std::string &Z, const std::string &K, unsigned char cetnost){
		zvys_vazbu(najdi_atom_idx(Z), najdi_atom_idx(K), cetnost);
	}
	void skelet::zvys_vazbu(size_t idxZ, size_t idxK, unsigned char cetnost){
		//pridej hranu:
		if(!je_vazba(idxZ, idxK)){	//pokud tam neni, tak ji pridej
			if(idxK>=atomy.size() || idxZ>=atomy.size()){
				return;
			}
			vazba v;
			v.cetnost=cetnost;
			v.cil=idxK;
				atomy[idxZ].vazby.push_back(v);
			v.cil=idxZ;
				atomy[idxK].vazby.push_back(v);
		}else{	//pokud tam uz je, tak jen zvys aritu o cetnost
			for(std::vector<vazba>::iterator it=atomy[idxZ].vazby.begin(); it!=atomy[idxZ].vazby.end(); ++it){
				if(it->cil==idxK){
					it->cetnost=it->cetnost+cetnost;
				}
			}
			if(idxZ!=idxK){
				for(std::vector<vazba>::iterator it=atomy[idxK].vazby.begin(); it!=atomy[idxK].vazby.end(); ++it){
					if(it->cil==idxZ){
						it->cetnost=it->cetnost+cetnost;
					}
				}
			}
		}
	}
	void skelet::zrus_vazbu(const std::string &Z, const std::string &K){
		zrus_vazbu(najdi_atom_idx(Z),najdi_atom_idx(K));
	}
	void skelet::zrus_vazbu(size_t idxZ, size_t idxK){
		if(idxZ>=atomy.size())
			error(ERR_NO_ATOM_INDEX,0,idxZ);
		if(idxK>=atomy.size())
			error(ERR_NO_ATOM_INDEX,0,idxK);

		if(idxZ>=atomy.size() || idxK>=atomy.size()){
			return;
		}

		for(std::vector<vazba>::iterator it=atomy[idxZ].vazby.begin(); it!=atomy[idxZ].vazby.end(); ++it){
			if(it->cil == idxK){
				atomy[idxZ].vazby.erase(it);
				break;
			}
		}
		if(idxZ!=idxK){
			for(std::vector<vazba>::iterator it=atomy[idxK].vazby.begin(); it!=atomy[idxK].vazby.end(); ++it){
				if(it->cil == idxZ){
					atomy[idxK].vazby.erase(it);
					break;
				}
			}
		}
	}

	void skelet::sniz_vazbu(const std::string &Z, const std::string &K, unsigned char cetnost=1){
		size_t idxZ=najdi_atom_idx(Z);
		size_t idxK=najdi_atom_idx(K);

		for(std::vector<vazba>::iterator it=atomy[idxZ].vazby.begin(); it!=atomy[idxZ].vazby.end(); ++it){
			if(it->cil == idxK){
				if(it->cetnost<=cetnost){
					if(it->cetnost != cetnost)
						error(ERR_NOEXIST_BIND,0);
					atomy[idxZ].vazby.erase(it);
				}else
					it->cetnost=it->cetnost-cetnost;
				break;
			}
		}
		if(idxZ!=idxK){
			for(std::vector<vazba>::iterator it=atomy[idxK].vazby.begin(); it!=atomy[idxK].vazby.end(); ++it){
				if(it->cil == idxZ){
					if(it->cetnost<=cetnost){
						if(it->cetnost != cetnost)
							error(ERR_NOEXIST_BIND,0);
						atomy[idxK].vazby.erase(it);
					}else
						it->cetnost=it->cetnost-cetnost;
					break;
				}
			}
		}
	}
	void skelet::presun_vazbu(const std::string &Z1, const std::string &K1, const std::string &Z2, const std::string &K2){
		size_t idxZ1=najdi_atom_idx(Z1);
		size_t idxK1=najdi_atom_idx(K1);

		//zrus starou:
		unsigned char arita=0;
		for(std::vector<vazba>::iterator it=atomy[idxZ1].vazby.begin(); it!=atomy[idxZ1].vazby.end(); ++it){
			if(it->cil == idxK1){
				arita=it->cetnost;
				atomy[idxZ1].vazby.erase(it);
				break;
			}
		}
		if(idxZ1!=idxK1){
			for(std::vector<vazba>::iterator it=atomy[idxK1].vazby.begin(); it!=atomy[idxK1].vazby.end(); ++it){
				if(it->cil == idxZ1){
					atomy[idxK1].vazby.erase(it);
					break;
				}
			}
		}

		//pridej novou:
		pridej_vazbu(Z2,K2,arita);
	}
	size_t skelet::pocet_atomu()const{
		return atomy.size()-1;
	}
/**/void skelet::pripoj(const skelet &co, bool zneplatni=true){
		atom	A;
		vazba	V;
		size_t	Len=pocet_atomu();

		if(Len==1 && co.pocet_atomu()==1 && atomy[1].jadro==prvky.end() && co.atomy[1].jadro==prvky.end() ){	//jedna se o torza
			//secti naboje:
			atomy[1].naboj=atomy[1].naboj+co.atomy[1].naboj;
			//sluc seznamy vazeb:

			for(std::vector<vazba>::const_iterator itV=co.atomy[1].vazby.begin(); itV!=co.atomy[1].vazby.end(); ++itV){
				pridej_vazbu(1,itV->cil,itV->cetnost);
			}
			return;
		}

		//sluc pocatky:
		for(std::vector<vazba>::const_iterator itV=co.atomy[0].vazby.begin(); itV!=co.atomy[0].vazby.end(); ++itV){
			V=*itV;
			if(V.cil!=0)
				V.cil+=Len;
			atomy[0].vazby.push_back(V);
		}

		//sluc zbytek:
		for(std::vector<atom>::const_iterator itA=co.atomy.begin()+1; itA!=co.atomy.end(); ++itA){
			A = *itA;
			//zneplatni jmena:
			if(zneplatni){
				for(std::vector<std::string>::iterator itN=A.jmena.begin(); itN!=A.jmena.end(); ++itN){
					itN->insert(0,"!");	//prefixuj vykricnikem
				}
			}
			//nastav vazby:
			A.posun_vazby(Len);
			pridej_atom(A);
		}
	}
/**/void skelet::volne_vazby(const std::string &kam,size_t za=0){
		size_t idxkam = najdi_atom_idx(kam);
		if(idxkam==std::string::npos){
			error(ERR_NO_ATOM_NAME, 0, kam.c_str());
			return;
		}

		size_t V=0;
		while(V<atomy[0].vazby.size() && atomy[0].vazby[V].cil<=za){
			++V;
		}

		if(V==atomy[0].vazby.size()){
			error(ERR_NO_FREE_BIND,0);
			return;
		}
		while(V<atomy[0].vazby.size()){	//pripoj vsechno
			size_t odkud=atomy[0].vazby[V].cil;
			unsigned char arita;
//			while(V<atomy[0].vazby.size() && atomy[0].vazby[V].cil==odkud){	//pripoj vsechno z prvniho atomu
				arita=atomy[0].vazby[V].cetnost;
				zrus_vazbu(0,odkud);	//odpovida ++V
				pridej_vazbu(idxkam,odkud,arita);
//			}
		}
	}
	void skelet::zamen_atom(const skelet &cim, const std::string &co){
		zamen_atom(cim,najdi_atom_idx(co));
	}
	void skelet::zamen_atom(const skelet &cim, size_t cil_idx){
		if(cim.atomy.size()!=2){
			error(ERR_NO_TORZO,0);
			return;
		}
		atom a=cim.atomy[1];
		if(cil_idx>=atomy.size()){
			error(ERR_NO_ATOM_INDEX, 0, cil_idx);
			return;
		}

		atomy[cil_idx].jadro=a.jadro;
		//atomy[cil_idx].naboj=a.naboj;
	}
	void skelet::smaz_atom(const std::string &co){
		size_t idx=najdi_atom_idx(co);
		if(idx==std::string::npos){
			error(ERR_NO_ATOM_NAME, 0, co.c_str());
			return;
		}

		//pores vazby:
		switch(atomy[idx].vazby.size()){
		case 1:
			zrus_vazbu(idx,atomy[idx].vazby[0].cil);
			break;
		case 2:{
			if(atomy[idx].vazby[0].cetnost!=atomy[idx].vazby[1].cetnost){
				error(ERR_DIF_BIND,0);
				return;
			}
			size_t cil1=atomy[idx].vazby[0].cil;
			size_t cil2=atomy[idx].vazby[1].cil;
			unsigned char cetnost=atomy[idx].vazby[0].cetnost;

			zrus_vazbu(idx, cil1);
			zrus_vazbu(idx, cil2);
			pridej_vazbu(cil1,cil2,cetnost);
			break;
			}
		default:
			error(ERR_NO_BRIDGE,0);
			return;
		}

		//smaz atom:
		atomy.erase(atomy.begin()+idx);

		//oprav vazby:
		for(std::vector<atom>::iterator itA=atomy.begin(); itA!=atomy.end(); ++itA){
			for(std::vector<vazba>::iterator itV=itA->vazby.begin(); itV!=itA->vazby.end(); ++itV){
				if(itV->cil>idx)
					--(itV->cil);
			}
		}
	}
/**/void skelet::smaz_atom_bez_vazeb(const std::string &co){
		size_t idx=najdi_atom_idx(co);
		if(idx==std::string::npos){
			error(ERR_NO_ATOM_NAME, 0, co.c_str());
			return;
		}

		//pores vazby:
		while(atomy[idx].vazby.size()>0){
			unsigned char cetnost = atomy[idx].vazby[0].cetnost;
			size_t        cil     = atomy[idx].vazby[0].cil;
			zrus_vazbu(idx,cil);
			pridej_vazbu(0,cil,cetnost);
		}

		//smaz atom:
		atomy.erase(atomy.begin()+idx);

		//oprav vazby:
		for(std::vector<atom>::iterator itA=atomy.begin(); itA!=atomy.end(); ++itA){
			for(std::vector<vazba>::iterator itV=itA->vazby.begin(); itV!=itA->vazby.end(); ++itV){
				if(itV->cil>idx)
					--(itV->cil);
			}
		}
	}
/**/void skelet::nastav_naboj(size_t idxkde, char kolik){
		if(idxkde<atomy.size()){
			return;
		}

		atomy[idxkde].naboj=atomy[idxkde].naboj+kolik;
	}
/**/unsigned char skelet::najdi_cetnost(size_t idxZ,size_t idxK)const{
		if(idxZ>atomy.size()){
			error(ERR_NO_ATOM_INDEX,0,idxZ);
			return 0;
		}
		for(std::vector<vazba>::const_iterator it=atomy[idxZ].vazby.begin(); it!=atomy[idxZ].vazby.end(); ++it){
			if(it->cil==idxK){
				return it->cetnost;
			}
		}

		return 0;		//neni vazba
	}
/**/void skelet::aplikuj(size_t idxkde, const skelet &co){
		if(idxkde==std::string::npos){
			error(ERR_NO_ATOM_INDEX,0,idxkde);
			return;
		}

		if(co.atomy.size()!=2 || co.atomy[1].jadro!=prvky.end()){
			error(ERR_NO_TORZO,0);
			return;
		}

		atomy[idxkde].naboj=atomy[idxkde].naboj+co.atomy[1].naboj;
		for(std::vector<vazba>::const_iterator it=co.atomy[1].vazby.begin(); it!=co.atomy[1].vazby.end(); ++it){
			if(it->cil==1){
				zvys_vazbu(idxkde,idxkde+1,it->cetnost-najdi_cetnost(idxkde,idxkde+1));
			}else{
				pridej_vazbu(idxkde,0,it->cetnost);
			}
		}
	}
/**/void skelet::aplikuj(const std::string &kde, const skelet &co){
		aplikuj(najdi_atom_idx(kde),co);
	}
/**/void skelet::resetuj_jmena(){
		size_t jmeno=0;
		for(std::vector<atom>::iterator itA=atomy.begin(); itA!=atomy.end(); ++itA, ++jmeno){
			itA->jmena.clear();
			if(jmeno!=0)
				itA->jmena.push_back(int2str(jmeno));
			else
				itA->jmena.push_back(".");
		}
	}
/**/void skelet::pricarkuj(){
		for(std::vector<atom>::iterator itA=atomy.begin(); itA!=atomy.end(); ++itA){
			for(std::vector<std::string>::iterator itN=itA->jmena.begin(); itN!=itA->jmena.end(); ++itN){
				*itN+='\'';
			}
		}
	}
/**/void skelet::finalizuj(){
		size_t Len=pocet_atomu();
		for(size_t idxA=1; idxA<atomy.size(); ++idxA){
			if(atomy[idxA].jadro == prvky.end()){
				error(ERR_UNKNOWN_ATOM, 0);
				continue;
			}

			size_t soucet=abs(atomy[idxA].naboj);
			for(std::vector<vazba>::const_iterator itV=atomy[idxA].vazby.begin(); itV!= atomy[idxA].vazby.end(); ++itV){
				soucet+=itV->cetnost;
			}
			if(soucet>atomy[idxA].jadro->arita){
				error(ERR_TOO_MANY_BIND, 0, atomy[idxA].jadro->arita, soucet, atomy[idxA].jadro->Zn.c_str());
			}else{
				while(soucet<atomy[idxA].jadro->arita){
					pridej_atom("H","");
					++Len;
					++soucet;
					pridej_vazbu(idxA,Len,1);
				}
			}
		}
	}
	//--------------------------------------------------------------------------------------
/**/std::string urizni_param(std::string &s, std::string sep=","){	//oddeli cast pred sep a cast za nim
		size_t poz=s.find(sep);
		std::string txt=s.substr(0,poz);
		if(poz==std::string::npos)
			s="";
		else
			s.erase(0,poz+sep.length());
		return txt;
	}
/**/size_t rozsekej_param(const std::string &txt, std::string &item){
		if(txt.length()<2 || txt[0]!='$'){
			error(ERR_INCOR_ARG, 0, txt.c_str());
			return std::string::npos;
		}

		size_t i=1;
		while(i<txt.length() && '0'<=txt[i] && txt[i]<='9'){ //precti cislo parametru
			++i;
		}
		size_t idx=str2int(txt.substr(1,i-1));

		if(txt.length()<i){	//cely uzel
			item="";
			return idx;
		}

		item=txt.substr(i);
		return idx;
	}
	//--------------------------------------------------------------------------------------
	namespace INT{
/**/	size_t dej(std::string &s, const std::vector<ntika> &DATA){	//kladne cele cislo
			if(s.length()<2){	//neni to nic!
				error(ERR_NUM_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return std::string::npos;
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='#'){	//je to (cele) ciselna konstanta?
				size_t i=1;
				while(i<txt.length()){
					if('0'>txt[i] || txt[i]>'9'){	//ne!
						error(ERR_NUM_ARG_EXPECT, 0, txt.c_str());
						return std::string::npos;
					}
					++i;
				}
				txt.erase(0,1);
				return str2int(txt);
			}

			if(txt[0]=='$'){	//je to (cele) ciselny parametr?
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || !(polozka==".int" || polozka=="")){	//chybna polozka!
					error(ERR_NUM_ARG_EXPECT, 0, polozka.c_str());
					return std::string::npos;
				}
				if(idx>=DATA.size()){	//chybny index!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return std::string::npos;
				}
				return DATA[idx].c;
			}

			error(ERR_NUM_ARG_EXPECT, 0, txt.c_str());
			return std::string::npos;
		}
		size_t dej_ref(std::string &s, const std::vector<ntika> &DATA){	//kladne cele cislo - pro zapis
			if(s.length()<2){	//neni to nic!
				error(ERR_NUM_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return std::string::npos;
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='$'){	//je to (cele) ciselny parametr?
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || !(polozka==".int" || polozka=="")){	//chybna polozka!
					error(ERR_NUM_ARG_EXPECT, 0, txt.c_str());
					return std::string::npos;
				}
				if(idx>=DATA.size()){	//chybny index!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return std::string::npos;
				}
				return idx;
			}

			error(ERR_NUM_ARG_EXPECT, 0, txt.c_str());
			return 0;
		}
/**/	void NEW(std::string params,std::vector<ntika> &DATA){
			DATA[0].c=dej(params,DATA);
		}
		void ADD(std::string params,std::vector<ntika> &DATA){
			size_t cil=dej_ref(params,DATA);
			if(cil==std::string::npos)
				return;
			DATA[cil].c+=dej(params,DATA);
		}
		void MUL(std::string params, std::vector<ntika> &DATA){
			size_t cil=dej_ref(params,DATA);
			if(cil==std::string::npos)
				return;
			DATA[cil].c*=dej(params,DATA);
		}
	}

	//--------------------------------------------------------------------------------------
	namespace STR{
/**/	std::string dej(std::string &s, const std::vector<ntika> &DATA){
			if(s.length()<2){	//neni to nic!
				error(ERR_STR_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return "";
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='~' && txt[txt.length()-1]=='~'){	//je to (cele) retezcova konstanta?
				return txt.substr(1,txt.length()-2);
			}

			if(txt[0]=='$'){	//je to (cele) retezcovy parametr?
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || idx>=DATA.size()){	//chybna polozka!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return "";
				}

				if(polozka==".str" || polozka=="")
					return DATA[idx].s;
				if(polozka==".zdo.z")
					return DATA[idx].z.back();
				if(polozka==".zdo.k")
					return DATA[idx].k.back();
			}

			error(ERR_STR_ARG_EXPECT, 0, txt.c_str());
			return "";
		}
		size_t dej_idx(std::string &s, const std::vector<ntika> &DATA){
			if(s.length()<2){	//neni to nic!
				error(ERR_STR_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return std::string::npos;
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='$'){	//je to (cele) retezcovy parametr?
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || idx>=DATA.size()){	//chybna polozka!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return std::string::npos;
				}

				if(polozka==".str" || polozka=="")
					return idx;
/** /			if(polozka==".zdo.z")
					return &(std::string)(DATA[idx].z.back());
				if(polozka==".zdo.k")
					return &(std::string)(DATA[idx].k.back());
/ **/
			}

			error(ERR_STR_ARG_EXPECT, 0, txt.c_str());
			return std::string::npos;
		}

/**/	void NEW(std::string params, std::vector<ntika> &DATA){
			DATA[0].s=dej(params,DATA);
			if(params!=""){
				error(ERR_TOO_MANY_ARGS, 0);
				return;
			}
		}
		void APP(std::string params, std::vector<ntika> &DATA){
			size_t cil=dej_idx(params,DATA);
			if(cil==std::string::npos)
				return;
			DATA[cil].s+=dej(params,DATA);
			if(params!=""){
				error(ERR_TOO_MANY_ARGS, 0);
				return;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	namespace MOL{
/**/	skelet dej(std::string &s, const std::vector<ntika> &DATA){
			if(s.length()<2){	//neni to nic!
				error(ERR_MOL_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return skelet();
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='$'){	//je to odkaz na skelet
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || !(polozka==".mol" || polozka=="")){	//chybna polozka!
					error(ERR_MOL_ARG_EXPECT, 0, txt.c_str());
					return skelet();
				}
				if(idx>=DATA.size()){	//chybny index!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return skelet();
				}
				return DATA[idx].m;
			}

			error(ERR_MOL_ARG_EXPECT, 0, txt.c_str());
			return skelet();
		}
/**/	size_t dej_idx(std::string &s, const std::vector<ntika> &DATA){
			if(s.length()<2){	//neni to nic!
				error(ERR_MOL_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return std::string::npos;
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='$'){	//je to odkaz na skelet
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || !(polozka==".mol" || polozka=="")){	//chybna polozka!
					error(ERR_MOL_ARG_EXPECT, 0, txt.c_str());
					return std::string::npos;
				}
				if(idx>=DATA.size()){	//chybny index!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return std::string::npos;
				}
				return idx;
			}

			error(ERR_MOL_ARG_EXPECT, 0, txt.c_str());
			return std::string::npos;
		}
		size_t dej_zdo_idx(std::string &s, const std::vector<ntika> &DATA){
			if(s.length()<2){	//neni to nic!
				error(ERR_ZDO_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return std::string::npos;
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='$'){	//je to (cele) retezcovy parametr?
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || idx>=DATA.size()){	//chybna polozka!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return std::string::npos;
				}

				if(polozka==".zdo" || polozka=="")
					return idx;
			}

			error(ERR_ZDO_ARG_EXPECT, 0, txt.c_str());
			return std::string::npos;
		}
/**/	char dej_naboj(std::string &s, const std::vector<ntika> &){	//cele cislo
			if(s.length()<2){	//neni to nic!
				error(ERR_NUM_ARG_EXPECT, 0, s.c_str());
				s.erase();
				return 0;
			}

			std::string txt=urizni_param(s);	//urizni jen jeden parametr

			if(txt[0]=='#'){	//je to (cele) ciselna konstanta?
				size_t i=1;
				bool minus=false;
				if(txt[i]=='-'){
					minus=true;
					++i;
				}
				while(i<txt.length()){
					if(txt[i]<'0' || '9'<txt[i]){	//ne!
						error(ERR_NUM_ARG_EXPECT, 0, txt.c_str());
						return 0;
					}
					++i;
				}
				txt.erase(0,(minus?2:1));
				size_t val=str2int(txt);
				if(val>8){
					error(ERR_BIG_CHARGE, 0, (int)val);
					return 0;
				}
				return (minus?-(char)val:(char)val);
			}

			/*if(txt[0]=='$'){	//je to (cele) ciselny parametr?
				std::string polozka;
				size_t idx=rozsekej_param(txt,polozka);
				if(idx==std::string::npos || !(polozka==".int" || polozka=="")){	//chybna polozka!
					error(ERR_NUM_ARG_EXPECT, 0, txt);
					return 0;
				}
				if(idx>=DATA.size()){	//chybny index!
					error(ERR_NO_DATA_INDEX, 0, idx);
					return 0;
				}
				return DATA[idx].c;
			}
			*/

			error(ERR_NUM_ARG_EXPECT, 0, txt.c_str());
			return 0;
		}
		bool je_vazba(char c){
			return c=='-' || c=='=' || c=='%';
		}
		char cetnost_vazby(char c){
			switch(c){
				case '-':
					return 1;
				case '=':
					return 2;
				case '%':
					return 3;
				default:
					std::string s="";
					s+=c;
					error(ERR_UNKNOWN_BIND, 0, s.c_str());
					return 0;
			}
		}
		void ZDO(std::string params, std::vector<ntika> &DATA){
			size_t cil=dej_zdo_idx(params, DATA);
			if(cil==std::string::npos){
				return;
			}
			DATA[cil].z.push_back(STR::dej(params,DATA));
			if(params=="")
				DATA[cil].k.push_back("");
			else
				DATA[cil].k.push_back(STR::dej(params,DATA));
		}
		void LAP(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_zdo_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			size_t src=dej_zdo_idx(params, DATA);
			if(src==std::string::npos){
				return;
			}
			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}

			std::vector<std::string>::iterator itZ, itK;
			for(itZ=DATA[src].z.begin(),itK=DATA[src].k.begin();
				itZ!=DATA[src].z.end() && itK!=DATA[src].k.end();
				++itZ, ++itK){
					DATA[dst].z.push_back(*itZ);
					DATA[dst].k.push_back(*itK);
			}
			//if(itZ!=DATA[src].z.end() || itK!=DATA[src].k.end()){
				//error(nekde nastala vnitrni chyba, nestejne dlouhe seznamy zdo.z a zdo.k);
			//	return;
			//}
		}
/**/	void NWA(std::string params, std::vector<ntika> &DATA){
			std::string param1=STR::dej(params,DATA);
			if(param1==""){
				return;
			}
			DATA[0].m.pridej_atom(param1, "1");
		}
/**/	void NEW(std::string params, std::vector<ntika> &DATA){
			//rozdel na atomovou a vazbovou cast
			std::string jadra=urizni_param(params,":");
			std::string err_zal="MOL::NEW("+jadra+")";
			if(jadra.length()>0 && (jadra[0]!='[' || jadra[jadra.length()-1]!=']')){
				error(ERR_INCOR_COMMAND, 0, err_zal.c_str());
				return;
			}
			//pokud jsou v skeletu jadra, tak kazde zpracuj
			if(jadra.length()){
				//smaz zavorky [] u seznamu atomu
				jadra.erase(jadra.length()-1);
				jadra.erase(0,1);
				//projdi jadra
				DATA[0].m=skelet();
				while(jadra.length()){
					std::string jadro=urizni_param(jadra,"-");
					if(jadro.length()==0 || jadro[0]!='(' || jadro[jadro.length()-1]!=')'){
						error(ERR_INCOR_COMMAND, 0, err_zal.c_str());
						return;
					}
					//smaz zavorky () u atomu
					jadro.erase(jadro.length()-1);
					jadro.erase(0,1);
					//sestav jadro
					atom a;
					std::string znacka=STR::dej(jadro,DATA);
					size_t arita=INT::dej(jadro,DATA);
					a.naboj=0; //=dej_naboj(jadro,DATA);
					a.jadro=najdi_atom(znacka,arita);
					a.jmena.clear();
					if(jadro.length()==0 || jadro[0]!='{' || jadro[jadro.length()-1]!='}'){
						error(ERR_INCOR_COMMAND, 0, err_zal.c_str());
						return;
					}
					//smaz zavorky {} u seznamu jmen
					jadro.erase(jadro.length()-1);
					jadro.erase(0,1);
					while(jadro.length()){
						a.jmena.push_back(STR::dej(jadro,DATA));
					}
					DATA[0].m.pridej_atom(a);
				}
			}

			if(params.length()>0 && (params[0]!='[' || params[params.length()-1]!=']')){
				error(ERR_INCOR_COMMAND, 0, err_zal.c_str());
				return;
			}

			//pokud jsou v skeletu vazby, tak je zpracuj
			if(params.length()){
				params.erase(params.length()-1);
				params.erase(0,1);
				while(params.length()){
					std::string param1=urizni_param(params,",");
					//rozeber na (size_t char size_t) .. z kolik kam
					size_t i=0;
					while(i<param1.length() && !je_vazba(param1[i]))
						++i;
					if(i>=param1.length()){
						error(ERR_INCOR_COMMAND, 0, err_zal.c_str());
						return;
					}
					//pridej hranu do skeletu:
					DATA[0].m.pridej_vazbu(str2int(param1.substr(0,i)),str2int(param1.substr(i+1)),cetnost_vazby(param1[i]));
				}
			}
		}
		void CPY(std::string params, std::vector<ntika> &DATA){
			DATA[0].m=dej(params,DATA);
			if(params.length()){
				error(ERR_TOO_MANY_ARGS, 0);
				return;
			}
		}
		void ION(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			char naboj=dej_naboj(params, DATA);
			if(naboj==0){
				return;
			}

			DATA[dst].m.nastav_naboj(0, naboj);

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
		void APP(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params,DATA);

			size_t pocet=INT::dej(params, DATA);
			size_t pozice=dej_zdo_idx(params, DATA);
			if(pozice==std::string::npos){
				return;
			}

			if(pocet!=DATA[pozice].z.size()){
				error(ERR_LONG_ARRAY,0);
				return;
			}

			for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny substituce dle seznamu
				size_t Len=DATA[dst].m.pocet_atomu();
				DATA[dst].m.pripoj(src);
				//pridej vazbu:
				size_t idxkam=DATA[dst].m.najdi_atom_idx(DATA[pozice].z[idx]);
				if(idxkam==std::string::npos){
					continue;
				}
				DATA[dst].m.pridej_vazbu(idxkam,Len+1,1);
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/*-*/	void CON(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params,DATA);

			size_t pocet=INT::dej(params, DATA);
			size_t pozice;
			if(params==""){
				for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny substituce na ~1~
					size_t Len=DATA[dst].m.pocet_atomu();
					DATA[dst].m.pripoj(src);
					//pridej vazby:
					DATA[dst].m.volne_vazby("1",Len);
				}
			}else{
				pozice=dej_zdo_idx(params, DATA);
				if(pozice==std::string::npos){
					return;
				}
				if(pocet!=DATA[pozice].z.size()){
					error(ERR_LONG_ARRAY,0);
					return;
				}
				for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny substituce dle seznamu
					size_t Len=DATA[dst].m.pocet_atomu();
					DATA[dst].m.pripoj(src);
					//pridej vazby:
					DATA[dst].m.volne_vazby(DATA[pozice].z[idx],Len);
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/*-*/	void COI(std::string params, std::vector<ntika> &DATA){ //TODO: opravit aby se opravdu pripojovala jen 1. vazba!!!
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params,DATA);

			size_t pocet=INT::dej(params, DATA);
			size_t pozice;
			if(params==""){
				for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny substituce na ~1~
					size_t Len=DATA[dst].m.pocet_atomu();
					DATA[dst].m.pripoj(src);
					//pridej vazby:
					DATA[dst].m.volne_vazby("1",Len);
				}
			}else{
				pozice=dej_zdo_idx(params, DATA);
				if(pozice==std::string::npos){
					return;
				}
				if(pocet!=DATA[pozice].z.size()){
					error(ERR_LONG_ARRAY,0);
					return;
				}
				for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny substituce dle seznamu
					size_t Len=DATA[dst].m.pocet_atomu();
					DATA[dst].m.pripoj(src);
					//pridej vazby:
					DATA[dst].m.volne_vazby(DATA[pozice].z[idx],Len);
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void LIN(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params, DATA);
			size_t pocet=INT::dej(params, DATA);

			DATA[dst].m=src;
			for(size_t idx=1; idx<pocet; ++idx){	//pocet-krat to zretez
				size_t Len=DATA[dst].m.pocet_atomu();
				DATA[dst].m.pripoj(src);
				//pridej vazbu:
				DATA[dst].m.pridej_vazbu(Len,Len+1,1);
			}

			DATA[dst].m.resetuj_jmena();

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void ADD(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params, DATA);
			size_t pocet=INT::dej(params, DATA);

			if(pocet==std::string::npos){
				return;
			}

			for(size_t idx=1; idx<=pocet; ++idx){	//pocet-krat to sluc
				DATA[dst].m.pripoj(src);
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void CHN(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params, DATA);
			size_t pocet=INT::dej(params, DATA);

			if(pocet==std::string::npos){
				return;
			}

			for(size_t idx=1; idx<=pocet; ++idx){	//pocet-krat to zretez
				DATA[dst].m.pripoj(src,false);
				src.pricarkuj();
			}

			//pridej vazby:
			if(params!=""){ //spoje definovany:
				size_t pozice = dej_zdo_idx(params, DATA);
				if(pozice==std::string::npos)
					return;

				for(std::vector<std::string>::const_iterator itZ=DATA[pozice].z.begin(),itK=DATA[pozice].k.begin(); itZ!=DATA[pozice].z.end(); ++itZ, ++itK){
					DATA[dst].m.pridej_vazbu(*itZ, *itK,1);
				}
			}else{			//spoje nedef.:
				std::string s("1");
				for(size_t i=1; i<pocet; ++i){
					DATA[dst].m.pridej_vazbu(s,s+'\'',1);
					s+='\'';
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
		void MVV(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			std::string Z1=STR::dej(params, DATA);
			std::string K1=STR::dej(params, DATA);
			std::string Z2=STR::dej(params, DATA);
			std::string K2=STR::dej(params, DATA);

			DATA[dst].m.presun_vazbu(Z1,K1,Z2,K2);

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void SUB(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params,DATA);

			size_t pocet=INT::dej(params, DATA);
			if(params!=""){
				size_t pozice=dej_zdo_idx(params, DATA);
				if(pozice==std::string::npos){
					return;
				}

				if(pocet!=DATA[pozice].z.size()){
					error(ERR_LONG_ARRAY,0);
					return;
				}

				for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny substituce dle seznamu
					DATA[dst].m.zamen_atom(src, DATA[pozice].z[idx]);
				}
			}else{
				if(pocet!=DATA[dst].m.pocet_atomu()){
					error(ERR_CMPX_SUB, 0, pocet, DATA[dst].m.pocet_atomu());
					return;
				}

				for(size_t idx=1; idx<=pocet; ++idx){	//proved vsechny substituce
					DATA[dst].m.zamen_atom(src, idx);
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
		void DLA(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			size_t pocet=INT::dej(params, DATA);
			size_t pozice=dej_zdo_idx(params, DATA);
			if(pozice==std::string::npos){
				return;
			}

			if(pocet!=DATA[pozice].z.size()){
				error(ERR_LONG_ARRAY,0);
				return;
			}

			for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny delece dle seznamu
				DATA[dst].m.smaz_atom(DATA[pozice].z[idx]);
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void DLX(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			std::string jmeno=STR::dej(params, DATA);
			if(jmeno.length()==0){
				return;
			}

			DATA[dst].m.smaz_atom_bez_vazeb(jmeno);

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
		void DLV(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			size_t pocet=INT::dej(params, DATA);
			size_t pozice=dej_zdo_idx(params, DATA);
			if(pozice==std::string::npos){
				return;
			}

			if(pocet!=DATA[pozice].z.size()){
				error(ERR_LONG_ARRAY,0);
				return;
			}

			for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny delece dle seznamu
				DATA[dst].m.zrus_vazbu(DATA[pozice].z[idx], DATA[pozice].k[idx]);
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void DEC(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			size_t pocet=INT::dej(params, DATA);
			size_t pozice=dej_zdo_idx(params, DATA);
			if(pozice==std::string::npos){
				return;
			}

			bool v_jednom=false;
			if(pocet==DATA[pozice].z.size()*2 && pocet==DATA[pozice].k.size()*2){
				v_jednom=false;
			}else if(pocet==DATA[pozice].z.size() && prazdny_seznam(DATA[pozice].k)){
				v_jednom=true;
			}else{
				error(ERR_LONG_ARRAY,0);
				return;
			}

			if(v_jednom){	//proved vsechny delece dle seznamu
				for(size_t idx=0; idx+1<pocet; idx+=2){
					DATA[dst].m.sniz_vazbu(DATA[pozice].z[idx],DATA[pozice].z[idx+1],1);
				}
			}else{
				for(size_t idx=0; idx<pocet/2; ++idx){
					DATA[dst].m.sniz_vazbu(DATA[pozice].z[idx], DATA[pozice].k[idx],1);
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
		void INC(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			size_t pocet=INT::dej(params, DATA);
			if(pocet==0){	//operace cyklo bez parametru = prvni s posledim
				DATA[dst].m.zvys_vazbu(1,DATA[dst].m.posl_atom_idx(),1);
				if(params!=""){
					error(ERR_TOO_MANY_ARGS,0);
				}
				return;
			}

			size_t pozice=dej_zdo_idx(params, DATA);
			if(pozice==std::string::npos){
				return;
			}

			bool v_jednom=false;
			if(pocet!=DATA[pozice].z.size()){
				if(pocet==DATA[pozice].z.size()*2 && prazdny_seznam(DATA[pozice].k)){
					v_jednom=true;
				}else{
					error(ERR_LONG_ARRAY,0);
					return;
				}
			}

			if(v_jednom){	//proved vsechny inserce dle seznamu
				for(size_t idx=0; idx+1<pocet; idx+=2){
					DATA[dst].m.zvys_vazbu(DATA[pozice].z[idx],DATA[pozice].z[idx+1],1);
				}
			}else{
				for(size_t idx=0; idx<pocet; ++idx){
					DATA[dst].m.zvys_vazbu(DATA[pozice].z[idx], DATA[pozice].k[idx],1);
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void APL(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			skelet src=dej(params, DATA);

			size_t pocet=INT::dej(params, DATA);

			if(params==""){
				size_t Len=DATA[dst].m.pocet_atomu();
				if(Len==pocet || Len-1==pocet){
					for(size_t idx=1;idx<=pocet;++idx){
						DATA[dst].m.aplikuj(idx,src);
					}
				}else if(pocet==1){
					DATA[dst].m.aplikuj(1,src);
				}else{
					error(ERR_NOEXIST_BIND,0);
					return;
				}
			}else{
				size_t pozice=dej_zdo_idx(params, DATA);
				if(pozice==std::string::npos){
					return;
				}

				if(pocet!=DATA[pozice].z.size()){
					error(ERR_LONG_ARRAY,0);
					return;
				}

				for(size_t idx=0; idx<pocet; ++idx){	//proved vsechny aplikace dle seznamu
					DATA[dst].m.aplikuj(DATA[pozice].z[idx],src);
				}
			}

			if(params!=""){
				error(ERR_TOO_MANY_ARGS,0);
				return;
			}
		}
/**/	void FIN(std::string params, std::vector<ntika> &DATA){
			size_t dst=dej_idx(params, DATA);
			if(dst==std::string::npos){
				return;
			}
			DATA[dst].m.finalizuj();
		}
	}
}
