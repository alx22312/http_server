#include "request_handler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "reply.hpp"
#include "request.hpp"
#include <vector>
#include <boost/algorithm/string.hpp>

namespace http {
namespace server {

size_t N = 100;

size_t max_length = 1024;

request_handler::request_handler()
{
}

std::string request_handler::parse_command(request& req)
{
  boost::recursive_mutex mutex;
  char uncorrect[] = "Uncorrect request\n";
  char get_command[] = "get";
  char add_command[] = "add";
  char del_command[] = "del";
  char wait_command[] = "wait";
  char wait_command1[] = "equal";
  char wait_command2[] = "greater";
  char wait_command3[] = "less";
  char done[] = "done\n";
  char str[max_length];
  size_t * i =new size_t(0);
  size_t * com_num =new size_t(-1);
  size_t * tmp_N =new size_t(0);
  bool tmp_N_exist = false;
  char * pch;
  try
  {
  std::string tmp = req.buff;
  std::vector <std::string> fields;
  using namespace std;
  //boost::split(fields, tmp, boost::is_any_of("GET "));
  //std::split( fields, tmp, "GET ");
  std::size_t found = tmp.find("GET ");
  std::size_t found2 = tmp.find(" HTTP");
  tmp = tmp.substr (found+4,found2-4);
  pch = new char[tmp.length() + 1];
  //boost::split(fields, tmp, boost::is_any_of(" HTTP"));
  //split( fields, tmp, " HTTP");
  strcpy(pch, tmp.c_str());
  //sprintf(pch, "%s",tmp.c_str());
  pch = strtok (pch, "/");
  while (pch != NULL)
  {
    *i=*i+1;
    switch(*i)
    {
    case 1:
        {
        if (std::strcmp(add_command,pch) == 0)
        //if (add_command == pch)
             *com_num = 0;
        //else if (del_command == pch)
        else if (std::strcmp(del_command,pch) == 0)
                *com_num = 1;
             //else if (get_command == pch)
             else if (std::strcmp(get_command,pch) == 0)
             {
                 *com_num = 2;
                 break;
             }
             //else if (wait_command == pch)
             else if (std::strcmp(wait_command,pch) == 0)
                    *com_num = -1;
        }
    case 2:
        {
        if (*com_num ==-1)
            {
                //if (wait_command1 == pch)
                if (std::strcmp(wait_command1,pch) == 0)
                     *com_num = 3;
                //else if (wait_command2 == pch)
                else if (std::strcmp(wait_command2,pch) == 0)
                        *com_num = 4;
                     //else if (wait_command3 == pch)
                     else if (std::strcmp(wait_command3,pch) == 0)
                            *com_num = 5;
            }
            else
            {
                if (isdigit(*pch))
                    {
                        *tmp_N = atoi(pch);
                        tmp_N_exist = true;
                        break;
                    }
            }
        }
    case 3:
        {
        if (isdigit(*pch))
            {
                *tmp_N = atoi(pch);
                tmp_N_exist = true;
            }
        }
    }
    pch = strtok (NULL, "/");
  }
    if (*com_num==2)
    {
        sprintf(str, "N = %u\n",N);
//        std::cout << str;
//        std::cout << strlen(str) << "\n";
        return str;
    }
    //Case 0,1,3 - locked
    if (tmp_N_exist)
        switch (*com_num)
        {
            case 0:
                {
                    boost::recursive_mutex::scoped_lock scoped_lock(mutex);
                    N= N + *tmp_N;
                    return done;
                }
            case 1:
                {
                    boost::recursive_mutex::scoped_lock scoped_lock(mutex);
                    N= N - *tmp_N;
                    return done;
                }
            case 3:
                {
                     for (;;)
                        {
                            if (N==*tmp_N)
                            {
                                return done;
                            }
                        }
                }
             case 4:
                {
                     for (;;)
                        {
                            if (N>*tmp_N)
                            {
                                return done;
                            }
                        }
                }
            case 5:
                {
                     for (;;)
                        {
                            if (N<*tmp_N)
                            {
                                return done;
                            }
                        }
                }
        }
    else if (*com_num!=2)
    {
        *com_num=-2;
        return uncorrect;
    }
//    if (*com_num==-2)
//        return uncorrect;
    delete [] pch;
    delete [] str;
    delete i;
    delete tmp_N;
    delete com_num;
  }
  catch (const boost::system::error_code& e)
  {
    std::cout << "Exception: " << e << "\n";
    delete [] pch;
    delete [] str;
    delete i;
    delete tmp_N;
    delete com_num;
    return uncorrect;
  }
}

void request_handler::handle_request(request& req, reply& rep)
{
  // Fill out the reply to be sent to the client.
  rep.status = reply::ok;
  //Write some content...
  rep.content = request_handler::parse_command(req);
  req.buff = "";

  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "text/html;charset=windows-1251";
}

} // namespace server
} // namespace http
