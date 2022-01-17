#ifndef MAILSLOT_FUNCTIONS_HPP
#define MAILSLOT_FUNCTIONS_HPP

#include <windows.h>

#include <string>
#include <cstdint>
#include <iostream>

enum ROLE{
    CLIENT,
    SERVER
};

class MailSlotHandler
{
private:
    HANDLE        _handle;
    ROLE          _role;

    std::wstring  _path;
    LPCWSTR       _mailslot_name;
    DWORD         _max_msg_size;
    DWORD         _waiting_time;
    DWORD         _next_msg_size;
    DWORD         _msg_amount_in_queue;
    std::wstring  _wstring_mailslot_name;
    std::wstring  _read_next_msg;
    SECURITY_ATTRIBUTES *_security_attributes;


public:
    MailSlotHandler(const std::wstring  &path,
                          uint16_t       max_msg_size,
                          uint32_t       waiting_time,
                    const std::wstring  &mailslot_name=L"",
                    SECURITY_ATTRIBUTES *sec_attributes = nullptr);

    ~MailSlotHandler();

    void     readInformationAboutRecordsInMailslot();
    void     readMailslotnameFromInput();
    void     getHandle();
    DWORD    getMsgAmountInQueue();
    DWORD    getNextMsgSize();
    DWORD    getMaxMsgSize();
    void     readNextMsg();
    void     recordMsgToMailslot();

    void     printNextMsgSize();
    void     printMsgAmountInQueue();
    void     printMaxMsgSize();
    void     printNextMsgInQueue();

    void     printActualMenu();
    ROLE     role();

private:
    void createMailSlot();
};

#endif // MAILSLOT_FUNCTIONS_HPP
