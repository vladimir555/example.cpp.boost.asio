//#include <iostream>


//using namespace std;


////Тестовое задание.
////Задача должна быть решена на языке C++ для ОС Linux.
////При разработке использовать только стандартную библиотеку C++, boost и POSIX API.

////Исходные данные: файл формата CSV с тремя колонками: дата/время в формате «dd.mm.yyyy hh:mm:ss»
////и две колонки с дробными числами; в качестве десятичного разделителя точка.
////Все три колонки обязательные.

////Решение состоит из серверной и клиентской частей, запускаемых на одной машине.
////Сервер работает постоянно, клиент запускается для обработки одного файла.
////Имя файла передается клиенту в командной строке.
////Клиент соединяется с сервером по протоколу TCP и передает считанные из файла данные.
////В случае успешного завершения получает от сервера количество записей исходного файла и сохраняет
////в файл протокола.

////Допустим запуск одновременно нескольких экземпляров клиентов с разными файлами. Сервер может принимать
////несколько клиенских соединений одновременно.
////Для каждого соединения находит запись с максимальной датой и в протокол пишет соответствующую дату
////и частное двух числовых полей. Подсчитывает количество записей и возвращает его клиенту.

////Сервер использует тот же файл протокола, что и клиенты.
////В случае возникновения ошибок протокол должен содержать достаточную для понимания причины информацию
////об ошибках
////как на стороне клиента, так и сервера.


//#include <chrono>
//#include <iostream>
//#include <fstream>
//#include <list>
//#include <string>
//#include <boost/lexical_cast.hpp>
//#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/qi_attr.hpp>
//#include <boost/iostreams/stream.hpp>


//namespace qi = boost::spirit::qi;


//struct TLine {
//    std::chrono::system_clock::time_point tp;
//    double value1;
//    double value2;
//};


//BOOST_FUSION_ADAPT_STRUCT(TLine, tp, value1, value2)
////BOOST_FUSION_ADAPT_STRUCT(TLine, (std::chrono::system_clock::time_point, tp), (double, value1), (double, value2))


//typedef std::list<TLine> TLines;


//template<typename TResult, typename TValue>
//TResult convert(TValue const &value);


//template<typename TResult, typename TValue, typename TFormat>
//TResult convert(TValue const &value, TFormat const &format);


//// default format: "%Y-%m-%d %H:%M:%S%z %Z"
//template<>
//std::chrono::system_clock::time_point convert(std::string const &source, std::string const &format) {
//    std::tm tm_ = {};
//    std::istringstream ss(source);
//    ss >> std::get_time(&tm_, format.c_str());
//    return std::chrono::system_clock::from_time_t(mktime(&tm_));
//}


//template<>
//std::chrono::system_clock::time_point convert(std::string const &source) {
//    return convert<std::chrono::system_clock::time_point>(source, std::string("%Y-%m-%d %H:%M:%S%z %Z"));
//}


////namespace boost::spirit::traits {


////template<>
////void assign_to(std::string const &value, std::chrono::system_clock::time_point& attribute) {
////    cout << "assign_to: " << value << endl;
////    attribute = convert<std::chrono::system_clock::time_point>(value);
////}


////template <typename TIterator>
////struct assign_to_attribute_from_iterators<std::chrono::system_clock::time_point, TIterator, void> {
////    static inline void call(TIterator first, TIterator last, std::chrono::system_clock::time_point& attribute) {
////        cout << "assign_to_attribute_from_iterators: " << std::string(first, last) << endl;
////        attribute = convert<std::chrono::system_clock::time_point>(std::string(first, last));
////    }
////};


////template<>
////struct is_container<std::chrono::system_clock::time_point>: mpl::false_ {};


////template<>
////struct assign_to_attribute_from_value<std::chrono::system_clock::time_point, std::string> {
////    static void call(std::string const &value, std::chrono::system_clock::time_point &attribute) {
////        cout << "assign_to_attribute_from_value: " << value << endl;
////        attribute = convert<std::chrono::system_clock::time_point>(value);
////    }
////};


////}; // boost::spirit::traits


//template<>
//TLine convert(std::string const &line) {
//    TLine result = {};
//    qi::rule<std::string::const_iterator, std::chrono::system_clock::time_point> time;
////        qi::uint_ - '.' >> qi::uint_ - '.' >> qi::uint_ - ' ' >> qi::uint_ - ':' >> qi::uint_ - ':' >> qi::uint_;
//    qi::rule<std::string::const_iterator, TLine()> rule =
//        time - ';' >> qi::double_ - ';' >> qi::double_ - ';';

////    TCSVGrammar<std::string::const_iterator> grammar;
////    boost::spirit::qi::phrase_parse(line.begin(), line.end(), rule, result);

//    auto first = line.begin();
//    auto last  = line.end();
////    auto r = boost::spirit::qi::parse(first, last, rule, result);
//    auto r = boost::spirit::qi::phrase_parse(first, last, rule, qi::space, result);
//    cout << "parsed: " << line << ", r: " << r << ", result.value1: " << result.value1 << endl;
//    cout << string(first, last) << endl;
//    return result;
//}


//std::list<TLine> parseCSV(std::istream &source_stream) {
//    try {
//        std::list<TLine> result;

//        std::string line;
//        while (getline(source_stream, line) && !line.empty())
//            result.push_back(convert<TLine>(line));

//        return result; // ----->
//    } catch (std::exception const &e) {
//        throw std::runtime_error("parsing error: " + std::string(e.what())); // ----->
//    }
//}


////std::list<TLine> parseCSV(std::istream &source_stream) {
////    try {
////        TLines result;

//////        qi::rule<std::string::const_iterator, TCSVGrammar<std::list<TLine> > > csv_parser;
////        TCSVGrammar<std::string::const_iterator> grammar;
//////        qi::rule<std::string::const_iterator, TCVSGrammar()> rule;
////        boost::spirit::qi::phrase_parse(line.begin(), line.end(), grammar, result);

////        return result; // ----->
////    } catch (std::exception const &e) {
////        throw std::runtime_error("parsing error: " + std::string(e.what())); // ----->
////    }
////}


//int main() {
////    ifstream f("test.csv");
////    auto result = parseCSV(f);
//    auto l = convert<TLine>(string("01.01.2023 01:02:03"));
////    auto l = convert<TLine>(string("01.01.2023 01:02:03; 0.111;0.2222"));
//    return 0;
//}


//template<typename TResult, typename TValue>
//TResult convert(TValue const &value);


//template<typename TResult, typename TValue, typename TFormat>
//TResult convert(TValue const &value, TFormat const &format);


//void trim(string_view &sv) {
//    static std::string const whitespaces(" \t\f\v\n\r");
//    sv.remove_prefix(std::min(sv.find_first_not_of(whitespaces), sv.size()));
//    sv.remove_suffix(sv.size() - sv.find_last_not_of (whitespaces) - 1);
//}


//// default format: "%Y-%m-%d %H:%M:%S%z %Z"
//template<>
//std::chrono::system_clock::time_point convert(std::string const &source, std::string const &format) {
//    cout << source << endl;
//    string_view sv(source);
//    trim(sv);
//    std::tm tm_ = {};
//    std::stringstream ss;
//    ss << sv;
//    ss >> std::get_time(&tm_, format.c_str());
//    cout << tm_.tm_hour << " " << tm_.tm_min << " " << tm_.tm_sec << endl;
//    auto t = std::mktime(&tm_);
//    if (t == -1)
//        throw std::runtime_error("convertion to time error: " + std::string(std::strerror(errno)));
//    return std::chrono::system_clock::from_time_t(t);
//}


//template<>
//std::chrono::system_clock::time_point convert(std::string const &source) {
//    return convert<std::chrono::system_clock::time_point>(source, std::string("%Y.%m.%d %H:%M:%S%z %Z"));
//}


//template<>
//string convert(std::chrono::system_clock::time_point const &value) {
//    static string const time_format_unix               = "%Y-%m-%d %H:%M:%S";
//    static size_t const time_to_string_buffer_size     = 64;

//    auto  value_ms  = duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
//    uint32_t    ms  = value_ms % 1000;
//    time_t      t   = std::chrono::system_clock::to_time_t(value);
//    struct tm   tm_ = {};

//    ::gmtime_r(&t, &tm_);

//    char buffer[time_to_string_buffer_size];
//    strftime(buffer, time_to_string_buffer_size, time_format_unix.c_str(), &tm_);
//    return string(buffer); // ----->
//}


//template<>
//double convert(std::string const &source) {
//    string_view sv(source);
//    trim(sv);
//    return boost::lexical_cast<double>(sv);
//}


//template<>
//std::string convert(size_t const &source) {
//    return boost::lexical_cast<string>(source);
//}


//template<>
//TLine convert(std::string const &line) {
//    TLine result = {};
//    size_t first = 0, last = 0;
//    std::vector<std::string> columns;

//    while (last < line.size()) {
//        if (line[last] == ';') {
//            columns.push_back(line.substr(first, last - first));
//            first = ++last;
//        }
//        last++;
//    }

//    try {
//        if (columns.size() == 3)
//            return {
//                .tp     = convert<std::chrono::system_clock::time_point>(columns[0]),
//                .value1 = convert<double>(columns[1]),
//                .value2 = convert<double>(columns[2])
//            }; // ----->
//        else
//            throw std::runtime_error("wrong columns count " + convert<std::string>(columns.size())); // ----->
//    } catch (std::exception const &e) {
//        throw std::runtime_error("csv parsing error: line '" + line + "', " + e.what()); // ----->
//    }
//}


//std::list<TLine> parseCSV(std::istream &source_stream) {
//    try {
//        std::list<TLine> result;

//        std::string line;
//        while (getline(source_stream, line) && !line.empty())
//            result.push_back(convert<TLine>(line));

//        return result; // ----->
//    } catch (std::exception const &e) {
//        throw std::runtime_error("parsing error: " + std::string(e.what())); // ----->
//    }
//}


//#include <boost/spirit/include/qi.hpp>
//BOOST_FUSION_ADAPT_STRUCT(TLine, tp, value1, value2)
//TLine parseLine(std::string const &line) {
//    using namespace boost::spirit::qi;
//    TLine result;
//    tm tm_ = { 0 };

//    auto first = line.begin(), last = line.end();
////    auto parse_result = phrase_parse(first, last,
////        (uint_(tm_.tm_year) - '.') >> (uint_(tm_.tm_mon) - '.') >> (uint_(tm_.tm_mday) - ' ') >>
////        (uint_(tm_.tm_hour) - ':') >> (uint_(tm_.tm_min) - ':') >> (uint_(tm_.tm_sec)  - ';') >>
////        (double_(result.value1) - ';') >>
////        (double_(result.value2) - ';'),
////        space
////    );
//    auto parse_result = phrase_parse(first, last,
//        uint_(tm_.tm_year) >> ';',
//        space
//    );

//    cout << "line: " << std::string(first, last) << endl;
//    cout << "tm h: " << tm_.tm_year << endl;
//    cout << "result: " << parse_result << endl;

//    result.tp = std::chrono::system_clock::from_time_t(std::mktime(&tm_));

//    return result;
//}

//#include <boost/chrono.hpp>
