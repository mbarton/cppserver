#include <string>

class Handler
{
public:
	virtual std::string handleRequest() = 0;
};