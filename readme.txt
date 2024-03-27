Тестовое задание.
Задача должна быть решена на языке C++ для ОС Linux.
При разработке использовать только стандартную библиотеку C++, boost и POSIX API.

Исходные данные: файл формата CSV с тремя колонками: дата/время в формате «dd.mm.yyyy hh:mm:ss»
и две колонки с дробными числами; в качестве десятичного разделителя точка.
Все три колонки обязательные.

Решение состоит из серверной и клиентской частей, запускаемых на одной машине.
Сервер работает постоянно, клиент запускается для обработки одного файла.
Имя файла передается клиенту в командной строке.
Клиент соединяется с сервером по протоколу TCP и передает считанные из файла данные.
В случае успешного завершения получает от сервера количество записей исходного файла и сохраняет
в файл протокола.

Допустим запуск одновременно нескольких экземпляров клиентов с разными файлами. Сервер может принимать
несколько клиенских соединений одновременно.
Для каждого соединения находит запись с максимальной датой и в протокол пишет соответствующую дату
и частное двух числовых полей. Подсчитывает количество записей и возвращает его клиенту.

Сервер использует тот же файл протокола, что и клиенты.
В случае возникновения ошибок протокол должен содержать достаточную для понимания причины информацию об ошибках как на стороне клиента, так и сервера.


Test.
The problem must be solved in C++ for Linux OS.
When developing, use only the standard C++ library, boost and POSIX API.

Input data: CSV file with three columns: date/time in the format “dd.mm.yyyy hh:mm:ss”
and two columns with fractions; dot as decimal separator.
All three columns are required.

The solution consists of server and client parts running on the same machine.
The server runs continuously, the client is launched to process one file.
The file name is passed to the client on the command line.
The client connects to the server via TCP and transmits the data read from the file.
If successful, receives the number of records in the source file from the server and saves
to the protocol file.

Let's allow several client instances to be launched simultaneously with different files. The server can accept
multiple client connections simultaneously.
For each connection, finds a record with the maximum date and writes the corresponding date to the protocol
and the quotient of two numeric fields. Counts the number of records and returns it to the client.

The server uses the same log file as the clients.
In case of errors, the protocol must contain sufficient information for understanding reasons error information both on the client and server side.
