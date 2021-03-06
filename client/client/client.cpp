// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ClientPipe.h"
#include "MessageTypes.h"

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

using namespace std;

typedef DWORD RemoteObjecHandler;

class RemoteCacheClient
{
public:
   RemoteCacheClient(ClientPipe *pClient)
      : m_pPipe(pClient)
   {
   }

   ~RemoteCacheClient()
   {
      delete m_pPipe;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////
   BOOL invoke(RemoteObjecHandler& objectHandler, const std::string& method, JSONRPC_PARAMS& params = JSONRPC_PARAMS(), std::ostream& out = std::cout)
   {
      try
      {
         JSONRPC_PARAMS new_params;
         
         new_params.push_back(std::to_string(objectHandler));
         new_params.push_back(method);

         for (auto s : params)
         {
            new_params.push_back(s);
         }

         if (m_pPipe->connect())
         {
            RequestObject message(INVOKE_METHOD, new_params);

            m_pPipe->send_message<RequestObject>(message);

            ResponseObject response;
            ErrorObject error = m_pPipe->recv_message<ResponseObject>(response);

            if (response.getResult() < 0)
            {
               std::cout << "Error\nresponse: " << response << std::endl;
               std::cout << "error: " << error << std::endl;
            }
            else
            {
               if (response.getData().size() > 0)
                  out << response.getData();
            }
         }
      }
      catch (exception &e)
      {
         cout << "Unable to communicate with server:" << e.what() << endl;
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////
   BOOL callAsynch(const std::string& function, JSONRPC_PARAMS& params)
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////
   RemoteObjecHandler create(const std::string& klass)
   {      
      RemoteObjecHandler result = 0;
      try
      {
         if (m_pPipe->connect())
         {
            JSONRPC_PARAMS params{ klass };            
            RequestObject message(INSTANTIATE_OBJECT, params);

            m_pPipe->send_message<RequestObject>(message);

            ResponseObject response;
            
            ErrorObject error = m_pPipe->recv_message<ResponseObject>(response);
            
            if (response.getResult() < 0)
            {
               std::cout << "Error\nresponse: " << response << std::endl;
               std::cout << "error: " << error << std::endl;
            }
            else
            {
               result = response.getResult();
            }
         }
      }
      catch (exception &e)
      {
         cout << "Unable to communicate with server:" << e.what() << endl;
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////////////////////
   BOOL call(const std::string& function, JSONRPC_PARAMS& params, std::ostream& out = std::cout)
   {
      BOOL result = false;
      try
      {
         if (m_pPipe->connect())
         {
            RequestObject message(function, params);

            m_pPipe->send_message<RequestObject>(message);

            ResponseObject response;

            ErrorObject error = m_pPipe->recv_message<ResponseObject>(response);

            result = response.getResult();

            if (result > 0)
            {
               if (response.getData().size() > 0)
               {
                  out << response.getData();
                  out << std::endl;
               }
            }
            else
            {
               std::cout << "response: " << response << std::endl;
               std::cout << "error: " << error << std::endl;
            }
         }
      }
      catch (exception &e)
      {
         cout << "Unable to communicate with server:" << e.what() << endl;
      }

      return result;
   }

private:
   ClientPipe *m_pPipe;
};

JSONRPC_PARAMS getFromCmdline(string &cmdLine)
{
   JSONRPC_PARAMS paramList;   
   istringstream input(cmdLine);
   string param;

   while (input >> param)
   {
      paramList.push_back(param);
   }

   return paramList;
}

int main()
{   
   RemoteCacheClient clientCache(new ClientPipe("StreamLabsPipe"));
  

   /*
    * Create 3 instances 
    */
   cout << "Creating 3 instances of class DemoClass" << endl;

   RemoteObjecHandler instance1 = clientCache.create(std::string("DemoClass"));

   if (instance1 == 0)
      cout << "failed alloc DemoClass1" << endl;

   RemoteObjecHandler instance2 = clientCache.create(std::string("DemoClass"));

   if (instance1 == 0)
      cout << "failed alloc DemoClass1" << endl;

   RemoteObjecHandler instance3 = clientCache.create(std::string("DemoClass"));

   if (instance1 == 0)
      cout << "failed alloc DemoClass1" << endl;
   
   cout << "Done" << endl;
   

   /*
   * List 3 instances in cache
   */
   cout << "Calling function '" << LIST_OBJECT_METHOD << "'" << endl;

   // list objects - for now list on server only
   clientCache.call( LIST_OBJECT_METHOD, JSONRPC_PARAMS());
   
   cout << "Done" << endl;

   /*
   * call methods from instance 2
   */
   cout << "Calling method 'setFirstString'" << endl;
   
   // call method
   clientCache.invoke(instance2, "setFirstString" , JSONRPC_PARAMS{ "This is String 1" });


   cout << "calling 'setSecondString' from instance2 of DemoClass" << endl;

   clientCache.invoke(instance2, "setSecondString", JSONRPC_PARAMS{ "This is String 2" });

   cout << "calling concatenate from instance2 of DemoClass" << endl;

   clientCache.invoke(instance2, "concatenate");

   cout << "Done" << endl;

   int i;
   cin >> i;

   return 0;
}

