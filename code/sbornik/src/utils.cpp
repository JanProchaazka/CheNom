#include "h/utils.hpp"
#include <cstdarg>

/**/bool isws(char c){
		return (c==' ')||(c=='\t')||(c=='\r')||(c=='\n');
	}
	std::string trimWS(std::string s){
		size_t z=0, k=s.length()-1;
		while(s.length()>z && isws(s[z]))++z;
		while(k>=0 && k>z && isws(s[k])) --k;
		if(z>k)
			s="";
		else
			s=s.substr(z,k-z+1);

		return s;
	}
/**/size_t str2int(const std::string & s){	//vynecha pocatecni bile znaky a pak vezme nasledujici cislice a ty vrati jako size_t
		size_t c=0;
		for(size_t i=0; i<s.length(); ++i){
			if('0'<=s[i] && s[i]<='9'){
				c*=10;
				c+=(s[i]-'0');
			}else{
				if(isws(s[i]) && c==0){
				}else{
					return c;
				}
			}
		}
		return c;
	}
	std::string int2str(size_t i){
		std::string s;
		if(i==0)
			return std::string("0");
		while(i>0){
			s=char('0'+(i%10))+s;
			i/=10;
		}
		return s;
	}
	std::string dbl2str(double i){
		std::string s="";
		if(i==0) return "0.";
		if(i<0){
			s+="-";
			i=-i;
		}
		if(i>=1.) s+=int2str((size_t)i);		//cela cast
		s+=".";
		for(size_t k=0; k<8; ++k){		//desetinna cast
			i=i-size_t(i);
			i*=10;
			s+=int2str((size_t)i);
		}
		return s;
	}
	double sgn(double x){
		return (x>0)?1:(x==0?0:-1);
	}
	size_t HEX2DEC(std::string s){
		size_t val=0;
		for(size_t i=0; i<s.length(); ++i){
			char c=s[i];
			if('0'<=c && c<='9'){
				val=16*val+(c-'0');
				continue;
			}
			if('a'<=c && c<='f'){
				val=16*val+(10+c-'a');
				continue;
			}
			if('A'<=c && c<='F'){
				val=16*val+(10+c-'F');
				continue;
			}
			//error(bad char in hex number);
			return std::string::npos;
		}
		return val;
	}

	int colorconvert(std::string HTML, double &R, double &G, double &B){
		if(HTML.length()!=7){
			//error(spatna definice barvy)
			return 1;
		}
		if(HTML[0]!='#'){
			//error(spatna definice barvy)
			return 2;
		}
		size_t t;
		if(std::string::npos==(t=HEX2DEC(HTML.substr(1,2)))){
			//noerror()
			R=G=B=0;
			return 3;
		}else{
			R=1.0/255*t;
		};
		if(std::string::npos==(t=HEX2DEC(HTML.substr(3,2)))){
			//noerror()
			R=G=B=0;
			return 3;
		}else{
			G=1.0/255*t;
		};
		if(std::string::npos==(t=HEX2DEC(HTML.substr(5,2)))){
			//noerror()
			R=G=B=0;
			return 3;
		}else{
			B=1.0/255*t;
		};

		return 0;
	}

	std::string urldecode(const std::string &s){
	// http://www.koders.com/cpp/fid6315325A03C89DEB1E28732308D70D1312AB17DD.aspx - upraveno
		std::string buffer = "";
		for (size_t i = 0; i < s.length(); ++i){
			size_t j = i;
			char ch = s.at(j);
			switch(ch){
				case '%':
					{
					char tmpstr[] = "0x0__";
					long chnum;
					tmpstr[3] = s.at(j+1);
					tmpstr[4] = s.at(j+2);
					chnum = strtol(tmpstr, NULL, 16);	  
					buffer += (char)chnum;
					i+=2;
					break;
					}
				case '+':
					buffer += ' ';
					break;
				default:
					buffer += ch;
					break;
			}
		}
		return buffer;
	}

	std::string urlencode(const std::string &s){
	//prevede text, aby se v HTML zobrazil tak jak je zadan
		std::string ret=s;
		size_t poz=0;
		while(std::string::npos!=(poz=ret.find("&",poz))){
			ret=ret.replace(poz++,std::string("&").length(),"&amp;");
		}

		return ret;
	}

	bool urlencoded(const std::string &s){
		for(std::string::const_iterator it=s.begin(); it!=s.end(); ++it){
			if(
				('a'<=*it && *it<='z')	||
				('A'<=*it && *it<='Z')	||
				('0'<=*it && *it<='9')	||
//				(*it=='_')				||	//pro potreby CheNom-u nevhodne
//				(*it=='*')				||
				(*it=='-')				||
				(*it=='%')
			){
				//je to ok
			}else
				return false;
		}
		return true;
	}

	std::string alphabeta2utf8(const std::string &s){
		std::string ret=s;
		char* alfabeta[] = {
			"&alpha;",
			"&beta;",
			"&gamma;",
			"&delta;",
			"&epsilon;",
			"&zeta;",
			"&eta;",
			"&theta;",
			"&iota;",
			"&kappa;",
			"&lambda;",
			"&mu;",
			"&nu;",
			"&xi;",
			"&omikron;",
			"&pi;",
			"&rho;",
			"&varsigma;",	//neexistuje, imaginarne, kvuli utf8, kde je!
			"&sigma;",
			"&tau;",
			"&upsilon;",
			"&phi;",
			"&chi;",
			"&psi;",
			"&omega;"
		};

		char** it=alfabeta;
		for(char i=1;i<16;++i,++it){	//nahrad prvni polovinu
			size_t poz=ret.find(*it);
			while(poz!=std::string::npos){	//dokud je nalezen vyskyt, nahrazuj
				ret.replace(poz,strlen(*it),"  ");
				ret[poz]=(unsigned char)(0xce);
				ret[poz+1]=(unsigned char)(0xb0+i);
				poz=ret.find(*it);
			}
		}
		for(char i=0;i<10;++i,++it){	//nahrad druhou polovinu
			size_t poz=ret.find(*it);
			while(poz!=std::string::npos){	//dokud je nalezen vyskyt, nahrazuj
				ret.replace(poz,strlen(*it),"  ");
				ret[poz]=(unsigned char)(0xcf);
//				ret[poz+1]=char(0x80+i+(i>=2?1:0));	//je potreba vynechat koncovou sigmu (i==2)
				ret[poz+1]=(unsigned char)(0x80+i);
				poz=ret.find(*it);
			}
		}

		return ret;
	}
	std::string string_printf(const char* format, ...){
		std::string po_dosazeni;
		va_list data;
		char c;
		int d;
		std::string s;
		size_t i=0;
		va_start(data, format);

		while (format[i]){
			if (format[i]=='%'){
				i++;
				switch (format[i]){
				case 'd':
					d=va_arg(data,int);
					if(d<0){
						c='-';
						d=-d;
					}else
						c=' ';
					if(d==0){
						s="0";
					}else{
						s="";
						while(d!=0){
							s=char('0'+(d%10))+s;
							d=d/10;
						}
					}
					if(c=='-')
						po_dosazeni+=c;
					po_dosazeni+=s;
					break;
				case 'c':
					//nelze pres ... posilat char (linux rve!)B
					po_dosazeni+='?';
					break;
				case 's':
					po_dosazeni+=va_arg(data, char*);
					break;
				case '%':
					po_dosazeni+='%';
					break;
				};
			}
			else 
				po_dosazeni+=format[i];
			i++;
		};
		va_end(data);
		return po_dosazeni;
	}

