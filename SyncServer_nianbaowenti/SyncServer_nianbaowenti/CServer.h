#include <boost/asio.hpp>
#include "CSession.h"
#include <memory.h>
#include <map>
using namespace std;
using boost::asio::ip::tcp;
class CServer
{
public:
    CServer(boost::asio::io_context& io_context, short port);
    void ClearSession(std::string uuid);
private:
    void HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error);
    void StartAccept();
    boost::asio::io_context& _io_context;
    short _port;
    tcp::acceptor _acceptor;
    std::map<std::string, shared_ptr<CSession>> _sessions;
};

