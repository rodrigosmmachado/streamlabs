#include "stdafx.h"
#include "ServerPipe.h"


ServerPipe::ServerPipe(const std::string name, const std::string server)
{
   m_name = "\\\\" + server + "\\pipe\\" + name;
}


ServerPipe::~ServerPipe()
{
   Flush();
   CloseHandle(m_handle);
}

bool ServerPipe::create()
{
   BOOL success = false;
   m_handle = CreateNamedPipeA(m_name.c_str(),
      PIPE_ACCESS_DUPLEX,  // read/write access
      PIPE_TYPE_MESSAGE |              // byte type                                                                                         
      PIPE_READMODE_MESSAGE |
      PIPE_WAIT,             // blocking mode                                                   
      //PIPE_UNLIMITED_INSTANCES,
      1,
      BUFSIZE, // output buffer size                                                   
      BUFSIZE, // input buffer size                                                   
      PIPE_TIMEOUT, // client time-out                                                   
      NULL); // no security attribute

   if (m_handle == INVALID_HANDLE_VALUE)
   {
      throw NamedPipeGeneralException(::GetLastError());
   }
   else
   {
      success = true;
   }

   return success;
}

void ServerPipe::Disconnect()
{
   FlushFileBuffers(m_handle);
   DisconnectNamedPipe(m_handle);
}

bool ServerPipe::WaitForClient()
{
   BOOL success = ConnectNamedPipe(m_handle, NULL) ? TRUE : (::GetLastError() == ERROR_PIPE_CONNECTED);

   if (!success)
   {
      CloseHandle(m_handle);

      throw NamedPipeGeneralException(::GetLastError());
   }

   return success;
}





