#include "HTTPServer.h"
#include <iostream>

using namespace std;

struct TextHandler : public Handler
{
	TextHandler(const std::string& text)
	: text(text)
	{	
	}

	virtual std::string handleRequest()
	{
		return text;
	}
private:
	std::string text;	
};

int main(int argc, char** argv)
{
	HTTPServer server(8080);

	boost::shared_ptr<Handler> indexHandler(new TextHandler("Welcome to my super server!"));
	server.addRoute("/", indexHandler);

	boost::shared_ptr<Handler> secretHandler(new TextHandler("You found the secret page! Sssh..."));
	server.addRoute("/secret", secretHandler);

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