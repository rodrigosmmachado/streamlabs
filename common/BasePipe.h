#pragma once
#include "MessageTypes.h"


class NamedPipeGeneralException : public std::exception
{
public:
   NamedPipeGeneralException(DWORD error)
   {
      if (error == 0)
      {
         m_message = "No Error";
         return;
      }

      LPSTR messageBuffer = nullptr;
      size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_SYSTEM |
         FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL,
         error,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         (LPSTR)&messageBuffer, 0, NULL);

      m_message = "Windows Error:";
      m_message += messageBuffer;

      //Free the buffer.
      LocalFree(messageBuffer);
   }

   virtual const char* what() const throw()
   {
      return m_message.c_str();
   }

private:
   std::string m_message;
};

class BasePipe
{
public:
   BasePipe() {};
   virtual ~BasePipe() 
   {
      Flush();
   };
     
   template <class T>
   DWORD send_message(const T& message)
   {
      DWORD written;
      std::string str = serialize(message);

      DWORD msgSize = str.size();
      WriteFile(m_handle,
         &msgSize,
         sizeof(DWORD),
         &written,
         NULL);

      BOOL success = WriteFile(m_handle,
         str.c_str(),
         str.size(),
         &written,
         NULL);

      if (!success)
      {
         throw NamedPipeGeneralException(::GetLastError());
      }

      //std::cout << "send:\n" << str << std::endl;

      Flush();

      return written;
   }

   template <class T>
   ErrorObject recv_message(T& message)
   {     
      ErrorObject error;

      std::string  jsonStr = recvString();
      if (0 < jsonStr.size())
      {
         //std::cout << "recv:\n" << jsonStr << std::endl;

         try
         {
            deserialize<T>(message, jsonStr);
         }
         catch(...)
         {
            deserialize<ErrorObject>(error, jsonStr);
         }         
      }
      return error;
   }
   
   /////////////////////////////
   void Flush();

protected:
   std::string m_name;
   HANDLE m_handle;
     

private:
   DWORD readDataSize();
   std::string recvString();   
};

