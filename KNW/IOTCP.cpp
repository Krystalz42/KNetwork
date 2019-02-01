#include <utility>

#include "IOTCP.hpp"
#include "ServerTCP.hpp"
#include "DataTCP.hpp"


KNW::IOTCP::IOTCP(
		KNW::DataTCP &dataTCP,
		tcp::socket socket,
		std::function<void(BaseDataType::Header, char *)> f)
		:
		callback_(std::move(f)),
		socket_(std::move(socket)),
		dataTCP_(dataTCP) {


}

void KNW::IOTCP::readSocketHeader() {
	boost::asio::async_read(
			socket_,
			boost::asio::buffer(buffer_data_, sizeof(BaseDataType::Header)),
			boost::bind(
					&KNW::IOTCP::handleReadHeader,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
			));

}

void KNW::IOTCP::handleReadHeader(const boost::system::error_code &ec,
									 size_t len) {

	if (ec.value() == 0) {
		BaseDataType::Header header;
		std::memcpy(&header, buffer_data_.data(), len);
		readSocketData(header);
	} else {
		std::cout << "handleReadHeader::error" << ec.message() << std::endl;
	}
}


void KNW::IOTCP::readSocketData(BaseDataType::Header header) {

	boost::asio::async_read(
			socket_,
			boost::asio::buffer(buffer_data_, dataTCP_.getSizeOfHeader(header)),
			boost::bind(
					&KNW::IOTCP::handleReadData,
					this,
					header,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
			));
}


void KNW::IOTCP::handleReadData(BaseDataType::Header header,
								   const boost::system::error_code &ec,
								   size_t len) {
	if (ec.value() == 0) {
		callback_(header, buffer_data_.data());
	} else {
		std::cout << "handleReadData::error" << std::endl;
	}
	readSocketHeader();
}

void
KNW::IOTCP::handleWrite(const boost::system::error_code &ec, size_t len) {
	if (ec.value() != 0)
		std::cout << "error" << std::endl;
}

void KNW::IOTCP::writeSocket(std::string data) {
	boost::asio::async_write(socket_, boost::asio::buffer(data),
							 boost::bind(&KNW::IOTCP::handleWrite,
										 this,
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred));
}

