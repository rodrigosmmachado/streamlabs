#include "stdafx.h"
#include "BasePipe.h"

std::string BasePipe::recvString()
{      
   std::stringstream ss; 
   char localBuff[128];
   DWORD dataSize = readDataSize();
   
   DWORD totalRead = 0;
   
   while(dataSize > 0)
   {
      DWORD toRead = min(sizeof(localBuff), dataSize);

      BOOL success = ReadFile(m_handle,
         localBuff,
         toRead,
         &totalRead,
         NULL);

      if (success || ERROR_MORE_DATA == ::GetLastError())
      {
         ss.write(localBuff, totalRead);

         dataSize -= totalRead;
      }
   }
   
   return ss.str();
}

DWORD BasePipe::readDataSize()
{
   DWORD totalRead;
   DWORD dataSize = 0;
   
   BOOL success = ReadFile(m_handle,
      &dataSize,
      sizeof(DWORD),
      &totalRead,
      NULL);

   if (success || ERROR_MORE_DATA == ::GetLastError())
   {
      return dataSize;
   }
   
   return 0;
}

void BasePipe::Flush()
{
   ::FlushFileBuffers(m_handle);
}