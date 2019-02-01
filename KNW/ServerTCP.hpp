#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/error.hpp>
#include <boost/array.hpp>
#include <boost/asio/detail/config.hpp>
#include <boost/cerrno.hpp>
#include <boost/thread.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <array>
#include "BaseDataType.hpp"
#include <bitset>
#include <unordered_map>
#include <iostream>
#include "Config.hpp"
#include "IOTCP.hpp"

using boost::asio::ip::tcp;

namespace KNW {

	class ServerTCP;
	class DataTCP;

	/*********************** ConnectionTCP ************************************/

	class ConnectionTCP {
	public:
		ConnectionTCP(ServerTCP &serverTCP, tcp::socket socket);
		void sendData(std::string data);
		~ConnectionTCP();
	private:
		friend class ServerTCP;
		std::unique_ptr<IOTCP> iotcp;
		ServerTCP &serverTCP_;
	};

	/*************************** ServerTCP ************************************/

	class ServerTCP {
	public:
		explicit ServerTCP(unsigned short port);

		void accept();

		template<typename T>
		void addDataType(std::function<void(T)> callback);

		template<typename T>
		void writeDataToOpenConnection(T data);

		virtual ~ServerTCP();

	private:

		void asyncAccept();

		//Network
		unsigned short port_;
		boost::asio::io_service io_service_;
		tcp::acceptor acceptor_;
		boost::thread thread;

		//connection
		std::vector<std::shared_ptr<ConnectionTCP>> connections;

		//Data management
		DataTCP dataTCP_;
		friend class ConnectionTCP;
		friend class IOTCP;

	};

	template<typename T>
	void ServerTCP::addDataType(std::function<void(T)> callback) {
		assert(!dataTCP_.hasType<T>());
		dataTCP_.addDataType<T>(callback);
	}

	template<typename T>
	void ServerTCP::writeDataToOpenConnection(T data) {
		auto header = DataType<T>::getHeader();
		assert(dataTCP_.hasType<T>());
		std::string buffer;
//		std::cout << "writeDataToOpenConnection:" << dataTCP_.getSizeOfType<T>() << std::endl;
		buffer.append(reinterpret_cast<char *>(&header), sizeof(BaseDataType::Header));
		buffer.append(reinterpret_cast<char *>(&data), dataTCP_.getSizeOfType<T>());
		for (auto &connection : connections) {
			connection->sendData(buffer);
		}
	}
	/*********************** ClientTCP ************************************/

	class ClientTCP {
	public:

		ClientTCP();

		void connect(std::string dns, std::string port);

		template<typename T>
		void addDataType(std::function<void(T)> callback);

		template<typename T>
		void writeDataToServer(T data);

	private:
		boost::asio::io_service io;
		tcp::socket socket_;
		tcp::resolver resolver;
		boost::thread thread;
		DataTCP dataTCP_;
		std::unique_ptr<IOTCP> iotcp;
	};

	template<typename T>
	void ClientTCP::addDataType(std::function<void(T)> callback) {
		assert(!dataTCP_.hasType<T>());
		dataTCP_.addDataType<T>(callback);
	}

	template<typename T>
	void ClientTCP::writeDataToServer(T data) {
		assert(dataTCP_.hasType<T>());
		auto header = DataType<T>::getHeader();
//		std::cout << "writeDataToServer:" << dataTCP_.getSizeOfType<T>() << std::endl;
		std::string buffer;
		buffer.append(reinterpret_cast<char *>(&header), sizeof(BaseDataType::Header));
		buffer.append(reinterpret_cast<char *>(&data), dataTCP_.getSizeOfType<T>());
		iotcp->writeSocket(buffer);
	}

}


