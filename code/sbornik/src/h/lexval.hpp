#ifndef CHENOM_LEXVAL_H_
#define CHENOM_LEXVAL_H_
	#include <iostream>

	#define znak unsigned char

	struct lexval{
		int				_int;
		znak			_uchar;
		size_t			_len;
		lexval(){
			clear();
		}
		void clear(){
			_int=0;
			_uchar=0;
			_len=0;
		}
	};

	std::ostream& operator<<(std::ostream&, lexval);
#endif
