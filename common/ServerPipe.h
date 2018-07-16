#pragma once
#include "BasePipe.h"

class ServerPipe : public BasePipe
{
public:
   static const int BUFSIZE = 4096;
   static const int PIPE_TIMEOUT = 1000;

   ServerPipe(const std::string name, const std::string server = std::string("."));
   virtual ~ServerPipe();

   bool create();
   bool WaitForClient();
   void Disconnect();
};

