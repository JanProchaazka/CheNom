#include "h/model.hpp"

namespace model{
	bod::bod(double a=0, double b=0, double c=0){
		x=a;
		y=b;
		z=c;
	}

	bod bod::operator+(const bod &b)const{
		bod bb;
		bb.x=x+b.x;
		bb.y=y+b.y;
		bb.z=z+b.z;
		return bb;
	}

	bod bod::operator/(const double b)const{
		bod bb;
		bb.x=x/b;
		bb.y=y/b;
		bb.z=z/b;
		return bb;
	}
	bod bod::operator*(const double b)const{
		bod bb;
		bb.x=x*b;
		bb.y=y*b;
		bb.z=z*b;
		return bb;
	}
	bod bod::operator-(const bod &b)const{
		bod bb;
		bb.x=x-b.x;
		bb.y=y-b.y;
		bb.z=z-b.z;
		return bb;
	}
	double norma(const bod &X){
		return sqrt(X.x*X.x+X.y*X.y+X.z*X.z);
	}

	atom::atom(semantika::atom a){
		jadro=a.jadro;
		jmena=a.jmena;
		vazby=a.vazby;
		naboj=a.naboj;
		poloha.x=poloha.y=poloha.z=0;
		smer.x=smer.y=smer.z=0;
		F.x=F.y=F.z=0;
	}
	/////////////////////////////////////////
	void zapis(std::string s){
		std::ofstream F;
		F.open("molekula.wrl");
		F<<s;
		F.close();
	}
	/////////////////////////////////////////
	skelet::skelet(semantika::skelet mol){
		for(size_t idx=0; idx<=mol.pocet_atomu(); ++idx){
			atomy.push_back(mol.dej_atom(idx));
		}
	}

	bod spocti_Fe(const atom &A, const atom &B){
		bod Fe;
		bod C=B.poloha-A.poloha;
		double c=	/*(e0*e0)/(4*PI*eps)*/	1//1.439964E-28
												*
//					/* Q1*Q2 */		(A.jadro->Z*B.jadro->Z);
									(3*3);	//mensi chyby, kdyz jsou vsechny naboje povazovany za stejne
		double velF = c/(norma(C)*norma(C));
		Fe = C*(-velF/norma(C));

		return Fe;
	}

	bod spocti_Fp(const atom &A, const atom &B, unsigned char cetnost){
		bod Fp;
		bod C=B.poloha-A.poloha;
		double K=/* tuhost gumy */ 1;//1.E-20;
		double baseLen=4.0;	//4.=délka gumy
		double Len;
#ifdef MODEL_BIND_LEN
		switch(cetnost){
			case 1:
				Len=baseLen*1.0;
				break;
			case 2:
			case 3:
				Len=baseLen*sqrt((double)cetnost)/2.0;
				break;
			default:
				Len=0;
				break;
		}
//		K=(double)cetnost;
#endif
		double koef=1.-(Len/norma(C)); 

		//Fp.x = K*C.x*koef;
		//Fp.y = K*C.y*koef;
		//Fp.z = K*C.z*koef;
		Fp = C*K*koef;
		return Fp;
	}

	void skelet::optimalize(){
		int d=1;
		//priprav (neni dobre zacinat ze se protinaj:
#ifdef SPHERE_MODEL
		double s=3.6/sqrt((double)atomy.size());
		double L=0;
		double dz=2.0/atomy.size();
		double z=1-dz/2.0;
		double R=2.5 /* kolikrat ma byt koule vetsi, nez je ta s minimalni pocatecni energii */
			     *(2.0*2.0) /* druha mocnina konecne delky vazby */
				 /4*sqrt(3.0)*atomy.size()/4/3.141592654
			   ;
#endif
		for(std::vector<atom>::iterator it=atomy.begin(); it!=atomy.end(); ++it, ++d){
#ifdef SPHERE_MODEL
			double r=sqrt(1-z*z);
			it->poloha.x=cos(L)*r*R;
			it->poloha.y=sin(L)*r*R;
			it->poloha.z=z*R;
			z-=dz;
			L+=s/r;
#else // puvodni inicializace (majici problemy s velkymi molekulami
			it->poloha.x=d;
			it->poloha.y=d*(-2*(d%2-.5));
#ifdef PLANAR_MODEL
			it->poloha.z=0; 								//planarne
#else
			it->poloha.z=d*d*(-2*(d%2-.5));					//prostorove
#endif
#endif
		}

		//hlavni cyklus:
		double t=		.01		/*ms*/;	//cas mezi
		double Fmez=	.01		/*N */;	//mez sily (nejvetsi sila v molekule mensi -> stop)
		double t_mez=	.0005	/*ms*/;	//mez casu (t< ->stop)

		double maxF=Fmez;
		while(t>=t_mez && maxF>=Fmez){
			//spocti:
			for(std::vector<atom>::iterator it=atomy.begin(); it!=atomy.end(); ++it){
				if(it->jadro==semantika::prvky.end()) continue;
				for(std::vector<atom>::iterator it2=atomy.begin(); it2!=atomy.end(); ++it2){
					if(it2->jadro==semantika::prvky.end()) continue;
					if(it==it2) continue;
					bod eF=spocti_Fe(*it, *it2);	// Fe - jadra maj naboj
					it->F=it->F+eF;
					it2->F=it2->F-eF;
				}
				for(std::vector<semantika::vazba>::iterator it2=it->vazby.begin(); it2!=it->vazby.end(); ++it2){
					if(it2->cil<(size_t)(it-atomy.begin())) continue;
					atom a=atomy[it2->cil];
					bod eF=spocti_Fp(*it, atomy[it2->cil], it2->cetnost);	// Fp - vazby jsou gumy
					it->F=it->F+eF;
					atomy[it2->cil].F=atomy[it2->cil].F-eF;
				}
			}
#ifdef DEBUG_PRINTS
			//zapis(grafika::tisk(*this));
			std::cout<<'.';
#endif
			//posun:
			maxF=0;
			for(std::vector<atom>::iterator it=atomy.begin(); it!=atomy.end(); ++it){
				if(it->jadro==semantika::prvky.end()) continue;
				if(maxF<norma(it->F)) maxF=norma(it->F);
				it->poloha=it->poloha+(it->smer*t+it->F*(5*t*t/it->jadro->m));
				it->smer=it->smer+(it->F/(it->jadro->m/t));
				it->F=bod()/* 0 */;
			}
			//uprava casovani:
			//t-=.000002;
			t*=0.9999;
			//if(t_mez * 1.998)
				//std::cout<<".";
		}
	}
	/////////////////////////////////////////
	void inicializace(){
		//...
		return;
	}

	void yymodel(semantika::skelet graf){
		skelet molekula(graf);
		molekula.optimalize();
		std::string s=grafika::tisk(molekula);
		std::cout<<s<<std::endl;
#ifdef DEBUG_PRINTS
		std::cout<<std::endl<<"----------------------------------------------------------------------------"<<std::endl;
#endif

		//zapis(s);

		return;
	}
}
