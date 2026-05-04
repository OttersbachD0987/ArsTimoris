// CSC 134
// M3LAB1
// Daley Ottersbach
// 3/6/2026

//#define __STDC_WANT_LIB_EXT1__ 1
//#include <cstring>
#ifdef _WIN32_
#define _WIN32_WINDOWS
#endif
#include <print>
#include <iostream>
#include <boost/asio.hpp>


int main(int argc, char** argv) {
    boost::asio::io_context ioContext;
    bool running = true;
    std::string data = "";
    while (running) {
        std::getline(std::cin, data);
        boost::asio::ip::tcp::socket socket = boost::asio::ip::tcp::socket(ioContext);
        boost::asio::ip::tcp::resolver resolver = boost::asio::ip::tcp::resolver(ioContext);
        boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));
        std::println("> Connected Console");
        std::println("Sending '{}' to server...", data);
        boost::asio::write(socket, boost::asio::buffer(data));
        boost::system::error_code error;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error);
        socket.close();
    }

    return 0;
}