#include <utility>

#include "ServerTCP.hpp"

namespace KNW {

	/** DataTCP **/

	DataTCP::DataTCP() = default;

	size_t DataTCP::getSizeOfHeader(DataTCP::Header header) {
		return sizeType[header];
	}

	void DataTCP::sendDataToCallback(DataTCP::Header header, void *data) {
		(*callbackType[header])(data);
	}

	DataTCP::~DataTCP() = default;


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

	void ServerTCP::asyncAccept() {
		acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
			connections.push_back(
					std::make_shared<ConnectionTCP>(*this, std::move(socket)));
		});
	}

	ServerTCP::~ServerTCP() = default;

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

	void ConnectionTCP::sendData(std::string data) {
		iotcp->writeSocket(std::move(data));
	}

	ConnectionTCP::~ConnectionTCP() {
		std::cout << "~ConnectionTCP" << std::endl;
	}

	void ClientTCP::connect(std::string dns, std::string port) {
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

}