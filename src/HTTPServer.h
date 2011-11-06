#include <map>
#include <string>
#include <vector>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Handler.h"

class HTTPServer : private boost::noncopyable
{
public:
	typedef std::map<std::string, std::string> Params;

	HTTPServer(int port);

	void addRoute(const char* url, boost::shared_ptr<Handler>& handler);

	void run();

private:

	void bind();

	void dispatchRequest(int confd, const std::vector<std::string>& lines);

	void write(int confd, const std::string& out);

	typedef std::map<std::string, boost::shared_ptr<Handler> > HandlerMap;
	HandlerMap handlers;

	int sockfd;
	int confd;
	int port;

	sockaddr_in serv_addr;
	sockaddr_in cli_addr;
	socklen_t len_cli_addr;

	static const int BUF_SIZE = 1024;

	// Enough room for a big ol request hopefully!
	char sock_buf[BUF_SIZE];
};