#pragma once


namespace Core
{
    class FileSystem
    {
    public:
        class FileContent
        {
            friend class Core::FileSystem;
        
        public:
            FileContent(void* buffer, size_t size)
                :
                m_Buffer(buffer),
                m_Size(size)
            {
            }
            
            ~FileContent()
            {
                if (m_OwnsBuffer)
                {
                    delete[] m_Buffer;
                }
            }

            template <typename T>
            T* as()
            {
                return static_cast<T*>(m_Buffer);
            }

        private:
            void* m_Buffer = nullptr;
            size_t m_Size = 0;
            bool m_OwnsBuffer = false;
        };

    public:
        static void CreateFolder(const char* path);
        static void ReadFileContent(const char* path, FileContent& content);
        static void WriteFileContent(const char* path, const FileContent& content);
    };
}
