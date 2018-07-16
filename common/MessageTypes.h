#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>


////////////////////////////////////////
// methods names

// AKA: new 
const std::string INSTANTIATE_OBJECT = "Create";

// Invoke method name (call a function or method)
const std::string INVOKE_METHOD = "Invoke";

// List objects in remote cache
const std::string LIST_OBJECT_METHOD = "ListObjects";


typedef std::vector<std::string>  JSONRPC_PARAMS;

class RequestObject 
{
public:
   ////////////////////////////////
   RequestObject(const std::string& method, const JSONRPC_PARAMS& params)
   : m_method(method)
   , m_params(params)
   , m_jsonrpc(std::string("2.0"))
   , m_id(RequestObject::getNextId())
   {
      
   }

   RequestObject(const RequestObject& rh)
   {
      m_method = rh.m_method;
      m_id = rh.m_id;
      m_params = rh.m_params;
   }

   RequestObject()
   {
   }

   ////////////////////////////////
   ~RequestObject() {}

   ////////////////////////////////
   int getId()
   {
      return m_id;
   }

   ////////////////////////////////
   std::string getMethod()
   {
      return m_method;
   }

   ////////////////////////////////
   JSONRPC_PARAMS& getParams()
   {
      return m_params;
   }

   ////////////////////////////////
   static int getNextId()
   {
      static int localId = 0;
      
      // TODO - add sync if multithread
      return localId++;
   }

   template<class Archive>
   void serialize(Archive & archive)
   {
      archive(cereal::make_nvp("jsonrpc",m_jsonrpc), 
         cereal::make_nvp("method",m_method), 
         cereal::make_nvp("params",m_params), 
         cereal::make_nvp("id",m_id) );
   }

   friend std::ostream& operator<< (std::ostream& os, const RequestObject& obj)
   {
      os << "jsonrpc:" << obj.m_jsonrpc << "  method:" << obj.m_method << "  id:" << obj.m_id << "  params:[";

      for (auto s : obj.m_params)
      {
         os << s << " ";
      }

      os << "]";

      return os;
   }

private:
   std::string m_jsonrpc;
   std::string m_method;
   int  m_id;
   JSONRPC_PARAMS m_params;
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class ErrorObject
{
public:
   static const int UnableToInstantiate   = -32000;

   static const int ParseError            = -32700;
   static const int InvalidRequestError   = -32600;
   static const int MethodNotFoundError   = -32601;
   static const int InvalidParamsError    = -32602;
   static const int InternalError         = -32603;
   static const int ServerError           = -32000;

   ////////////////////////////////
   ErrorObject(const int code,const std::string& message = "", const std::string& data = "") 
      : m_code(code)
      , m_message(message)
      , m_data(data)
   {
      if (m_message.size() == 0)
      {
         m_message = ErrorObject::DecodeError(m_code);
      }
   }

   ////////////////////////////////
   ErrorObject(const ErrorObject& rh) 
   {
      m_code = rh.m_code;
      m_message = rh.m_message;
      m_data = rh.m_data;
   }

   ////////////////////////////////
   ErrorObject()
      : m_code(0)
   {

   }
   template<class Archive>
   void serialize(Archive & archive)
   {
      archive(cereal::make_nvp("code",m_code), 
            cereal::make_nvp("message",m_message), 
         cereal::make_nvp("data",m_data) );
   }

   friend std::ostream& operator<< (std::ostream& os, const ErrorObject& err)
   {
      os << "code:" << err.m_code << "  msg:" << err.m_message << "  data:" << err.m_data;
      return os;
   }

   
   static std::string DecodeError(int error)
   {
      std::string str;

      if (error >= -32000 && error <= -32099)
         str = "Server error";
      else
      {
         switch (error)
         {
            case -32000: str = "Unable to instantiate"; break;
            case -32700: str = "Parse error"; break;
            case -32600: str = "Invalid Request"; break;
            case -32601: str = "Method not found"; break;
            case -32602: str = "Invalid Params"; break;
            case -32603: str = "Internal Error"; break;
            default:
               str = "Unknown Error";
            break;
         }
      }
      return str;
   }

private:
   int m_code;
   std::string m_message;
   std::string m_data;
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class ResponseObject
{
public:
   ////////////////////////////////
   ResponseObject()
      : m_jsonrpc(std::string("2.0"))
      , m_id(0)
      , m_result(-1)
      , m_data("")
   {
   }

   ////////////////////////////////
   ResponseObject(const int result,const int id, const std::string& data)
   : m_jsonrpc(std::string("2.0"))
   , m_id(id)
   , m_result(result)
   , m_data(data)
   {
   }
   
   ////////////////////////////////
   template<class Archive>
   void serialize(Archive & archive)
   {
      archive(cereal::make_nvp("jsonrpc",m_jsonrpc), 
         cereal::make_nvp("result", m_result), 
         cereal::make_nvp("id", m_id),
         cereal::make_nvp("data", m_data));
   }

   ////////////////////////////////
   int getId()
   {
      return m_id;
   }

   ////////////////////////////////
   int getResult()
   {
      return m_result;
   }

   std::string getData()
   {
      return m_data;
   }

   // for debug
   friend std::ostream& operator<< (std::ostream& os, const ResponseObject& obj)
   {
      os << "jsonrpc:" << obj.m_jsonrpc << "  result:" << obj.m_result << "  id:" << obj.m_id << " data:" << obj.m_data;
      return os;
   }

private:
   std::string m_jsonrpc;
   std::string m_data;
   int m_result;
   int  m_id;
};


//  helper function
template<class T>
std::string serialize(const T& m)
{
   std::stringstream ss;
   do
   {
      cereal::JSONOutputArchive oArch(ss);
      oArch(m);
   } while (false);

   return ss.str();
}

template<class T>
DWORD deserialize(T& m, const std::string jsonString)
{
   do
   {
      std::stringstream ss;
      ss << jsonString;
      cereal::JSONInputArchive iArch(ss);

      iArch(m);
   } while (false);

   return 0;
}



