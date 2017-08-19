#ifndef CHENOM_UTILS_H_
#define CHENOM_UTILS_H_
#include <string>

bool isws(char c);
std::string trimWS(std::string s);
size_t str2int(const std::string & s);
std::string int2str(size_t i);
std::string dbl2str(double i);
double sgn(double x);
size_t HEX2DEC(std::string s);
int colorconvert(std::string HTML, double &R, double &G, double &B);
std::string urldecode(const std::string &s);
std::string urlencode(const std::string &s);
bool urlencoded(const std::string &s);
std::string alphabeta2utf8(const std::string &s);
std::string string_printf(const char* format, ...);
#endif
