#include "mailslot_functions.hpp"

#include <iostream>
#include <cstring>

// Максимальный размер сообщения
const uint16_t MAX_MSG_SIZE = 424;


// Результирующая строка эквивалентна строке "\\.\mailslot\\" из-за экранирования символов
const std::wstring MAIN_PATH = L"\\\\.\\mailslot\\";


// Период ожидания (бесконечный)
const uint32_t WAITING_TIME = MAILSLOT_WAIT_FOREVER;


// Параметры безопасности при передаче дочернему процессу (Пустая струкура. Дочерних процессов нет)
const SECURITY_ATTRIBUTES* WITHOUT_SECURITY_ATTRIBUTES = nullptr;

// Для облегчения читабельности коды создаем именнованные константы, которые после будут использоваться
// c целью устранения магических переменных. Namespace нужен для обращения к константам по USER_ANSWERS для большей
// наглядности и для возможного устранения конфликтов имен с другими константами
namespace USER_ANSWERS{
enum USER_ANSWERS
{
    AMOUNT_OF_MSGS = 1,
    LAST_MSG_SIZE  = 2,
    MAX_MSG_SIZE   = 3,
    READ_MSG       = 4,
    WRITE_MSG      = 4,
    END_OF_WORK    = 5
};
}

int main()
{
    // Создаем экземпляр класса, обслуживаший описатель mailslot и функции, связанные с ним
    MailSlotHandler mailslot_handler(MAIN_PATH, MAX_MSG_SIZE, WAITING_TIME);
    // Считываем название mailslot c консоли и создаём mailslot с указанным именем. В случае если
    // c указанным именем уже существует дальнейшая работа будет производиться с уже существующим mailslot
    mailslot_handler.readMailslotnameFromInput();

    // Переменная для хранения выбора пользователя
    uint16_t user_answer = 0;

    do
    {
        mailslot_handler.printActualMenu();

        // Считываем выбор пользователя с консоли
        std::wcin >> user_answer;

        getchar();

        // Выполняем функцию в соответствии с выбором пользователя
        if( user_answer == USER_ANSWERS::AMOUNT_OF_MSGS )
            mailslot_handler.printMsgAmountInQueue();

        if( user_answer == USER_ANSWERS::LAST_MSG_SIZE  )
            mailslot_handler.printNextMsgSize();

        if( user_answer == USER_ANSWERS::MAX_MSG_SIZE   )
            mailslot_handler.printMaxMsgSize();

        if( mailslot_handler.role() == CLIENT )
        {
            if( user_answer == USER_ANSWERS::WRITE_MSG      )
                mailslot_handler.recordMsgToMailslot();
        }
        else if( mailslot_handler.role() == SERVER )
        {
            if( user_answer == USER_ANSWERS::READ_MSG       )
                mailslot_handler.printNextMsgInQueue();
        }

    // В случае если пользователь выбрал вариант с завершением работы программы, выходим из цикла и завершаем работу программы
    } while( user_answer != USER_ANSWERS::END_OF_WORK   );

    // По завершению работы handle будет автоматически закрыт с помощью деструктора класса MailslotHandler
    std::wcout << L"Завершение работы программы" << std::endl;
}
