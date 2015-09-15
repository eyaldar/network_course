#include <string.h>
#include <map>
#include <sstream>

#define BODY_KEY "Body"
#define METHOD_KEY "Method"
#define PATH_KEY "Path"
#define HTTP_VER_KEY "HTTP-Version"

std::string rtrim(std::string str);
std::string ltrim(std::string str);
std::string trim(std::string s);
std::map<std::string, std::string> parse_request(const char* data);
