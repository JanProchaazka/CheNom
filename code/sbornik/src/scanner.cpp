#include "h/scanner.hpp"
namespace scanner{

//flagy uzlu lexikalniho stromu
//interval - meze + jmeno
class interval{
public:
	int			lo, hi;
	std::string name;
	interval(int lower=0, int upper=0, std::string label=std::string("")){
		lo=lower; hi=upper; name=label;
	}
	bool in(int val){
		return lo<=val && val<=hi;
	}
};
//mnozina intervalu s vyhledavanim
class intervaly{
private:
	std::vector<interval>	data;
public:
	void add(int lower=0, int upper=0, std::string label=std::string("")){
		data.push_back(interval(lower,upper,label));
	}
	interval find(std::string s){
		for(std::vector<interval>::iterator it=data.begin(); it!=data.end();++it){
			if(s==it->name) return *it;
		}
		return interval();
	}
} groups;

class flags{
public:
	bool pocisle;			// levy kontext  - [0-9]+["']*->[a-z];
	bool stahni_a;			// pravy kontext - sev slova + nasleduje 'a' -> lze vypustit koncove 'a'
	bool rozsir_o;			// pravy kontext - sev slova + nasleduje 'o' -> o lze ignorovat
	flags(bool po_c=false, bool sta_a=false, bool roz_o=false){
		pocisle=po_c;
		stahni_a=sta_a;
		rozsir_o=roz_o;
	}
};

class typeoftoken{
private:
	std::vector<znak> samohlasky;
public:
	bool cislo(int ret){
		interval i=groups.find("Cislice");
		return (i.lo<=ret && ret<=i.hi);
	}
	bool reckepismeno(int ret){
		interval i=groups.find("Alfabeta");
		return (i.lo<=ret && ret<=i.hi);
	}
	bool latinskepismeno(int ret){
		interval i=groups.find("Latinka");
		return (i.lo<=ret && ret<=i.hi);
	}
	bool prvek(int ret){
		interval i=groups.find("ZnackaPrvku");
		return (i.lo<=ret && ret<=i.hi);
	}
	bool carka(int ret){
		interval i=groups.find("Carka");
		return (i.lo<=ret && ret<=i.hi);
	}
	bool priponacharskupiny(int ret){
		interval i=groups.find("CharakteristickaSkupina");
		return(i.lo<=ret && ret<=i.hi);
	}
	bool samohlaska(znak c){
		for(std::vector<znak>::iterator it=samohlasky.begin(); it!=samohlasky.end(); ++it){
			if(*it == c)
				return true;
		}
		return false;
	}
	void add_samohlaska(znak c){
		samohlasky.push_back(c);
	}
} je;

//uzel lexikalniho stromu .. implementace je se zasobníkem na potomky, protoze #([a-zA-Z0-9.:;,-<>/()[]{} .. ]) je velky
class node{
public:
	static const int	noret=-1;	//navratova hodnota uzlu, ktery nevaraci
	znak				value;		//znak, ktery tento uzel reprezentuje
	int					ret;		//navratova hodnota
	flags				spec;		//kontextove zalezitosti
private:
	std::vector<node*>	childs;
	node*				parent;
	void konstrukt(const char* s, int r, flags f, node* p){
		parent=p;
		spec=f;
		if(!s || strlen(s)==0){
			value=0;
			ret=r;
			return;
		}
		value=s[0];
		if(strlen(s)>1){
			ret=noret;
			childs.push_back(new node(++s, r, f, this) );
		}else
			ret=r;
	}
public:
	node(const std::string &s, int r, flags f, node* p=0){
		konstrukt(s.c_str(),r,f,p);
	}
	node(const char* s, int r, flags f, node* p=0){
		konstrukt(s,r,f,p);
	}
	~node(){
		for(std::vector<node*>::iterator it=childs.begin();it!=childs.end();++it){
			delete *it;
			*it=0;
		}
	}
	//prida s jako vetev podstromu s navratovou hodnotou r
	void add(const char* s, int r, flags f){
		if(!s || strlen(s)==0){
			spec=f;
			ret=r;
			return;
		}
		for(std::vector<node*>::iterator it=childs.begin(); it!=childs.end(); ++it){
			if((*it)->value==(znak)s[0]){
				(*it)->add(s+1, r, f);
				return;
			}
		}
		childs.push_back(new node(s, r, f, this));
	}
	void add(const std::string &s, int r, flags f){
		add(s.c_str(),r,f);
	}
	//pøejde pøes potomka s hodnotou c
	node* over(znak c) const{
		for(std::vector<node*>::const_iterator it=childs.begin(); it!=childs.end(); ++it){
			if((*it)->value==c){
				return *it;
			};
		}
		return 0;
	}
	//najde posledni navratovou hodnotou na vetvi odpovidajici retezci s a do hl nastavi delku prislusne vetve
	node* find_first(const char* s, size_t &hl){
		if(s && *s){	//jeste je dost textu
			node* p=over(*s);	//najdi naslednika
			if(p){				//existuje naslednik
				p=p->find_first(++s,hl);	//zpracuj zbytek
				if(!p && ret!=-1)			//jsem posl. OK uzel
					return this;
				else
					if(!p)
						return 0;
					else{
						++hl;
						return p;
					}
			}else				//neex. naslednik
				if(ret!=-1)
					return this;			//jsem poslední OK uzel
				else
					return 0;				//nepatrim do terminalu
		}else			//retezec je prazdny
			if(ret!=-1)
				return this;
			else
				return 0;
	}
	node* find_first(std::string s, size_t &hl){
		return find_first(s.c_str(), hl);
	}
	//najde nejbližšího návratového rodièe
	node* find_prev(size_t &hl){
		if(hl<=0)
			return 0;
		--hl;
		if(parent)	//ma predchudce
			if(parent->ret!=noret)
				return parent;
			else
				return parent->find_prev(hl);
		//else		//nema predchudce
		return 0;
	}
	//vypíše svùj podstrom i s návratovými hodnotami
	void out(std::ostream &os, int i=0){
		if(parent)
			os<<value<<(ret==noret?"\t":"");
		if(ret!=noret) os<<"{"<<ret<<"}\t";
		for(std::vector<node*>::iterator it=childs.begin(); it!=childs.end(); ++it){
			if(it!=childs.begin()){
				os<<std::endl;
				for(int j=i; j; --j)
					os<<"\t";
			}
			(*it)->out(os, i+1);
		}
	}
};

//uzel+zaèátek v øetìzci -> node + index
class nodex{
public:
	size_t	idx;
	node*	nod;
	nodex(size_t i=0, node* n=0){
		idx=i;
		nod=n;
	};
};

//jmeno neterminalu a jeho hodnota
class strval{
public:
	int val;
	std::string str;
	strval(const std::string &s, int v=node::noret){
		val=v;
		str=s;
	}
};
/** globalni deklarace */
node				terminaly(0,node::noret,0);
std::stack<nodex>	parsed;
size_t				min2parse;
std::vector<strval>	seznam;

/** seznam konstant */
void inicializace_intervalu(std::istream &is){
	std::string s,s2;
	char c;
	int lo,hi;
	while(!is.eof()){
		is>>lo>>c>>hi>>c;	// " 0000-0000 -"
		if(c!='-')			// je to komentar / prazdny interval (c=='<')
			groups.add(lo,hi,"<comment/>");
		else{				// jsou to data
			is>>s>>s2;		// "Name (komentar,priklady,...)"
			groups.add(lo,hi,s);
		}
	}
}

/** postav strom */
void inicializace(std::istream &is){
	std::string s;
	int i=0;
	int c=0;

	while(!is.eof()){
		s="";
		++i;		//pokud neni uvedeno cislo, tak bude nastaven default - hodnota predchoziho+1
		is>>s>>i;
		if(!is.eof() && !is.good())	//pokud nebylo uvedeno cislo, tak je potreba nastavit good, jinak uz nic nenacteme
			is.clear();
		++c;		//pocita radky

		//prazdne radky preskoc
		if(	s=="" || s[0]!='\'' )
			continue;

		//nekorektni radky nahlas a preskoc
		if(!((s[0]=='\'' && s[s.length()-1]=='\'' && s.length()>2)	//spatny text
			&& i>=0)												//spatne ID tokenu
		){
			error(ERR_BAD_DATA_INPUT, c, i);
			continue;
		}

		//odstran apostrofy okolo
		s.replace(0,1,"");
		s.replace(s.length()-1,1,"");

		interval inter=groups.find("Samohlaska");
		if(inter.in(i)){	//je to samohlaska
			je.add_samohlaska(s[0]);
			continue;
		}

		//pridej do stromu
		inter=groups.find("Latinka");
		if(!inter.in(i))		//neni to pismenko latinky
			terminaly.add(s,i,flags(false, false));
		else					//je to pismenko latinky
			terminaly.add(s,i,flags(true, false));
		seznam.push_back(strval(s,i));		//pro funkci text

		//pokud je mozny stazeny tvar ve svu, tak pridej i toto oriznuti (metha -> meth)
		if(s.length()>1){
			switch(s[s.length()-1]){
				case 'a':
					s.replace(s.length()-1,1,"");
					terminaly.add(s,i,flags(false, true));
					break;
			}
		}
		//pokud je mozne rozsireni o "o", tak ho pridej
		//if(true /*&& s.length()>2 && !je.samohlaska(s[s.length()-1]) && s[s.length()-1]!='>'/**/){
		if(
			groups.find("JmenoPrvku").in(i) ||
			groups.find("CharakteristickaSkupina").in(i) ||
			(groups.find("ReckePocty").in(i) && !je.samohlaska(s[s.length()-1]))||
			groups.find("TrivKoren").in(i) ||
			groups.find("TrivNazvy").in(i)
		){
			terminaly.add(s+"o",i,flags(false,false,true));
		}
	}
	//vykresli strom (stream, indent[# '\t'])
#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_DATABASE
	std::cout<<"LEXIKALNI STROM:"<<std::endl<<"----------------"<<std::endl;
	terminaly.out(std::cout);
	std::cout<<std::endl;
#endif
}

/* najdi hodnotu pro dany terminál */
int hodnota(std::string s){
	size_t i=0;
	node *n=terminaly.find_first(s.c_str(), i);
	if(n && s.length()==i)	//nalezl a není to jen prefix
		return n->ret;
	else
		return node::noret;
}
/* najdi neterminal pro danou hodnotu */
std::string text(int i){
	for(std::vector<strval>::const_iterator it=seznam.begin();it!=seznam.end();++it){
		if(it->val==i) return it->str;
	}
	return "";
}

/* rozparsuj jmeno molekuly */
bool pras(const char* s, const int predchozi){
	min2parse=min2parse<strlen(s)?min2parse:strlen(s);
	if(s[0]==0)		// hotovo (vse)
		return true;

	size_t i=0;
	node* n=terminaly.find_first(s,i);

	if(!n)			//nelze
		return false;

	//kontext:
	if(n->spec.pocisle &&		//tvar [0-9"'][a-z]¤
	   !(je.cislo(predchozi)
	   ||
		 je.carka(predchozi)
	    )
	  ){
		return false;
	}
	/*if((n->spec.stahni_a && !(s+i==0 || je.samohlaska(s[i])))		//tvar "pent¤hexan"
			//||
	  // (je.samohlaska(n->value) &&
	  //  s+i!=0 && je.samohlaska(s[i]))			//tvar "but(a)¤an", ale "1,2,3-tri¤en"
	){
		return false;
	}
	/ **/

	if(pras(s+i,n->ret)){		//povedlo se
		parsed.push( nodex(i,n) );
		return true;
	}
	//nepovedlo se
	while(i && ((n=n->find_prev(i))!=0)){ //pro blby prekladace
		if(pras(s+i, n->ret)){	//povedlo se
			parsed.push( nodex(i,n) );
			return true;
		}
	}
	return false;
};

/* skutecny pars jmena molekuly */
void parse(std::string jmeno){
	size_t start=0;
	min2parse=jmeno.length();
	pras(jmeno.c_str(),-1);
	if(parsed.empty()){
		start=jmeno.length()-min2parse;
		error(ERR_UNKNOWN_TOKEN, start+1, jmeno.substr(start, min2parse).c_str() );
	}
	#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_TOKENS
		std::cout<<"TOKENY:"<<std::endl<<"-------"<<std::endl;
	#endif
}

/* dej dalsi token */
int yylex_priprav(lexval *param, unsigned int &token){
	nodex nx;
	interval inter;
	++token;

	if(!parsed.empty()){
		nx=parsed.top();
		param->_len=nx.idx;
		parsed.pop();

		//negrupovany element...
		return nx.nod->ret;
	}
	//else
	return 0;
}

int yylex(lexval *param, unsigned int &token){
	int x = yylex_priprav(param, token);

	#if defined DEBUG_PRINTS && defined DEBUG_PRINTS_TOKENS && !defined DEBUG_LEXONLY
		if(x){
			std::cout<<"Token No. "<<token<<":\t";
		}else{
			std::cout<<"<<END>>:\t";
		}
			std::cout<<text(x)<<"("<<x<<")\t"<<*param<<std::endl;
	#endif

	return x;
}
}
