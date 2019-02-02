#include <utility>

#include "ServerTCP.hpp"

namespace KNW {

	/** ServerTCP **/

	ServerTCP::ServerTCP(unsigned short port) :
			port_(port),
			acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)) {
	}

	void ServerTCP::accept() {
		asyncAccept();
		thread = boost::thread(
				boost::bind(&boost::asio::io_service::run, &io_service_));
		thread.detach();
	}


	void ServerTCP::accept(std::function<void(size_t)> callbackAccept) {
		callbackAccept_ = callbackAccept;
		asyncAccept();
		thread = boost::thread(
				boost::bind(&boost::asio::io_service::run, &io_service_));
		thread.detach();
	}


	void ServerTCP::asyncAccept() {
		acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
			std::shared_ptr<ConnectionTCP> connection
				= std::make_shared<ConnectionTCP>(*this, std::move(socket));
			connections.push_back(connection);
			std::cout << "New connection" << std::endl;
			if (callbackAccept_) {
				std::cout << "Callback" << std::endl;
				callbackAccept_(std::distance(connections.begin(), std::find(connections.begin(), connections.end(), connection)));
			}
			asyncAccept();
		});
	}

	ServerTCP::~ServerTCP() {
		connections.clear();
		io_service_.stop();
		acceptor_.cancel();
		acceptor_.close();
		thread.interrupt();
	}

	size_t ServerTCP::getSizeOfConnections() const {
		return connections.size();
	}

	/** ConnectionTCP **/

	ConnectionTCP::ConnectionTCP(ServerTCP &serverTCP, tcp::socket socket) :
			serverTCP_(serverTCP) {

		iotcp = std::make_unique<IOTCP>(
				serverTCP.dataTCP_,
				std::move(socket),
				std::bind(&DataTCP::sendDataToCallback,
						  std::ref(serverTCP.dataTCP_),
						  std::placeholders::_1,
						  std::placeholders::_2
				));

		iotcp->readSocketHeader();
	}

	bool ClientTCP::isConnect() const {
		return socket_.is_open();
	}

	void ConnectionTCP::sendData(std::string data) {
		iotcp->writeSocket(std::move(data));
	}

	ConnectionTCP::~ConnectionTCP() {
		std::cout << "~ConnectionTCP" << std::endl;
	}

	void ClientTCP::connect(std::string dns, std::string port) {
		std::cout << "socket : " << socket_.is_open() << std::endl;
		try {
			tcp::resolver::query query(dns, port);
			tcp::resolver::iterator it = resolver.resolve(query);
			boost::asio::connect(socket_, it);
			iotcp = std::make_unique<IOTCP>(
					dataTCP_,
					std::move(socket_),
					std::bind(&DataTCP::sendDataToCallback, std::ref(dataTCP_),
							  std::placeholders::_1, std::placeholders::_2));
			iotcp->readSocketHeader();
			thread = boost::thread(
					boost::bind(&boost::asio::io_service::run, &io));
			thread.detach();
		} catch (std::exception &exception) {
			std::cout << exception.what() << std::endl;
		}

	}

	ClientTCP::ClientTCP()
			: resolver(io),
			  socket_(io) {
		std::cout << "ClientTCP" << std::endl;
	}


	ClientTCP::~ClientTCP() {
		try {
			if (socket_.is_open()) {
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
				socket_.close();
			}
		} catch (std::exception const &e) {
			std::cout << e.what() << std::endl;
		}

		thread.interrupt();
		io.stop();
	}

}