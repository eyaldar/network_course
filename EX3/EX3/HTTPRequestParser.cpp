#include "HTTPRequestParser.h"

using namespace std;

string rtrim(string str)
{
	int index = str.length();

	while ((str.at(index - 1) == ' ' || str.at(index - 1) == '\r') && index - 1 > 0)
	{
		index--;
	}

	str.erase(index, str.length());
	return str;
}


string ltrim(string str)
{
	int index = 0;
	do
	{
		char c = str.at(index);
		if (c != ' ' && c != '\r')
		{
			str.erase(0, index);
			break;
		}

		index++;
	} while (str.at(index) != '\0');

	return str;
}

// trim from both ends
string trim(string s)
{
	return ltrim(rtrim(s));
}


map<string, string> parse_request(const char* data)
{
	map<string, string> m;

	istringstream resp(data);
	string header;
	string::size_type sep;

	getline(resp, header);
	sep = header.find(' ', 0);

	if (sep != std::string::npos)
	{
		m.insert(std::make_pair(METHOD_KEY, header.substr(0, sep)));

		string::size_type sep2 = header.find(' ', sep + 1);
		if (sep != std::string::npos)
		{
			m.insert(std::make_pair(PATH_KEY, trim(header.substr(sep + 2, sep2 - (sep + 2)))));
			m.insert(std::make_pair(HTTP_VER_KEY, trim(header.substr(sep2 + 1))));
		}
	}

	while (getline(resp, header) && header != "\r")
	{
		sep = header.find(':', 0);
		if (sep != std::string::npos) {
			m.insert(std::make_pair(
				trim(header.substr(0, sep)),
				trim(header.substr(sep + 1))
				));
		}
	}

	getline(resp, header);

	if (!header.empty())
	{
		int bodyIndex = (int)resp.tellg();

		string body = resp.str().substr(bodyIndex, resp.str().length());
		m.insert(std::make_pair(
			BODY_KEY, body
			));
	}

	return m;
}