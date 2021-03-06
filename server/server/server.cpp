/*
Create 2 C++ console applications, one will be the client and the other one, the server.

1) The purpose of this assignment is to implement an interprocess communication.
2) The client will connect to the server and will make different calls to it.
3) They will communicate through a named pipe, the code doesn't have to be multi platform, the targeted platform is Windows.
4) The client should be able to make synchronous and asynchronous calls to the server.
5) The client should be able to send and receive different type of data to/from the server.
6) The server needs to register a set of functions / classes for the client to use.
7) The client needs to be able to create objects on the server and retrieve them, their attributes and call methods on them.


Once you are done, please explain your choice of implementation.Send the first version of your code after 8 hours of coding.

## Time Allotted: 8 Hours (you can begin whenever is good for you - 8 hours is just the max time you should spend on this project)
## Deploying: Please (1) share the code via a Github link and (2) include individual executable files for both client and server
## Disclaimer: All work you do will be your property. We're not trying to get you to do work for free. Hundreds of other people have done this assignment before and we have this functionality already live on our app. This is to test your actual coding skills.
*/

#include "stdafx.h"

#include "ServerPipe.h"
#include "MessageTypes.h"

using namespace std;

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
typedef struct _ReflectionResult
{
   int result;
   std::string message;

   _ReflectionResult(int result=0,const std::string& message="")
      : result(result)
      , message(message)
   {
   }

   _ReflectionResult(const _ReflectionResult &rh)
   {
      result = rh.result;
      message = rh.message;
   }
} ReflectionResult;

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
class RemoteControlledCache
{
   RTTR_ENABLE()

public:
   RemoteControlledCache() 
      : m_objectIndex(0)
   {
   }

   ~RemoteControlledCache() {}
   
   ///////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////
   int getNextIndex()
   {
      m_objectIndex++;
      return m_objectIndex;
   }
     
   ///////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////
   // create new class
   ReflectionResult CreateHandler(JSONRPC_PARAMS& params)
   {
      std::string klassToCreate = params[0];

      std::cout << "Inside CreateHandler: " << klassToCreate << "\n";

      rttr::type t = rttr::type::get_by_name( klassToCreate );
      rttr::variant var = t.create();

      int idx = ErrorObject::UnableToInstantiate;
      if (var.is_valid())
      {
         idx = getNextIndex();
         m_objects[idx] = std::make_pair(klassToCreate,var);
      }
            
      ReflectionResult result(idx);      
      return result;
   }

   ///////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////
   // invoke function or class method
   ReflectionResult InvokeHandler(JSONRPC_PARAMS& params)
   {
      if (params.size() < 2)
      {
         ReflectionResult result(ErrorObject::InvalidParamsError);
         return result;
      }

      int objectIdx = std::stoi(params[0]);
      std::string methodName = params[1];

      std::cout << "Inside InvokeHandler hdl: " << objectIdx << " method:" << methodName << "\n";

      ReflectionResult result;
      if (m_objects.find(objectIdx) == m_objects.end())
      {
         // no object instance for handler
         result.result = -1;
         result.message = "Object not found in cache";
      }
      else
      {
         std::string class_name = m_objects[objectIdx].first;
         rttr::variant pObject = m_objects[objectIdx].second;

         if (pObject.is_valid())
         {
            JSONRPC_PARAMS p;
            for (unsigned i = 3; i < params.size(); i++)
               p.push_back(params[i]);
            
            rttr::type t = rttr::type::get_by_name(class_name);
            
            rttr::method meth = t.get_method(methodName);

            rttr::variant var;
            if (meth.is_valid())
            {
               var = meth.invoke(pObject);
            }
            else
            {
               std::cout << "meth NOT" << std::endl;
            }

            if (var.is_type<ReflectionResult>())
            {
               ReflectionResult result;
               result = var.get_value<ReflectionResult>();
               return result;
            }
         }
         else
         {
            std::cout << "method is invalid " << std::endl;
         }
      }
      
      
      return result;
   }
   
   ///////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////
   ReflectionResult ListObjectsHandler(JSONRPC_PARAMS& params)
   {
      ReflectionResult result;
      std::stringstream ss;

      std::cout << "inside ListObjects" << std::endl;

      ss << "  Handler      Object type   " << std::endl;
      ss << "-------------------------------------------------" << std::endl;
      for (auto n : m_objects)
      {
         std::string classname = n.second.first;

         ss << n.first << "      " << classname << std::endl;
         ss << std::endl;
      }

      result.message = ss.str();
      result.result = result.message.size();
      return result;
   }

private:
   int m_objectIndex;

   // objects cache
   std::map<int, std::pair < std::string , rttr::variant > > m_objects;
};


class DemoClass 
{ 
   RTTR_ENABLE()

public:
   DemoClass() 
   {
     /// cout << ">>>> ctor DemoClass" << endl;
   }

   ~DemoClass() 
   {
      cout << ">>>> destructor DemoClass" << endl;
   }
   
   ReflectionResult setFirstString(JSONRPC_PARAMS& params)
   {
      cout << ">>>> inside setFirstString: " << params[0] << " " << params[1] << endl;

      ReflectionResult result(1);
      m_str1 = params[0];

      return result;
   }

   ReflectionResult setSecondString(JSONRPC_PARAMS& params)
   {
      cout << ">>>> inside setSecondString: " << params[0] << " " << params[1] << endl;

      ReflectionResult result(1);
      m_str2 = params[0];
      return result;
   }

   ReflectionResult concatenate()
   {      
      cout << ">>>> inside concatenate\n";

      m_concat = m_str1 + m_str2;

      ReflectionResult result(1,m_concat);
      return result;
   }
private:
   std::string m_str1;
   std::string m_str2;
   std::string m_concat;
};

// for test asynch calls
class TestClass
{
public:
   static void Sleep()
   {
      ::Sleep(15000);
   }
};

/*
* Register classes
*/
RTTR_REGISTRATION
{
   // required for RemoteControlledCache
   rttr::registration::class_<RemoteControlledCache>("RemoteControlledCache")
   .constructor<>()
   .method(INVOKE_METHOD, &RemoteControlledCache::InvokeHandler)
   .method(LIST_OBJECT_METHOD, &RemoteControlledCache::ListObjectsHandler)
   .method(INSTANTIATE_OBJECT, &RemoteControlledCache::CreateHandler);


   // Add your classes below
   rttr::registration::class_<DemoClass>("DemoClass")
   .constructor<>()
   .method("concatenate", &DemoClass::concatenate)
   .method("setFirstString", &DemoClass::setFirstString)
   .method("setSecondString", &DemoClass::setSecondString);

//   rttr::registration::class_<TestClass>("TestClass")
//      .method("Sleep", &Sleep);
}


int main()
{
   RemoteControlledCache cacheCtrl;
      
   ServerPipe *pPipe = new ServerPipe("StreamLabsPipe");
   pPipe->create();

   while (true)
   {
      cout << "waiting for connections ..." << endl;
      try
      {
         pPipe->WaitForClient();

         RequestObject message;
         pPipe->recv_message<RequestObject>(message);

         //
         //  Call only methods from RemoteControlledCache
         ////////
         rttr::method meth = rttr::type::get(cacheCtrl).get_method( message.getMethod() );

         if (meth.is_valid())
         {
            rttr::variant varRet = meth.invoke(cacheCtrl, message.getParams());

            if (varRet.is_type<ReflectionResult>())
            {
               ReflectionResult result = varRet.get_value<ReflectionResult>();
               if (result.result > 0)
               {
                  //cout << "result: " << result.result << ":" << result.message << endl;

                  ResponseObject response(result.result, message.getId(), result.message);
                  pPipe->send_message<ResponseObject>(response);
               }
               else
               {
                  ErrorObject error(result.result);
                  pPipe->send_message<ErrorObject>(error);
               }
            }
         }
         else
         {
            ErrorObject error(ErrorObject::MethodNotFoundError);
            pPipe->send_message<ErrorObject>(error);
         }
      }
      catch (NamedPipeGeneralException& e)
      {
         cout << "Pipe error:" << e.what() << endl;
      }

      pPipe->Disconnect();
   }
   
   delete pPipe;

   return 0;
}


