#include "ExceptionInformation.hpp"

#include <cstdio>
#include <Shlwapi.h>
#include <DbgHelp.h>


ExceptionInformation::ExceptionInformation(const EXCEPTION_RECORD* exceptionRecord, const CONTEXT* contextRecord)
    :
    m_ExceptionRecord(exceptionRecord),
    m_ContextRecord(contextRecord)
{
}

std::string ExceptionInformation::GetCode() const
{
    auto getCodeName = [](uint32_t code) -> const char*
    {
        switch (code)
        {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_GUARD_PAGE:               return "EXCEPTION_GUARD_PAGE";
        case EXCEPTION_INVALID_HANDLE:           return "EXCEPTION_INVALID_HANDLE";
        }
        
        return "???";
    };

    char codeBuffer[64] = {};
    sprintf_s(codeBuffer, "0x%08X  [%s]", m_ExceptionRecord->ExceptionCode, getCodeName(m_ExceptionRecord->ExceptionCode));
    
    return codeBuffer;
}

std::string ExceptionInformation::GetAddress() const
{
    auto getModuleName = [](HMODULE moduleHandle) -> std::string
    {
        char fileName[MAX_PATH] = {};
        GetModuleFileNameA(moduleHandle, fileName, sizeof(fileName));

        return PathFindFileNameA(fileName);
    };

    HMODULE moduleHandle = NULL;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        static_cast<LPCSTR>(m_ExceptionRecord->ExceptionAddress),
        &moduleHandle
    );

    char addressBuffer[128] = {};
    if (moduleHandle != NULL)
    {
        const void* baseAddress = moduleHandle;
        ptrdiff_t rva = static_cast<const uint8_t*>(m_ExceptionRecord->ExceptionAddress) - static_cast<const uint8_t*>(baseAddress);
        sprintf_s(addressBuffer, "0x%p  [%s + 0x%X]", m_ExceptionRecord->ExceptionAddress, getModuleName(moduleHandle).c_str(), rva);
    }
    else
    {
        sprintf_s(addressBuffer, "0x%p", m_ExceptionRecord->ExceptionAddress);
    }
    
    return addressBuffer;
}

std::string ExceptionInformation::GetFlags() const
{
    auto getFlagName = [](uint32_t flag) -> const char*
    {
        switch (flag)
        {
        case EXCEPTION_NONCONTINUABLE:     return "EXCEPTION_NONCONTINUABLE";
        case EXCEPTION_UNWINDING:          return "EXCEPTION_UNWINDING";
        case EXCEPTION_EXIT_UNWIND:        return "EXCEPTION_EXIT_UNWIND";
        case EXCEPTION_STACK_INVALID:      return "EXCEPTION_STACK_INVALID";
        case EXCEPTION_NESTED_CALL:        return "EXCEPTION_NESTED_CALL";
        case EXCEPTION_TARGET_UNWIND:      return "EXCEPTION_TARGET_UNWIND";
        case EXCEPTION_COLLIDED_UNWIND:    return "EXCEPTION_COLLIDED_UNWIND";
        case EXCEPTION_SOFTWARE_ORIGINATE: return "EXCEPTION_SOFTWARE_ORIGINATE";
        }
        
        return "???";
    };

    char flagsBuffer[16] = {};
    sprintf_s(flagsBuffer, "0x%08X", m_ExceptionRecord->ExceptionFlags);

    std::string flags = flagsBuffer;
    for (uint32_t bit = 0; bit < 32; ++bit) // Test every bit.
    {
        uint32_t flag = static_cast<uint32_t>(1) << bit;
        if (m_ExceptionRecord->ExceptionFlags & flag)
        {
            char flagBuffer[64] = {};
            sprintf_s(flagBuffer, "\n - 0x%08X  [%s]", flag, getFlagName(flag));
            flags += flagBuffer;
        }
    }
    
    return flags;
}

std::string ExceptionInformation::GetParameters() const
{
    std::string parameters;
    for (uint32_t i = 0; i < m_ExceptionRecord->NumberParameters; ++i)
    {
        char parameterBuffer[16] = {};
        sprintf_s(parameterBuffer, "0x%08X\n", m_ExceptionRecord->ExceptionInformation[i]);
        parameters += parameterBuffer;
    }
    
    return parameters;
}

std::string ExceptionInformation::GetRegisters() const
{
    char registersBuffer[256] = {};
    sprintf_s(
        registersBuffer,
        "EAX     0x%08X\n"
        "EBX     0x%08X\n"
        "ECX     0x%08X\n"
        "EDX     0x%08X\n"
        "ESI     0x%08X\n"
        "EDI     0x%08X\n"
        "EBP     0x%08X\n"
        "ESP     0x%08X\n"
        "EIP     0x%08X\n"
        "EFLAGS  0x%08X",
        m_ContextRecord->Eax,
        m_ContextRecord->Ebx,
        m_ContextRecord->Ecx,
        m_ContextRecord->Edx,
        m_ContextRecord->Esi,
        m_ContextRecord->Edi,
        m_ContextRecord->Ebp,
        m_ContextRecord->Esp,
        m_ContextRecord->Eip,
        m_ContextRecord->EFlags
    );
    
    return registersBuffer;
}

std::string ExceptionInformation::GetStackTrace() const
{
    constexpr int maxStackFrames = 30;

    STACKFRAME stackFrame =
    {
        .AddrPC =
        {
            .Offset = m_ContextRecord->Eip,
            .Mode = AddrModeFlat,
        },
        .AddrFrame =
        {
            .Offset = m_ContextRecord->Ebp,
            .Mode = AddrModeFlat,
        },
        .AddrStack =
        {
            .Offset = m_ContextRecord->Esp,
            .Mode = AddrModeFlat,
        }
    };
    CONTEXT context = *m_ContextRecord;

    std::string stackTrace;
    for (int i = 0; i < maxStackFrames; ++i)
    {
        if (StackWalk(
            IMAGE_FILE_MACHINE_I386,
            GetCurrentProcess(),
            GetCurrentThread(),
            &stackFrame,
            &context,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        ) == FALSE)
        {
            break;
        }

        char returnAddressBuffer[16] = {};
        sprintf_s(returnAddressBuffer, "0x%p\n", reinterpret_cast<void*>(stackFrame.AddrReturn.Offset));
        stackTrace += returnAddressBuffer;
    }

    return stackTrace;
}
