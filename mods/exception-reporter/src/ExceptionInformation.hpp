#pragma once


#include <string>
#include <Windows.h>


class ExceptionInformation
{
public:
    ExceptionInformation(const EXCEPTION_RECORD* exceptionRecord, const CONTEXT* contextRecord);

public:
    std::string GetCode() const;
    std::string GetAddress() const;
    std::string GetFlags() const;
    std::string GetParameters() const;
    std::string GetRegisters() const;
    std::string GetStackTrace() const;

private:
    const EXCEPTION_RECORD* m_ExceptionRecord = nullptr;
    const CONTEXT* m_ContextRecord = nullptr;
};
