#include <string>
#include <map>

/***********************************************************************
 * A java-like System.getProperty() utility.
 ***********************************************************************/
class System
{
private:
   map<string, string> mProperties;

public:

   string getProperty(string pKey, string pDefault)
   {
      if (mProperties.find(pKey) == mProperties.end())
      {
         return pDefault;
      }
      else
      {
         return mProperties[pKey];
      }
   }

   void setProperty(string pKeyValue)
   {
      size_t i = pKeyValue.find_first_of('=');
      if (i != string::npos)
      {
         string key = pKeyValue.substr(0, i);
         string value = pKeyValue.substr(i + 1);
         setProperty(key, value);
      }
   }

   void setProperty(string pKey, string pValue)
   {
      mProperties.insert(pair<string, string>(pKey, pValue));
   }

} System;

class Boolean
{
public:
   bool getBoolean(string pKey)
   {
      bool result = false;
      result = toBoolean(System.getProperty(pKey, "0"));
      return result;
   }
private:
   bool toBoolean(string name)
   { 
      return (name != "0");
   }
} Boolean;
