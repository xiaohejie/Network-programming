#include "CServer.h"

CServer::CServer(boost::asio::io_context& io_context, short port) :_io_context(io_context), _port(port),
_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{
	cout << "Server start success, on port: " << port << endl;
	StartAccept();
}

void CServer::ClearSession(std::string uuid)
{
	_sessions.erase(uuid);
}

void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error)
{
	if (!error) {
		new_session->Start();
		_sessions.insert(make_pair(new_session->GetUuid(), new_session));
	}
	else {
		//delete new_session;
	}
	StartAccept();
}

void CServer::StartAccept()
{
	shared_ptr<CSession> new_session = make_shared<CSession>(_io_context, this);
	_acceptor.async_accept(new_session->Socket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}
