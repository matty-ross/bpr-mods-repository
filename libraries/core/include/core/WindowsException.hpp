#pragma once


#include <exception>


namespace Core
{
    class WindowsException : public std::exception
    {
    public:
        WindowsException(const char* message);

    public:
        const char* what() const noexcept override;

    private:
        char m_Buffer[1024] = {};
    };
}
