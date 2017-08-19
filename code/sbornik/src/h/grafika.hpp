#ifndef CHENOM_GRAFIKA_H_
#define CHENOM_GRAFIKA_H_
#include "model.hpp"
#include <vector>
#include <string>
#include "utils.hpp"
#include <fstream>

namespace grafika{
	void inicializace(std::istream &is);
	std::string tist(const semantika::Prvek &p);
	std::string tisk(const model::atom &a);
	std::string vazba_tisk(model::bod Z, model::bod K, unsigned char arita);
	std::string tisk(const model::skelet &s);
}
#endif
