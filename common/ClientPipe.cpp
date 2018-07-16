#include "stdafx.h"
#include "ClientPipe.h"


ClientPipe::ClientPipe(const std::string name, const std::string server)
{
   m_name = "\\\\" + server + "\\pipe\\" + name;
}


ClientPipe::~ClientPipe()
{
   CloseHandle(m_handle);
}


BOOL ClientPipe::connect()
{
   BOOL connected = false;
   while (!connected)
   {
      m_handle = CreateFileA(m_name.c_str(),   
                              GENERIC_READ | GENERIC_WRITE,                                 
                              0,              // no sharing 
                              NULL,           // default security attributes
                              OPEN_EXISTING,  // opens existing pipe 
                              0,              // default attributes 
                              NULL);          // no template file 

      if (m_handle != INVALID_HANDLE_VALUE)
      {
         connected = true;
         
         // The pipe connected; change to message-read mode. 
         DWORD dwMode = PIPE_READMODE_MESSAGE;
         if(0 == SetNamedPipeHandleState(m_handle,  
                                      &dwMode,  // new pipe mode 
                                        NULL, 
                                         NULL) )
         {
            throw NamedPipeGeneralException(::GetLastError());
         }
      }
      else
      {
         // Exit if an error other than ERROR_PIPE_BUSY occurs. 
         if (GetLastError() != ERROR_PIPE_BUSY)
         {
            throw NamedPipeGeneralException(::GetLastError());
         }

         // All pipe instances are busy, so wait for 20 seconds. 
         if (!WaitNamedPipeA(m_name.c_str(), 20000))
         {
            printf("Could not open pipe: 20 second wait timed out.");
            break;
         }
      }
   }

   return connected;
}
