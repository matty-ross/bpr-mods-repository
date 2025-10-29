#include "Path.hpp"

#include <Shlwapi.h>
#include <ShlObj.h>

#include "WindowsException.hpp"


namespace Core
{
    Path::Path(const char* path)
    {
        ExpandEnvironmentStringsA(path, m_Path, sizeof(m_Path));
    }

    const char* Path::GetPath() const
    {
        return m_Path;
    }

    Path& Path::Append(const char* path)
    {
        PathAppendA(m_Path, path);
        return *this;
    }
    
    void Path::Create() const
    {
        int result = SHCreateDirectoryExA(NULL, m_Path, nullptr);
        if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS)
        {
            SetLastError(result);
            throw WindowsException("Cannot create directory");
        }
    }
}
