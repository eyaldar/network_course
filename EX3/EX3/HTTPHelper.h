#include <string.h>
#include <map>
#include <sstream>

#define OK "200 OK"
#define CREATED "201 Created"
#define NO_CONTENT "204 No Content"
#define NOT_FOUND "404 Not Found"
#define PRECONDITION_FAILED "412 Precondition Failed"
#define NOT_IMPLEMENTED "500 Not Implemented"

#define MESSAGE_CONTENT_TYPE "message/http"
#define HTML_CONTENT_TYPE "text/html"

#define ALLOWED_METHODS "HEAD,GET,PUT,DELETE,OPTIONS"

#define CONTENT_LENGTH_KEY "Content-Length"
#define DATE_KEY "Date"
#define CONTENT_TYPE_KEY "Content-Type"
#define BODY_KEY "Body"
#define ALLOW_KEY "Allow"
#define METHOD_KEY "Method"
#define PATH_KEY "Path"
#define HTTP_VER_KEY "HTTP-Version"
#define STATUS_CODE_KEY "Status"

std::string rtrim(std::string str);
std::string ltrim(std::string str);
std::string trim(std::string s);
std::map<std::string, std::string> parse_request(const char* data);
std::string build_response(std::map<std::string, std::string> response_params);
