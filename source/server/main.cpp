//Тестовое задание.
//Задача должна быть решена на языке C++ для ОС Linux.
//При разработке использовать только стандартную библиотеку C++, boost и POSIX API.

//Исходные данные: файл формата CSV с тремя колонками: дата/время в формате «dd.mm.yyyy hh:mm:ss»
//и две колонки с дробными числами; в качестве десятичного разделителя точка.
//Все три колонки обязательные.

//Решение состоит из серверной и клиентской частей, запускаемых на одной машине.
//Сервер работает постоянно, клиент запускается для обработки одного файла.
//Имя файла передается клиенту в командной строке.
//Клиент соединяется с сервером по протоколу TCP и передает считанные из файла данные.
//В случае успешного завершения получает от сервера количество записей исходного файла и сохраняет
//в файл протокола.

//Допустим запуск одновременно нескольких экземпляров клиентов с разными файлами. Сервер может принимать
//несколько клиенских соединений одновременно.
//Для каждого соединения находит запись с максимальной датой и в протокол пишет соответствующую дату
//и частное двух числовых полей. Подсчитывает количество записей и возвращает его клиенту.

//Сервер использует тот же файл протокола, что и клиенты.
//В случае возникновения ошибок протокол должен содержать достаточную для понимания
//причины информацию об ошибках
//как на стороне клиента, так и сервера.


#include <chrono>
#include <fstream>
#include <mutex>
#include <signal.h>
#include <iostream>
#include <iomanip>

#include <boost/asio.hpp>
#include <boost/log/sinks/syslog_backend.hpp>


using std::cerr;
using std::cout;
using std::endl;


// LOGGER -----
#include <boost/log/common.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sources/logger.hpp>


boost::log::sources::logger createLogger() {
    auto syslog_backend = boost::make_shared<boost::log::sinks::syslog_backend>(
        boost::log::keywords::use_impl = boost::log::sinks::syslog::native
    );
    auto sink = boost::make_shared<boost::log::sinks::synchronous_sink< boost::log::sinks::syslog_backend> >(syslog_backend);

    boost::log::core::get()->add_sink(sink);
    boost::log::sources::logger logger;

    return logger;
}


auto logger = createLogger();
// LOGGER -----



bool ignore(std::istream &input, char const &symbol) {
    if (input.peek() == ' ')
        input.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    if (input.peek() == symbol)
        return static_cast<bool>(input.ignore(std::numeric_limits<std::streamsize>::max(), symbol));
    if (input.peek() == ' ')
        input.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    return true;
}


struct TCSVLine {
    std::chrono::system_clock::time_point tp = {};
    double value1 = {};
    double value2 = {};

    friend std::istream &operator >> (std::istream &input_, TCSVLine &csv_line) {
        std::string line;
        std::getline(input_, line);
        std::stringstream input(line);
        std::tm tm_ = {};
        bool is_ok =
            input >> std::get_time(&tm_, "%d.%m.%Y %H:%M:%S") && ignore(input, ';') &&
            input >> csv_line.value1 && ignore(input, ';') &&
            input >> csv_line.value2 && ignore(input, ';');// && ignore(input, '\n');
        if (is_ok) {
            csv_line.tp = std::chrono::system_clock::from_time_t(std::mktime(&tm_) - timezone);
            return input_;
        } else
            throw std::runtime_error("csv line parsing error: line '" + line + "'");
    }
};


std::string convert(std::chrono::system_clock::time_point const &tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm *tm_ = std::gmtime(&t);
    char time_buffer[32];
    std::strftime(time_buffer, 32, "%d.%m.%Y %H:%M:%S", tm_);
    return time_buffer;
}


class Session {
public:
    Session(boost::asio::ip::tcp::socket socket)
    :
        m_socket(std::move(socket))
    {
        BOOST_LOG(logger) << "server " << m_socket.remote_endpoint() << ": session open" << endl;
    }
   ~Session() {
        BOOST_LOG(logger) << "server " << m_socket.remote_endpoint() << ": session close" << endl;
    }

    void run() {
        std::string buffer;
        TCSVLine csv_line       = {};
        TCSVLine last_csv_line  = {};
        size_t   count          = {};

        // request - respoonse
        try {
            bool is_continue_read = true;
            // read all lines
            while (is_continue_read) {
                boost::asio::streambuf stream_buffer;
                boost::asio::read_until(m_socket, stream_buffer, "\n");

                buffer = boost::asio::buffer_cast<char const *>(stream_buffer.data());
                std::stringstream ss;
                ss << buffer;

                // end of file marker "\n\n"
                is_continue_read = !(buffer.size() > 1 && buffer.substr(buffer.size() - 2) == "\n\n");

                // parsing
                try {
                    while (ss.good() && ss >> csv_line) {
                        count++;
                        if (last_csv_line.tp < csv_line.tp)
                            last_csv_line = csv_line;
                        // read "\n\n" if exists
                        ignore(ss, '\n');
                        ignore(ss, '\n');
                    }

                    if (!is_continue_read) {
                        // print result and send response OK, finish session
                        BOOST_LOG(logger) << "server " << m_socket.remote_endpoint() << ": "
                             << convert(last_csv_line.tp)
                             << "; quotient: " << last_csv_line.value1 / last_csv_line.value2
                             << "; count: " << count << endl;

                        static std::string const ok = "OK\n";
                        boost::asio::write(m_socket, boost::asio::buffer(ok.c_str(), ok.size()));
                    }

                // parsing error
                } catch (std::exception const &e) {
                    is_continue_read = false;
                    BOOST_LOG(logger) << "server " << m_socket.remote_endpoint() << ": "
                         << e.what()  << endl
                         << "buffer:" << endl
                         << buffer << endl;
                    std::string what("ERROR: ");
                    what += e.what();
                    what += "\n";
                    // send response with error
                    boost::asio::write(m_socket, boost::asio::buffer(what.c_str(), what.size()));
                }
            }

        // request - response error
        } catch (std::exception const &e) {
            BOOST_LOG(logger) << "server " << m_socket.remote_endpoint() << ": "
                 << e.what()  << endl
                 << "buffer:" << endl
                 << buffer << endl;
        }
    }
private:
    boost::asio::ip::tcp::socket m_socket;
};


class Server {
public:
    Server(std::string const &url)
    :
        m_address   (url.substr(0, url.find(':'))),
        m_port      (std::atoi(url.substr(m_address.size()).c_str())),
        m_socket    (m_io_service)
    {
        if (m_port == 0)
            m_port =  8080;
    }


    void initialize() {
        using namespace boost::asio::ip;
        m_acceptor = std::make_shared<tcp::acceptor>(
            m_io_service, tcp::endpoint(address::from_string(m_address), m_port));

        accept();
        BOOST_LOG(logger) << "server " << m_address << ":" << m_port << ", server start ..." << endl;
        m_io_service_thread = std::make_unique<std::thread>(
            [&] () {
                m_is_running = true;
                m_io_service.run();
            }
        );
        BOOST_LOG(logger) << "server " << m_address << ":" << m_port << ", server start OK" << endl;
    }


    void finalize() {
        BOOST_LOG(logger) << "server " << m_address << ":" << m_port << ", server stop ..." << endl;
        m_is_running = false;
        m_io_service.stop();
        m_io_service_thread->join();
        BOOST_LOG(logger) << "server " << m_address << ":" << m_port << ", server stop OK" << endl;
    }

private:
    void accept() {
        using namespace boost::asio::ip;
        m_acceptor->async_accept(
            [&] (boost::system::error_code error_code, tcp::socket socket) {
                if (error_code)
                    cerr << boost::system::system_error(error_code).what() << endl;
                else
                    std::make_shared<Session>(std::move(socket))->run();

                if (m_is_running)
                    accept();
            }
        );
    }

    std::string                     m_address;
    uint16_t                        m_port;
    boost::asio::io_service         m_io_service;
    boost::asio::ip::tcp::socket    m_socket;

    std::shared_ptr<boost::asio::ip::tcp::acceptor>
                                    m_acceptor;

    std::unique_ptr<std::thread>    m_io_service_thread;
    std::atomic<bool>               m_is_running;
};


static std::mutex m;


void handleSignal(int signal) {
    cout << endl;
    m.unlock();
}


int main(int argc, char *argv[]) {
    std::string url;
    try {
        struct sigaction sigsignal_action = {};

        sigsignal_action.sa_handler = handleSignal;
        sigsignal_action.sa_flags   = 0;
        sigemptyset(&sigsignal_action.sa_mask);
        sigaction(SIGINT, &sigsignal_action, NULL);
        m.lock();

        if (argc == 2) {
            url = argv[1];
        } else {
            cout << argv[0] << " ip[:port], default port is 8080" << endl;
            return 1;
        }

        Server server(url);

        server.initialize();
        m.lock();
        server.finalize();

        return 0;
    } catch (std::exception const &e) {
        cerr << e.what() << endl;
        BOOST_LOG(logger) << argv[0] << " " << url << ": " << e.what();
        return 1;
    }
}
