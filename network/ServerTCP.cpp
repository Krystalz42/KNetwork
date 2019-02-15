#include <utility>

#include <utility>

#include <utility>

#include "ServerTCP.hpp"
namespace KNW {
	/** Connection TCP **/

	ConnectionTCP::ConnectionTCP() = default;

	ConnectionTCP::boost_shared_ptr ConnectionTCP::create(
			ServerTCP::b_wptr serverTCP,
			boost::shared_ptr<boost::asio::ip::tcp::socket> sock) {
		auto ptr = boost::shared_ptr<ConnectionTCP>(new ConnectionTCP());
		auto sptr = serverTCP.lock();
		assert(sptr->dataTCP != nullptr);
		ptr->iotcp = IOTCP::create(
				sptr->dataTCP->weak_from_this(),
				std::move(sock),
				boost::bind(&ConnectionTCP::callbackDeadIO, ptr));
		ptr->serverTCP_ = serverTCP;
		return ptr;
	}

	void ConnectionTCP::callbackDeadIO() {
		auto sptr = serverTCP_.lock();
		if (sptr) {
			auto it = std::find(sptr->connections.begin(), sptr->connections.end(), shared_from_this());
			boost::system::error_code ec;
			iotcp->getSocket_().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			iotcp->getSocket_().close(ec);
			*it = nullptr;
		}
	}

	boost::asio::ip::tcp::socket &ConnectionTCP::getSocket_() {
		return iotcp->getSocket_();
	}

	void ConnectionTCP::write(std::string data) {
		iotcp->writeSocket(std::move(data));
	}

	/** Server TCP **/

	ServerTCP::b_sptr ServerTCP::create(IOManager &io_manager) {
		return boost::shared_ptr<ServerTCP>(new ServerTCP(io_manager));
	}

	ServerTCP::ServerTCP(IOManager &io_manager)
			: io_manager_(io_manager),
			dataTCP(DataTCP::create()),
			acceptor_(new boost::asio::ip::tcp::acceptor(io_manager_.getIo())) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	void ServerTCP::startServer(std::string port) noexcept {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		boost::asio::ip::tcp::resolver resolver(io_manager_.getIo());
		boost::asio::ip::tcp::resolver::query query("127.0.0.1",
													boost::lexical_cast<std::string>(
															port));
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
		acceptor_->open(endpoint.protocol());
		acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_->bind(endpoint);
		acceptor_->listen(boost::asio::socket_base::max_connections);
	}

	void ServerTCP::startAsyncAccept() noexcept {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		ServerTCP::b_wptr wptr(shared_from_this());
		io_manager_.getThreadGroup().create_thread([wptr](){ auto sptr = wptr.lock(); if (sptr) sptr->acceptConnection(); });
	}

	void ServerTCP::startAsyncAccept(std::function<void(size_t)> c) noexcept {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		callbackAccept_ = std::move(c);
		startAsyncAccept();
	}

	void ServerTCP::startAsyncAccept(
			std::function<void(size_t)> a,
			std::function<void(size_t)> d) noexcept {
		callbackAccept_ = std::move(a);
		callbackDeadConnection_ = std::move(d);
		startAsyncAccept();
	}

	void ServerTCP::acceptConnection() noexcept {
		std::cout << __PRETTY_FUNCTION__ << acceptor_->is_open() << std::endl;
		if (acceptor_->is_open()) {
			boost::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(io_manager_.getIo()));

			ServerTCP::b_wptr wptr(weak_from_this());
			acceptor_->async_accept(*sock, [wptr, sock](const boost::system::error_code &ec){auto sptr = wptr.lock(); if (sptr) sptr->handleAcceptConnection(ec, sock); });
		}
	}

	DataTCP &ServerTCP::getDataTCP(){
		return *dataTCP;
	}

	void ServerTCP::handleAcceptConnection(
			const boost::system::error_code &ec,
			boost::shared_ptr<boost::asio::ip::tcp::socket> sock
	) {

		if (ec) {
			std::cout << "Fail ! " << ec.message() << std::endl;
		} else {
			std::cout << "GG" << std::endl;
			acceptConnection();
			auto it = std::find_if(connections.begin(), connections.end(),
								   [](boost::shared_ptr<ConnectionTCP> connection) {
									   return connection == nullptr ||
											  !connection->getSocket_().is_open();
								   });
			if (it != connections.end()) {
				*it = ConnectionTCP::create(shared_from_this(),
											std::move(sock));
				if (callbackAccept_)
					callbackAccept_(static_cast<size_t>(
							std::distance(connections.begin(),
							std::find(connections.begin(), connections.end(), *it))));
			}

		}
	}

	ServerTCP::~ServerTCP() {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		boost::system::error_code ec;

		acceptor_->cancel(ec);
		if (ec)
			std::cout << ec.message() << std::endl;
		acceptor_->close(ec);
		if (ec)
			std::cout << ec.message() << std::endl;
		for (auto &connection : connections) {
			if (connection) {
				boost::system::error_code ec_sock;
				connection->getSocket_().shutdown(
						boost::asio::ip::tcp::socket::shutdown_both, ec_sock);
				connection->getSocket_().close(ec_sock);
			}
		}

	}
}
