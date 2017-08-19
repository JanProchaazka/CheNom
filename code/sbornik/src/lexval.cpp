#include "h/lexval.hpp"

std::ostream& operator<<(std::ostream& os, lexval l){
	return os<<"("<<l._len<<", '"<<l._uchar<<"', "<<l._int<<")";
}

