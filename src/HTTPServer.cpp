#include "HTTPServer.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>

using namespace std;

HTTPServer::HTTPServer(int port)
: port(port), len_cli_addr(sizeof(cli_addr))
{
	// Clear out the gunk memory in serv_addr
	bzero((char*) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	bind();
}

void HTTPServer::addRoute(const char* url, boost::shared_ptr<Handler>& handler)
{
	handlers[url] = handler;
}

void HTTPServer::run()
{
	while(true)
	{
		confd = accept(sockfd, (sockaddr*)&cli_addr, &len_cli_addr);

		if(confd < 0)
			throw runtime_error("Error accepting connection");
		
		int len_read = read(confd, sock_buf, BUF_SIZE);

		if(len_read < 0)
			throw runtime_error("Error reading from socket");

		std::vector<std::string> lines;
		boost::split(lines, sock_buf, boost::is_any_of("\n"));
		
		dispatchRequest(confd, lines);

		close(confd);
	}
}

void HTTPServer::bind()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
		throw runtime_error("Error creating socket");
	
	if(::bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		throw runtime_error("Error binding to port");
	
	if(listen(sockfd, 5) < 0)
		throw runtime_error("Error listening to port");
}

void HTTPServer::dispatchRequest(int confd, const std::vector<std::string>& lines)
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
			write(confd, "HTTP/1.1 200 OK\n\n");
			
			boost::shared_ptr<Handler> handler = it_handler->second;

			std::string r = handler->handleRequest();

			write(confd, r.c_str());
			write(confd, "\n");
		}
		else
		{
			write(confd, "HTTP/1.1 404 Not Found\n\nYou done did a fall over, that page isn't here\n");
		}
	}
	else
	{
		write(confd, "HTTP/1.1 405 Method Not Allowed\n\nYou done did a fall over, only GET is supported\n");
	}
}

void HTTPServer::write(int confd, const std::string& out)
{
	int len_w = ::write(confd, out.c_str(), out.size());

	if(len_w < 0)
		throw runtime_error("Error writing to socket");
}