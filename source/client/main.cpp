#include <iostream>


using namespace std;


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
//В случае возникновения ошибок протокол должен содержать достаточную для понимания причины информацию
//об ошибках
//как на стороне клиента, так и сервера.


#include <fstream>
#include <iostream>


#include <boost/asio.hpp>


class Client {
public:
    Client(std::string const &url)
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
        m_socket.connect(tcp::endpoint(address::from_string(m_address), m_port));
    }


    void finalize() {
        m_socket.close();
    }


    void write(istream &stream) {
        string line;
        while(std::getline(stream, line)) {
            line += "\n";
            m_socket.write_some(boost::asio::buffer(line));
        }
        // send end of file marker "\n\n"
        line = "\n";
        m_socket.write_some(boost::asio::buffer(line));

        boost::asio::streambuf      stream_buffer;
        boost::asio::read_until(m_socket, stream_buffer, "\n");

        std::string const buffer = boost::asio::buffer_cast<char const *>(stream_buffer.data());
        if (buffer != "OK\n")
            throw std::runtime_error(buffer);
    }
private:
    std::string                     m_address;
    uint16_t                        m_port;
    boost::asio::io_service         m_io_service;
    boost::asio::ip::tcp::socket    m_socket;
};


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


int main(int argc, char* argv[]) {
    string url;
    string csv_path;

    if (argc == 3) {
        url     = argv[1];
        csv_path   = argv[2];
    } else {
        cout << argv[0] << " ip[:port] csv_path, default port is 8080" << endl;
        return 1;
    }

    ifstream    f(csv_path);
    Client      client(url);
    bool        is_initialized = false;

    try {
        client.initialize();
        is_initialized = true;
        client.write(f);
    } catch (std::exception const &e) {
        BOOST_LOG(logger)  << argv[0] << " " << csv_path << ": " << e.what() << endl;
        return 1;
    }

    try {
        if (is_initialized)
            client.finalize();
    } catch (std::exception const &e) {
        BOOST_LOG(logger)  << argv[0] << " " << csv_path << ": " << e.what() << endl;
        return 1;
    }

    return 0;
}
