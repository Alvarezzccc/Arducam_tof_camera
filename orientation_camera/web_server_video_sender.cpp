#include <opencv2/opencv.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

http::response<http::dynamic_body> make_response(cv::Mat image) {
    std::vector<uchar> buffer;
    cv::imencode(".jpg", image, buffer);
    auto const body = boost::beast::make_printable(buffer.data());

    http::response<http::dynamic_body> res{http::status::ok, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "image/jpeg");
    res.body() = body;
    res.prepare_payload();

    return res;
}

void do_session(tcp::socket& socket) {
    bool close = false;
    boost::beast::error_code ec;

    for (;;) {
        http::request<http::dynamic_body> req;
        http::read(socket, buffer, req, ec);

        if (ec == http::error::end_of_stream)
            break;
        if (ec)
            return fail(ec, "read");

        cv::Mat image = capture_image();  // Aquí debes implementar la captura de la imagen
        auto res = make_response(image);

        http::write(socket, res, ec);

        if (ec)
            return fail(ec, "write");
    }

    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(std::atoi("8080"));

    boost::asio::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    for (;;) {
        tcp::socket socket{ioc};
        acceptor.accept(socket);
        std::thread{std::bind(&do_session, std::move(socket))}.detach();
    }

    return EXIT_SUCCESS;
}