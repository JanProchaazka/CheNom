#ifdef WIN32
	#define WINDOWS
#else
	#define LINUX
#endif

#ifdef WINDOWS
	#pragma comment(lib, "wsock32.lib")

	#include <windows.h>
	
	#define UKLID WSACleanup()
	#define ZAVRI closesocket
	#define PORTtyp u_short
	#define socklen_t int
	#define EXECUTIVE_SCRIPT ".\\run.bat"
#endif

#ifdef LINUX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	#define byte unsigned char
	#define INVALID_SOCKET (-1)
	#define SOCKET_ERROR (-1)
	#define SOCKET int
	#define UKLID
	#define ZAVRI close
	#define PORTtyp int
	#define EXECUTIVE_SCRIPT "./run.sh"
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "h/utils.hpp"

#define BUFSIZE 1000

using namespace std;

bool pokracuj;
string sablona_stranky;


int inicializace(){
	ifstream F("index.htm");
	if(!F.good()){
		cerr<<"Cannot open file ('index.htm')!"<<endl;
		return 1;
	}

	std::string s="";
	F>>noskipws;
	while(!F.eof()){
		char c;
		F>>c;
		s+=c;
	}
	F.close();

	sablona_stranky=s;
	return 0;
}

string urlescape(const std::string &str){
	static char digits[]="0123456789ABCDEF";
	string ret("");

	for(std::string::const_iterator it=str.begin(); it!=str.end(); ++it){
		if(
			(*it=='\\')				||
			(*it=='.')				||
			('a'<=*it && *it<='z')	||
			('A'<=*it && *it<='Z')	||
			('0'<=*it && *it<='9')	||
//			(*it=='_')				||	//pro potreby CheNom-u nevhodne
//			(*it=='*')				||
			(*it=='-')				||
			(*it=='%')
		){
			//je to ok
			ret+=*it;
		}else{
			ret+="%";
			ret+=digits[(unsigned char)(*it)/16];
			ret+=digits[(unsigned char)(*it)%16];
		}
	}
	return ret;
}

string molekula(string jmeno){
	string vrat="";

	//osetri vstup:
	if(!urlencoded(jmeno)){
		cerr<<"ERROR: '"<<jmeno<<"' - no correct urlencoded string!!!"<<endl;
		return "<center><p class=\"error\" >Error: There is some incorrect character in the name!!!</p></center>\r\n";
	}

	//sestav prikaz:
	string calling=EXECUTIVE_SCRIPT;
	calling+=" \"";
	calling+=jmeno;
	calling+="\"";

	//proved prikaz:
	int ret=system(calling.c_str());
	
	//vrat vystup, bud chybovy, nebo model:
	if(ret==0)
		vrat+="<embed src=\"outs/" + jmeno + ".wrl\" class=\"vystup\">\r\n";
		//vrat+="<iframe width=\"100%\" height=\"100%\" src=\"outs/"+jmeno+".wrl\" />\r\n";
	else{
		ifstream F("error.log");
		F>>noskipws;
		vrat+="<p class=\"error\" >Chybovy vypis:<br />\r\n";
		while(F.good()){
			char c;
			F>>c;
			if(c=='\r' || c=='\n')
				vrat+="<br />\r\n";
			else
				vrat+=c;
		}
		F.close();
		vrat+="</p>\r\n";
	}
	return vrat;
}
///////////////////////////////////////////////////////
string oprav_separatory(string s){
#ifdef WINDOWS
	size_t pos=s.find("/");
	while(pos!=string::npos){
		s.replace(pos,1,"\\");
		pos=s.find("/");
	}
#endif
	return s;
}

string hlava_http(string error="200 OK"){
	string vrat="";
	vrat+="HTTP/1.0 "+error+"\r\n";
	//vrat+="Cache-Control: no-store, no-cache, max-age=1, s-maxage=1, must-revalidate, post-check=0, pre-check=0\r\n";
	vrat+="Connection: close\r\n";
	//vrat+="Date: Sun, 20 Jun 2004 13:36:19 GMT\r\n";
	vrat+="Pragma: no-cache\r\n";
	//vrat+="ETag: 300909a30597d76f8590cd4a60a340aa\r\n";
	vrat+="Server: CheNom/0.0.1\r\n";
	vrat+="Content-Type: text/html\r\n";
	//vrat+="Expires: Sat, 18 Aug 2002 05:30:00 GMT\r\n";
	//vrat+="Last-Modified: Sun, 20 Jun 2004 13:36:19 GMT\r\n";
	//vrat+="Client-Date: Sun, 20 Jun 2004 13:36:13 GMT\r\n";
	//vrat+="Client-Response-Num: 1\r\n";
	vrat+="X-Powered-By: CheNom/0.0.1\r\n";
	vrat+="\r\n";
	return vrat;
}
string soubor_http(string filename, string typ="image/x-icon"){
	if(filename[0]=='/')
		filename.erase(0,1);
	filename=urlescape(oprav_separatory(filename));
	string vrat="";
	string error="200 OK";
	ifstream F;
	vector<byte> DATA;

	F.open(filename.c_str());
	F>>noskipws;
	if(!F.good())
		error="404 Not found";
	while(F.good()){
		byte b;
		F>>b;
		DATA.push_back(b);
	}
	F.close();

	vrat+="HTTP/1.0 "+error+"\r\n";
	//vrat+="Date: Sun, 20 Jun 2004 13:36:19 GMT\r\n";
	vrat+="Server: CheNom/0.0.1\r\n";
	//vrat+="Last-Modified: Sun, 20 Jun 2004 13:36:19 GMT\r\n";
	//vrat+="ETag: 300909a30597d76f8590cd4a60a340aa\r\n";
	vrat+="Accept-Ranges: bytes\r\n";
	if(error=="200 OK"){
		vrat+="Content-Length: "+int2str(DATA.size())+"\r\n";
		vrat+="Content-Type: "+typ+"\r\n";
	}
	vrat+="Connection: close\r\n";
	vrat+="\r\n";

	if(error!="200 OK")
		return vrat;
	else{	//vloz soubor:
		for(vector<byte>::const_iterator it=DATA.begin(); it!=DATA.end(); ++it){
			vrat+=*it;
		}
		return vrat;
	}
}

string telo(string nadpis="", string obsah=""){
	string vrat=sablona_stranky;
	size_t poz;
	poz=vrat.find("$NAZEV$");
	while(poz!=string::npos){
		vrat.replace(poz, string("$NAZEV$").length(), nadpis);
		poz=vrat.find("$NAZEV$");
	}

	poz=vrat.find("$NAZEV*$");
	while(poz!=string::npos){
		vrat.replace(poz, string("$NAZEV*$").length(), urlencode(nadpis));
		poz=vrat.find("$NAZEV*$");
	}
	poz=vrat.find("$TELO$");
	while(poz!=string::npos){
		vrat.replace(poz, string("$TELO$").length(), obsah);
		poz=vrat.find("$TELO$");
	}
	return vrat;
}

string zpracuj(string s){
	//zpracuj:
	size_t konec=s.find("\r\n");
	if(konec==string::npos){
		cerr<<"ERROR: 1-line request!!!"<<endl;
		return hlava_http("300 spatny vstup")+telo("ERROR", "spatny vstup - chci vic radku\r\n");
	}
	string radek=s.substr(0,konec); //GET /chenom?mol=butan HTTP/1.1
	//odrizni zacatek:
	if(radek.substr(0,4)=="GET "){
		radek.erase(0,4);
	}else{
		cerr<<"ERROR: GET requests only!!!"<<endl;
		return hlava_http("300 spatny vstup")+telo("ERROR", "spatny vstup - chci GET\r\n");
	}
	//odrizni konec:
	konec=radek.find(" ");
	if(konec==string::npos){
		cerr<<"ERROR: PROTOCOL specification expected!!!"<<endl;
		return hlava_http("300 spatny vstup")+telo("ERROR", "spatny vstup - chci HTTP/1.0 (treba)\r\n");
	}else{
		radek.erase(konec);
	}

	//soubor:
	konec=radek.find("?");
	string domena,dotaz;
	if(konec==string::npos)
		dotaz="";
	else
		dotaz=radek.substr(konec+1);
	domena=radek.substr(0,konec);
	if(domena!="/chenom"){
		//zastav server:
		if(radek=="/STOP"){
			pokracuj=false;
			return hlava_http()+telo("STOP", "OK");
		}else
		if(radek=="/RESET"){
			if(inicializace())
				return hlava_http()+telo("RESET", "KO");
			else
				return hlava_http()+telo("RESET", "OK");
		}else{
			//je to ikonka:
			if(radek=="/favicon.ico"){
				return hlava_http("404 Not found");
				//return soubor_http(radek, "image/x-icon");
			}else{
				//je to molekula:
				if(radek.length()>=4 
					&& radek.substr(radek.length()-4,4)==".wrl"
					&& radek.find("/outs")==0
				){
					return soubor_http(radek,"x-world/x-vrml");
				}else{
					//je to neco nedovoleneho
					return hlava_http("404 slozka neexistuje")+telo("ERROR", "Tento pozadavek neni z&nbsp;bezpecnostnich duvodu mozne uspokojit. Pouzivejte pouze odkazy z&nbsp;hlavni stranky <a href=\"/chenom\">CheNom</a>.\r\n");
				}
			}
		}
	}
	//je to dotaz:
	if(dotaz.length()==0){//zakladni prazdny form:
		return hlava_http()+telo("CheNom", "Do kolonky vyse vyplnte jmeno molekuly urcene k&nbsp;zobrazeni!");
	}

	if(dotaz.substr(0,4)!="mol="){
		return hlava_http("404 spatny dotaz")+telo("ERROR", "spatny dotaz - vyplnte radej formular;)");
	}else{
		dotaz.erase(0,4);
	}

	return hlava_http()+telo(urldecode(dotaz), molekula(dotaz)+"\r\n");
}

int main(int argc, char *argv[]){
#ifdef WINDOWS
    WORD wVersionRequested = MAKEWORD(1,1); // Cislo verze
    WSADATA data;           // Struktura s info. o knihovne;
#endif
	SOCKET mainSocket;      // Soket
    string text;			// Prijimany text
    sockaddr_in sockName;   // "Jmeno" soketu a cislo portu
    sockaddr_in clientInfo; // Klient, ktery se pripojil 
	socklen_t addrlen;      // Velikost adresy vzdaleneho pocitace
    PORTtyp port;           // Cislo portu
    char buf[BUFSIZE];      // Prijimaci buffer
	size_t size;            // Pocet prijatych a odeslanych bytù

    if (argc != 2){
        cout<<"Syntax:\r\n\t"<<argv[0]<<" <port>"<<endl;
        return -1;
    }

	if(inicializace()){
		cerr<<"ERROR: Cannot open index.htm"<<endl;
		return -1;
	}
#ifdef WINDOWS
    // Pripravime sokety na praci
    if(WSAStartup(wVersionRequested, &data) != 0){
		cerr<<"ERROR: Cannot inicialize sockets!!!"<<endl;
        return -1;
    }
#endif
    port = (PORTtyp)str2int(argv[1]);
    // Vytvorime soket - viz minuly dil
    if((mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET){
		cerr<<"ERROR: Cannot create socket!!!"<<endl;
		UKLID;
		return -1;
    }
    // Zaplnime strukturu sockaddr_in
    // 1) Rodina protokolu;
    sockName.sin_family = AF_INET;
    // 2) Cislo portu, na kterem cekame
    sockName.sin_port = htons(port);
    // 3) Nastavení IP adresy lokalni sitove karty, pres kterou 
    // je mozno se pripojit. 
    // Nastavime moznost pripojit se odkudkoliv. 
    sockName.sin_addr.s_addr = INADDR_ANY;
    //priradime soketu jméno
    if(bind(mainSocket, (sockaddr *)&sockName, sizeof(sockName)) == SOCKET_ERROR){
		cerr<<"ERROR: Cannot bind the socket!!!"<<endl;
		UKLID;
		return -1;
    }
    // Vytvorime frontu pozadavku na spojeni. 
    // Vytvorime frontu maximalni velikosti 10 pozadavku.
    if(listen(mainSocket, 10) == SOCKET_ERROR){
		cerr<<"ERROR: Cannot create the socket-front!!!"<<endl;
		UKLID;
		return -1;
    }

	//priprav smycku.. zastavi az na spec. prikaz:
	pokracuj=true;
    do{
        // Poznacim si velikost struktury clientInfo.
        // Predam to funkci accept. 
        addrlen = sizeof(clientInfo);
        // Vyberu z fronty pozadavek na spojeni.
        // "client" je novy soket spojujici klienta se serverem.
        SOCKET client = accept(mainSocket, (sockaddr*)&clientInfo, &addrlen);
        size_t totalSize = 0;
        if (client == INVALID_SOCKET){
				cerr<<"ERROR: Cannot accept the incoming binding request!!!"<<endl;
                UKLID;
                return -1;
        }
        // Zjistim IP adresu klienta.
        cout<<"INCOMING REQUEST FROM: "<<inet_ntoa((in_addr)clientInfo.sin_addr)<<endl;
        // Prijmu data. Ke komunikaci s klientem pouzivam soket "client"
        text = "";
        // Prijmeme celou hlavicku
		int cekaci_doba=0;
		while((totalSize<=4 || 
			!(text[totalSize-4]==13 && text[totalSize-3]==10 && text[totalSize-2]==13 && text[totalSize-1]==10))
				&&
			cekaci_doba<=10
			)
		{
                if ((size = recv(client, buf, BUFSIZE - 1, 0)) 
                          == SOCKET_ERROR)
                {
						cerr<<"ERROR: Data income failed!!!"<<endl;
                        UKLID;
                        return -1;
                }
				buf[size]=0;
				cout<<"INCOMED: "<<size<<endl;
                totalSize += size;
                text += buf;
				++cekaci_doba;
        }
        cout << text;	//vypis co prislo
        // Odesleme odpoved
		text=zpracuj(text);
		cout<<"--------------------------------------------------------------------------------"<<endl;
		cout<<text;		//vypis co odeslo
        if ((size = send(client, text.c_str(), (int)text.length(), 0))==SOCKET_ERROR){
			cerr<<"ERROR: Data sending failed!!!"<<endl;
                UKLID;
                return -1;
        }
        cout<<"SENT: "<<size<<endl;
		cout<<"================================================================================"<<endl;
        // Uzavru spojeni s klientem
        ZAVRI(client);
    }while(pokracuj);

    cout<<"TERMINATING"<<endl;

	ZAVRI(mainSocket);
    UKLID;    
    return 0;
}
