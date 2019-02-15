#include "network/ServerTCP.hpp"
#include "network/ClientTCP.hpp"

void test(std::string s) {
	std::cout << s << std::endl;
}

int main() {
	IOManager ioManager;
	KNW::ServerTCP::b_sptr server = nullptr;
	KNW::ClientTCP::b_sptr client = nullptr;
	std::string buffer;
	bool b = true;
	while (true) {
		std::getline(std::cin, buffer);
		if (buffer == "exit")
			return (0);
		else if (buffer == "s") {
			try {

				server = KNW::ServerTCP::create(ioManager);
				server->startServer("8000");
				server->getDataTCP().addDataType<std::string>(&test);
				server->startAsyncAccept();
				sleep(1);
				std::cout << "Server finsih count : " << server.use_count() << std::endl;

			} catch (const std::exception &e) {
				std::cout << e.what() << std::endl;
				server = nullptr;
			} catch (const boost::system::system_error &ex) {
				std::cout << ex.what() << std::endl;

			} catch (const boost::exception &e) {
				std::cout << diagnostic_information(e) << std::endl;
				server = nullptr;
			}
		} else if (buffer == "d") {
			std::cout << server.use_count() << "  " << client.use_count()
					  << std::endl;
			server = nullptr;
			client = nullptr;
		} else if (buffer == "c") {

			std::cout << "KNW::ClientTCP::create(ioManager): " << server.use_count() << std::endl;
			client = KNW::ClientTCP::create(ioManager);

			std::cout << "getDataTCP_().addDataType : "<< server.use_count() << std::endl;
			client->getDataTCP_().addDataType<std::string>(&test);

			std::cout << "connect : "<< server.use_count() << std::endl;
			client->connect("127.0.0.1", 8000);
			sleep(1);

			std::cout << "client finsih count : "<< server.use_count() << std::endl;
		} else if (buffer == "i") {
			std::cout << server.use_count() << "  " << client.use_count()
					  << std::endl;
		} else if (buffer == "hardtest") {
			while (true) {
				try {
					server = KNW::ServerTCP::create(ioManager);
					server->startServer("8000");
					server->startAsyncAccept();
					client = KNW::ClientTCP::create(ioManager);
					client->connect("127.0.0.1", 8000);
				} catch (const boost::system::system_error &error) {
					std::cout << error.what() << std::endl;
					server = nullptr;
					client = nullptr;
					continue;
				} catch (const boost::exception &e) {
					std::cout << diagnostic_information(e) << std::endl;
					server = nullptr;
					client = nullptr;
					continue;
				} catch (const std::exception &e) {
					server = nullptr;
					client = nullptr;
					continue;
				}
				std::cout << server.use_count() << "  " << client.use_count()
						  << std::endl;
				server = nullptr;
				client = nullptr;
			}
		} else {
			if (b)
				server->writeDataToOpenConnections(buffer);
			else
				client->writeDataToServer(buffer);
			b = !b;
		}
	}

}