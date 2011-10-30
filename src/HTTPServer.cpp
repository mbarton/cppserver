#include "HTTPServer.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;
using namespace std;

HTTPServer::HTTPServer(int port)
: acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	
}

void HTTPServer::addRoute(const char* url, Handler handler)
{
	handlers[url] = handler;
}

void HTTPServer::run()
{
	while(true)
	{
		tcp::socket socket(io_service);

		acceptor.accept(socket);

		read(socket);

		std::vector<std::string> lines;
		boost::split(lines, sock_buf, boost::is_any_of("\n"));
		
		dispatchRequest(socket, lines);
	}
}

void HTTPServer::dispatchRequest(tcp::socket& socket, const std::vector<std::string>& lines)
{
	// The first line should be the HTTP request verby bit
	// eg GET / HTTP/1.1
	const std::string& first_line = lines[0];

	// Split by space to get VERB, RESOURCE and PROTCOL_VER
	vector<string> tokens;
	boost::split(tokens, first_line, boost::is_any_of(" "));

	// Only support GET at the moment
	if(tokens[0] == "GET")
	{
		// Find our handler!
		HandlerMap::iterator it_handler = handlers.find(tokens[1]);
		if(it_handler != handlers.end())
		{
			write(socket, "HTTP/1.1 200 OK\n\n");
			
			HTTPParams empty;
			Handler& handler = it_handler->second;

			Response r = handler(empty);

			write(socket, r.body);
			write(socket, "\n");
		}
		else
		{
			write(socket, "HTTP/1.1 404 Not Found\n\nYou done did a fall over, that page isn't here\n");
		}
	}
	else
	{
		write(socket, "HTTP/1.1 405 Method Not Allowed\n\nYou done did a fall over, only GET is supported\n");
	}
}

void HTTPServer::read(tcp::socket& socket)
{
	boost::system::error_code error_code;
	socket.read_some(boost::asio::buffer(sock_buf), error_code);
}

void HTTPServer::write(tcp::socket& socket, const std::string& out)
{
	boost::system::error_code error_code;
	boost::asio::write(socket, boost::asio::buffer(out), error_code);
}