#pragma once

#include <boost/system/error_code.hpp>
#include "IOTCP.hpp"
#include "DataTCP.hpp"
#include <boost/bind.hpp>
#include "BaseDataType.hpp"
#include <boost/asio.hpp>
#include <boost/array.hpp>


using boost::asio::ip::tcp;

namespace KNW {

	class ServerTCP;

	class IOTCP {
	public:
		IOTCP(
				DataTCP &dataTCP_,
				tcp::socket socket_,
				std::function<void(BaseDataType::Header, char *)> f
		);

		void writeSocket(std::string data);

		void readSocketHeader();

	private:

		void readSocketData(DataTCP::Header header);

		void handleReadHeader(const boost::system::error_code &, size_t);


		void handleReadData(DataTCP::Header header,
							const boost::system::error_code &, size_t);

		void handleWrite(const boost::system::error_code &, size_t);

		DataTCP &dataTCP_;
		boost::array<char, eConfig::kMaxBufferSize> buffer_data_;
		tcp::socket socket_;
		std::function<void(BaseDataType::Header, char *)> callback_;
	};
}


