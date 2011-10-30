#include "HTTPServer.h"
#include <iostream>

using namespace std;

struct TextHandler
{
	TextHandler(const std::string& text)
	: text(text)
	{	
	}

	HTTPServer::Response operator()(const HTTPServer::HTTPParams& params)
	{
		HTTPServer::Response resp;
		resp.body = text;

		return resp;
	}
private:
	std::string text;	
};

int main(int argc, char** argv)
{
	HTTPServer server(8080);

	server.addRoute("/", HTTPServer::Handler(TextHandler("Welcome to my super server!")));
	server.addRoute("/secret", HTTPServer::Handler(TextHandler("You found the secret page! Sssh...")));

	try
	{
		server.run();
	}
	catch(exception& ex)
	{
		cout << ex.what() << endl;
	}

	return 0;
}