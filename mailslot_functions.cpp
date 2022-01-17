#include "mailslot_functions.hpp"

#include <iostream>
#include <io.h>
#include <fcntl.h>


// Ошибка, возвращаемая GetLastError() в случае, если mailslot с указанным именем уже существует
const uint16_t MAILSLOT_ALREADY_EXISTS = 183;


// Получение название mailslot c ввода в консоль
void MailSlotHandler::readMailslotnameFromInput()
{
    // Буфер для чтения строки с консоли
    std::wstring mailslot = L"";

    std::wcout << L"Введите название почтового ящика (mailslot): ";

//     Пытаемся считывать название mailslot рекурсивно до тех пор пока не будет получена строка ненулевой длины
    do std::wcin >> mailslot; while(mailslot.length() == 0);

    // Cоздание пути до mailslot исходя из полученного названия
    mailslot = _path + mailslot;

    std::wcout << L"Ваш mailsot: " << L"\"" << mailslot << L"\"" << std::endl;

    // Сохранение название mailslot для дальнейшего использования и создания mailslot
    _mailslot_name = mailslot.c_str();

    // Создаём mailslot c полученным названием
    createMailSlot();
}


// Функция получения описателя (handle) на уже существующий mailslot
void MailSlotHandler::getHandle()
{
    // Открыть mailslot на клиенте
    _handle = CreateFile(_mailslot_name,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    // Если произошла ошибка при открытии mailslot на клиенте, вывести ошибку
    if (_handle == INVALID_HANDLE_VALUE)
        std::wcout << L"Ошибка при инициализации handle: " << GetLastError() << std::endl;
}


// Прочитать информацию о размере следующего сообщения в очереди mailslot и количестве сообщений в очереди
// Используется всякий раз при получении информации о mailslot с целью получения актуальной информации о текущем состоянии mailslot
void MailSlotHandler::readInformationAboutRecordsInMailslot()
{
    // Переменная для хранения статуса об ошибке во время выполнения GetMailslotInfo
    bool err;

    // Получить информацию о размере следующей записи (record) и количестве полученных сообщений в очереди
    err = GetMailslotInfo(_handle, &_max_msg_size, &_next_msg_size, &_msg_amount_in_queue, &_waiting_time);

    // Проверить наличие ошибок во время выполнения функции GetMailslotInfo
    if (!err)
        std::wcout << L"Ошибка во время выполнения GetMailslotInfo: "
                   << GetLastError()
                   << std::endl;
}


// Получить количество записей в очереди mailslot, прочитанное в функции readInformationAboutRecordsInMailslot
DWORD MailSlotHandler::getMsgAmountInQueue()
{
    readInformationAboutRecordsInMailslot();
    return _msg_amount_in_queue;
}


// Получить размер следующей запиcи, прочитанный в функции readInformationAboutRecordsInMailslot
DWORD MailSlotHandler::getNextMsgSize()
{
    readInformationAboutRecordsInMailslot();
    return _next_msg_size;
}


// Получить максимальный размер сообщения доступный для чтения из очереди mailslot
DWORD MailSlotHandler::getMaxMsgSize()
{
    readInformationAboutRecordsInMailslot();
    return _max_msg_size;
}


// Вывести следующее значение из очереди сообщений mailslot в консоль
void MailSlotHandler::printNextMsgSize()
{
    DWORD next_msg_size = getNextMsgSize();
    if( next_msg_size == (DWORD)MAILSLOT_NO_MESSAGE)
        std::wcout << L"В очереди mailslot нет сообщений для чтения." << std::endl;
    else
        std::wcout << L"Размер последнего сообщения в очереди mailslot: "
                   << next_msg_size
                   << std::endl;
}


// Вывeсти в количество сообщений в очереди в консоль
void MailSlotHandler::printMsgAmountInQueue()
{
    std::wcout << L"Количество сообщений в очереди доступных для чтения: "
               << getMsgAmountInQueue()
               << std::endl;
}


// Вывести в консоль максимально доступный размер сообщения, который можно считать с mailslot
void MailSlotHandler::printMaxMsgSize()
{
    std::wcout << L"Максимальный размер сообщения доступный для чтения: "
               << getMaxMsgSize()
               << std::endl;
}


// Вывести меню для сервера или клиента в соответствии
void MailSlotHandler::printActualMenu()
{
         if( _role == CLIENT )
    {
        std::wcout << L"\n"
                   << L"Какое действие необходимо выполнить? (введите номер 1-5):" << L"\n"
                   << L"1: вывести количество сообщений в очереди mailslot"        << L"\n"
                   << L"2: вывести размер последнего сообщения в очереди mailslot" << L"\n"
                   << L"3: наибольший допустимый размер для данного mailslot"      << L"\n"
                   << L"4: записать сообщение в очередь mailslot"                  << L"\n"
                   << L"5: закончить работу с программой"                          << L"\n"
                   << std::endl;
    }
    else if( _role == SERVER )
    {
         std::wcout << L"\n"
                   << L"Какое действие необходимо выполнить? (введите номер 1-5):" << L"\n"
                   << L"1: вывести количество сообщений в очереди mailslot"        << L"\n"
                   << L"2: вывести размер последнего сообщения в очереди mailslot" << L"\n"
                   << L"3: наибольший допустимый размер для данного mailslot"      << L"\n"
                   << L"4: вывести сообщение из очереди mailslot"                  << L"\n"
                   << L"5: закончить работу с программой"                          << L"\n"
                   << std::endl;
    }
}


// Получить роль текущего процесса (клиент или сервер)
ROLE MailSlotHandler::role()
{
    return _role;
}


// Вспомогательная функция для конвертирования wstring в string
void stringToWString(std::wstring &wide_string, const std::string &short_string)
{
    wide_string = std::wstring(short_string.begin(), short_string.end());
}


// Вывести в консоль следующее сообщение из очереди (вытаскиваются в порядке поступления)
void MailSlotHandler::printNextMsgInQueue()
{
    // Перед чтением сообщения из очерди получить информацию о длине сообщения
    readInformationAboutRecordsInMailslot();

    // выделение памяти под буфер для чтения следующего сообщения из сети
    char *buffer_for_read_record_from_queue = static_cast<char*>(GlobalAlloc(GMEM_FIXED, _next_msg_size));

    // Если во время выделения памяти под буфер произошла ошибка (получен нулевой указатель), вывести сообщение об ошибке в консоль
    if (!buffer_for_read_record_from_queue)
        std::wcout << L"Ошибка в выделении памяти под буфер для чтения сообщения из сети";
    else
    {
        // Переменная для хранения прочитанных байт из очереди mailslot
        DWORD num_read_bytes;

        // Прочитать следующую запись, сохранить статус об ошибке в переменную err, если есть ошибка
        bool err = ReadFile(_handle, buffer_for_read_record_from_queue, _next_msg_size, &num_read_bytes, 0);

        // Вывести сообщение об ошибке в консоль при чтении из очереди, если есть ошибка
        if (!err)
            std::wcout << L"Ошибка при чтении следующего сообщения из mailslot: " << GetLastError() << std::endl;

        // Убедиться, что все байты следующей записи были прочитаны, если нет вывести сообщение об ошибке в консоль
        else if (_next_msg_size != num_read_bytes)
            std::wcout << L"Сообщение не было прочитано целиком!" << std::endl;
        else
        {
            // Преобразовать полученное сообщение из очереди в wstring и сохранить для дальнейшего использования
            stringToWString(_read_next_msg, std::string(buffer_for_read_record_from_queue));
//            _read_next_msg = std::wstring(buffer_for_read_record_from_queue);
            std::wcout << _read_next_msg << std::endl;
            // Освободить память буфера, использованного для чтения следующего сообщения из очереди mailslot
            GlobalFree(buffer_for_read_record_from_queue);
        }
    }
}


// Записать сообщение в очередь mailslot
void MailSlotHandler::recordMsgToMailslot()
{
    // Временная переменная для сохранения сообщения прочитанного с консоли
    std::string read_msg_from_input("");

    std::wcout << L"Введите сообщение для записи в mailslot:" << std::endl;

    // Пытаемся считаеть сообщение с консоли до тех пор пока не будет получена строка ненулевой длины
    do getline(std::cin, read_msg_from_input); while(read_msg_from_input.length() == 0);
//    std::wcout << read_msg_from_input << std::endl;

    // Преобразование полученной из консоли строки в указатель на буфер памяти, используемый для записи сообщения в очередь mailslot
    const char* message_for_writing_to_mailslot = read_msg_from_input.c_str();

    // Переменная для хранения статуса ошибки, которая может произойти во время записи сообщения в очередь mailslot
    bool     err;

    // Переменная для получения количества записанных байт в очередь mailslot
    DWORD    amount_of_written_bytes;

    // Запись считанного сообщения с консоли в mailslot
    err = WriteFile( _handle,
                     message_for_writing_to_mailslot,
                     strlen(message_for_writing_to_mailslot),
                     &amount_of_written_bytes, 0);

    // Проверить, есть ли ошибки при записи в mailslot
    if (!err)
        std::wcout << L"Ошибка при записи: " << GetLastError() << std::endl;

    // Убедиться, что сообщение было записано целиком в mailslot
    else if (strlen(message_for_writing_to_mailslot) != amount_of_written_bytes)
        std::wcout << L"Сообщение не было целиком записано в mailslot" << std::endl;
}


// Создание mailslot и сохранение handle, связанного с созданным mailslot
void MailSlotHandler::createMailSlot()
{
    // Получение описателя (handle) на созданный mailslot
    _handle = CreateMailslot(_mailslot_name, _max_msg_size, _waiting_time, _security_attributes);
    if (_handle == INVALID_HANDLE_VALUE)
    {
        _role = CLIENT;
        std::wcout << L"\n"
                   << L"Программа работает в качестве клиента" << std::endl;

        // Получение кода ошибки, которая может возникнуть во время выполнения CreateMailslot
        uint16_t error_code = GetLastError();

        // В случае если mailslot уже существует получаем handle на данный mailslot
        if( error_code == MAILSLOT_ALREADY_EXISTS )
            getHandle();
        else
            // В случае наличии другой ошибки выводим информацию о ней в консоль
            std::wcout << L"Ошибка при создании mailslot: " << GetLastError() << std::endl;
    }
    else
    {
        _role = SERVER;
        std::wcout << L"\n"
                   << L"Программа работает в качестве сервера" << std::endl;
    }
}


// Инициализация переменных, необходимых для конфигурации mailslot при его создании
MailSlotHandler::MailSlotHandler( const std::wstring   &path,
                                  uint16_t              max_msg_size,
                                  uint32_t              waiting_time,
                                  const std::wstring   &mailslot_name,
                                  SECURITY_ATTRIBUTES  *sec_attributes):
    _path(path),
    _mailslot_name(mailslot_name.c_str()),
    _max_msg_size(max_msg_size),
    _waiting_time(waiting_time),
    _security_attributes(sec_attributes)
{
    // Задание формата вывода в консоль. Необходим для вывода wstring и иных wide переменных
    _setmode(_fileno(stdout), _O_U16TEXT);
}


// По окончанию работы деструктор MailSlotHandler автоматически закроет handle связанный с mailslot
MailSlotHandler::~MailSlotHandler()
{
    CloseHandle(_handle);
    std::cout << "Закрытие mailslot" << std::endl;
}

