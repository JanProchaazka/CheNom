#include "h/grafika.hpp"

namespace grafika{
	std::string HLAVA_FILE;
	void inicializace(std::istream &is){
		HLAVA_FILE="";
		is>>std::noskipws;
		while(is.good()){
			char c;
			is>>c;
			HLAVA_FILE += c;
		}

		return;
	}
	std::string tisk(const semantika::Prvek &p){
		std::string TEXT="";
		double r, g, b;
		colorconvert(p.Col, r, g, b);
		TEXT+="PROTO Prvek_"+p.Zn+" [\n";
		TEXT+="\t\tfield MFString\tlokanty []\n";
		TEXT+="]{\n";
		TEXT+="\tAtom{\n";
		TEXT+="\t\tdCol "+dbl2str(r)+" "+dbl2str(g)+" "+dbl2str(b)+"\n";;
		TEXT+="\t\tfRad "+dbl2str(p.R)+"\n";
		TEXT+="\t\tsSig [\""+p.Zn+"\"]\n";
		TEXT+="\t\tsLoc IS lokanty\n";
		TEXT+="\t}\n";
		TEXT+="}\n";
		return TEXT;
	}
	std::string tisk(const model::atom &a){	//vypise VRML definici atomu
		std::string TEXT="";
		TEXT+="\t\tTransform{\n";
		TEXT+="\t\t\ttranslation "+dbl2str(a.poloha.x)+" "+dbl2str(a.poloha.y)+" "+dbl2str(a.poloha.z)+"\n";
		TEXT+="\t\t\tchildren [\n";
		TEXT+="\t\t\t\tPrvek_"+a.jadro->Zn+" {\n";
		TEXT+="\t\t\t\t\tlokanty [\n";
		for(std::vector<std::string>::const_iterator it=a.jmena.begin(); it!=a.jmena.end(); ++it){
			TEXT+="\t\t\t\t\t\t\""+ alphabeta2utf8(*it) + "\"\n";
		}
		TEXT+="\t\t\t\t\t]\n";
		TEXT+="\t\t\t\t}\n";
/** / //debug - sila
		bod N=(smer+F*(5/jadro->m))/10000; //pro t=1s
		double dist=norma(N);
		TEXT+="\t\t\t\tTransform{\n";
		TEXT+="\t\t\t\t\trotation "+
			dbl2str(N.z)+" "+
			"0. "+
			dbl2str(-N.x)+" "+
			dbl2str(acos(N.y/dist))+"\n";
		TEXT+="\t\t\t\t\tchildren [\n";
		TEXT+="\t\t\t\t\t\tSpoj{\n";
		TEXT+="\t\t\t\t\t\t\tdCol 0 1 0\n";	//zelene
		TEXT+="\t\t\t\t\t\t\tfLen "+dbl2str(dist)+"\n";	//velikost
		TEXT+="\t\t\t\t\t\t\tfPos 0 "+dbl2str(dist/2)+" 0\n";
		TEXT+="\t\t\t\t\t\t}\n";
		TEXT+="\t\t\t\t\t]\n";
		TEXT+="\t\t\t\t}\n";
/ **/
		TEXT+="\t\t\t]\n";
		TEXT+="\t\t}\n";

		return TEXT;
	}

	std::string vazba_tisk(model::bod Z, model::bod K, unsigned char arita){
		model::bod d=K-Z;
		double dist=model::norma(d);
		std::string TEXT="";
		TEXT+="\t\tTransform{\n";
		TEXT+="\t\t\ttranslation "+dbl2str(Z.x)+" "+dbl2str(Z.y)+" "+dbl2str(Z.z)+"\n";	//posun do Z
		TEXT+="\t\t\tchildren [\n";
		TEXT+="\t\t\t\tTransform{\n";
		TEXT+="\t\t\t\t\trotation "+	//rotace do K!!!
			dbl2str(d.z)+" "+				//osa
			"0. "+
			dbl2str(-d.x)+" "+
			dbl2str(acos(d.y/dist))+"\n";	//uhel
		TEXT+="\t\t\t\t\tchildren Vazba"+((1<=arita && arita<=3)?int2str(arita):"_err")+" {\n";	//arita
		TEXT+="\t\t\t\t\t\tfDist "+dbl2str(dist)+"\n";			//|ZK|
		TEXT+="\t\t\t\t\t\tfPoz  0 "+dbl2str(dist/2)+" 0\n";	//0 |ZK|/2 0
		TEXT+="\t\t\t\t\t}\n";
		TEXT+="\t\t\t\t}\n";
		TEXT+="\t\t\t]\n";
		TEXT+="\t\t}\n";

		return TEXT;
	}
	std::string tisk(const model::skelet &s){
		std::string HLAVA=HLAVA_FILE;
		std::string TELO="";
		std::string ATOMY="";
		std::string VAZBY="";

		//priprav telo:
		TELO+="Transform{\n";
		TELO+="\tchildren [\n";

		std::vector<std::vector<semantika::Prvek>::const_iterator> pouzite;
		for(std::vector<model::atom>::const_iterator idx=s.atomy.begin(); idx!=s.atomy.end(); ++idx){
			if(idx->jadro == semantika::prvky.end()){
				//neznamy prvek
				continue;
			}

			//PROTOs:
			bool pouzit=true;
			for(std::vector<std::vector<semantika::Prvek>::const_iterator>::const_iterator it=pouzite.begin(); it!=pouzite.end(); ++it){
				if(idx->jadro == *it){ //neni novy (jiz definovan)
					pouzit=false;
					break;
				}
			}
			if(pouzit){
				pouzite.push_back(idx->jadro);
				HLAVA+=tisk(*(idx->jadro));
				HLAVA+="\n";
			}
			//atomy:
			ATOMY+=tisk(*idx);
			//vazby:
			for(std::vector<semantika::vazba>::const_iterator it=idx->vazby.begin(); it!=idx->vazby.end(); ++it){
				if(s.atomy.begin()+it->cil>idx){	//kresli jen nenakreslene
					VAZBY+=vazba_tisk(idx->poloha, s.atomy[it->cil].poloha, it->cetnost);
				}
			}
		}
		HLAVA+="###############################################################################\n";	//konec
		//slep dohromady:
		TELO+=ATOMY+VAZBY;
		//opatri patickou:
		TELO+="\t]\n";
		TELO+="}\n";
		//vrat:
		return HLAVA+TELO;
	}

}