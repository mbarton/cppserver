#include <map>
#include <string>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

class HTTPServer : private boost::noncopyable
{
public:
	typedef std::map<std::string, std::string> HTTPParams;
	
	struct Response
	{
		std::string body;
		HTTPParams params;	
	};

	typedef boost::function<Response (const HTTPParams& params)> Handler;

	HTTPServer(int port);

	void addRoute(const char* url, Handler handler);

	void run();

private:

	void dispatchRequest(boost::asio::ip::tcp::socket& socket, const std::vector<std::string>& lines);

	void read(boost::asio::ip::tcp::socket& socket);

	void write(boost::asio::ip::tcp::socket& socket, const std::string& out);

	typedef std::map<std::string, Handler> HandlerMap;
	HandlerMap handlers;

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;

	// Enough room for a big ol request hopefully!
	char sock_buf[1024];
};