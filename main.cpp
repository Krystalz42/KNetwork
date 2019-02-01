
#include <iostream>
#include "KNW/ServerTCP.hpp"

struct Int {
	Int(): id(42){};
	int id;
};
struct Char {
	Char(): c('a'){};
	char c;
};
struct Buffer {
	Buffer() : buffer("salut") {};
	char buffer[256];
};

struct Position {
	Position() : x(4), y(2) {};
	int x, y;
};

void intorino(Int id) {

	std::cout << id.id << std::endl;
}
void bufferino(Buffer buff) {
	std::cout << buff.buffer << std::endl;

}
void positionino(Position pos) {
	std::cout << "x : " <<  pos.x << " y : " << pos.y << std::endl;
}

void charino(char pos) {
	std::cout << pos << std::endl;
}

int main () {

	KNW::ServerTCP serverTCP(8000);
	KNW::ClientTCP clientTCP;

	std::function<void(char)> fa = std::bind(charino, std::placeholders::_1);
	std::function<void(Int)> fi = std::bind(intorino, std::placeholders::_1);
	std::function<void(Position)> fp = std::bind(positionino, std::placeholders::_1);

	serverTCP.addDataType<char>(fa);
	serverTCP.addDataType<Int>(fi);
	serverTCP.addDataType<Position>(fp);

	clientTCP.addDataType<char>(fa);
	clientTCP.addDataType<Int>(fi);
	clientTCP.addDataType<Position>(fp);

	std::string s;

	while (1) {
		std::getline(std::cin, s);
		if (s == "a")
			serverTCP.accept();
		if (s == "connect")
			clientTCP.connect("localhost", "8000");


		if (s == "wsc")
			serverTCP.writeDataToOpenConnection<char>('a');
		if (s == "wcc")
			clientTCP.writeDataToServer<char>('a');

		if (s == "wsi")
			serverTCP.writeDataToOpenConnection<Int>(Int());
		if (s == "wci")
			clientTCP.writeDataToServer<Int>(Int());

		if (s == "wsp")
			serverTCP.writeDataToOpenConnection<Position>(Position());
		if (s == "wcp")
			clientTCP.writeDataToServer<Position>(Position());


		if (s == "exit")
			return (0);
	}

	std::cout << "Finish" << std::endl;
}
































