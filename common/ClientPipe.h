#pragma once
#include "BasePipe.h"

class ClientPipe : public BasePipe
{
public:
   ClientPipe(const std::string name, const std::string server=std::string("."));
   virtual ~ClientPipe();

   BOOL connect();
};

