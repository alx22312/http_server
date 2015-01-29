#include "request_parser.hpp"
#include "request.hpp"

namespace http {
namespace server {

request_parser::request_parser()
  : state_(method_start)
{
}

void request_parser::reset()
{
  state_ = method_start;
}

boost::tribool request_parser::consume(request& req, char input)
{
  req.buff.push_back(input);
  switch (state_)
  {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      state_ = method;
      req.method.push_back(input);
      return boost::indeterminate;
    }
  case method:
    if (input == ' ')
    {
      state_ = uri;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.method.push_back(input);
      return boost::indeterminate;
    }
  case uri:
    if (input == ' ')
    {
      state_ = http_version_h;
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      req.uri.push_back(input);
      return boost::indeterminate;
    }
  case http_version_h:
    if (input == 'H')
    {
      state_ = http_version_t_1;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_t_1:
    if (input == 'T')
    {
      state_ = http_version_t_2;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_t_2:
    if (input == 'T')
    {
      state_ = http_version_p;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_p:
    if (input == 'P')
    {
      state_ = http_version_slash;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_slash:
    if (input == '/')
    {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      state_ = http_version_major_start;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_major_start:
    if (is_digit(input))
    {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      state_ = http_version_major;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_major:
    if (input == '.')
    {
      state_ = http_version_minor_start;
      return boost::indeterminate;
    }
    else if (is_digit(input))
    {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_minor_start:
    if (is_digit(input))
    {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      state_ = http_version_minor;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case http_version_minor:
    if (input == '\r')
    {
      state_ = expecting_newline_1;
      return boost::indeterminate;
    }
    else if (is_digit(input))
    {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case expecting_newline_1:
    if (input == '\n')
    {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case header_line_start:
    if (input == '\r')
    {
      state_ = expecting_newline_3;
      return boost::indeterminate;
    }
    else if (!req.headers.empty() && (input == ' ' || input == '\t'))
    {
      state_ = header_lws;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.headers.push_back(header());
      req.headers.back().name.push_back(input);
      state_ = header_name;
      return boost::indeterminate;
    }
  case header_lws:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (input == ' ' || input == '\t')
    {
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      state_ = header_value;
      req.headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case header_name:
    if (input == ':')
    {
      state_ = space_before_header_value;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.headers.back().name.push_back(input);
      return boost::indeterminate;
    }
  case space_before_header_value:
    if (input == ' ')
    {
      state_ = header_value;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case header_value:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return boost::indeterminate;
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      req.headers.back().value.push_back(input);
      return boost::indeterminate;
    }
  case expecting_newline_2:
    if (input == '\n')
    {
      state_ = header_line_start;
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case expecting_newline_3:
    return (input == '\n');
  default:
    return false;
  }
}

//void request_parser::parse_command(request& req)
//{
//  boost::recursive_mutex mutex;
//  char uncorrect[] = "Uncorrect request\n";
//  try
//  {
//  char get_command[] = "get";
//  char add_command[] = "add";
//  char del_command[] = "del";
//  char wait_command[] = "wait";
//  char wait_command1[] = "equal";
//  char wait_command2[] = "greater";
//  char wait_command3[] = "less";
//  char done[] = "done\n";
//  char str[max_length];
//  char * pch;
//  size_t * i =new size_t(0);
//  size_t * com_num =new size_t(-1);
//  size_t * tmp_N =new size_t(0);
//  bool tmp_N_exist = false;
//  std::cout << req.buff << "\n";
//  strcpy(pch, req.buff.c_str());
//  pch = strtok (pch,"/");
//  while (pch != NULL)
//  {
//    *i=*i+1;
//    switch(*i)
//    {
//    case 1:
//        {
//        if (std::strcmp(add_command,pch) == 0)
//        if (add_command == pch)
//             *com_num = 0;
//        else if (del_command == pch)
//        else if (std::strcmp(del_command,pch) == 0)
//                *com_num = 1;
//             else if (get_command == pch)
//             else if (std::strcmp(get_command,pch) == 0)
//             {
//                 *com_num = 2;
//                 break;
//             }
//             else if (wait_command == pch)
//             else if (std::strcmp(wait_command,pch) == 0)
//                    *com_num = -1;
//        }
//    case 2:
//        {
//        if (*com_num ==-1)
//            {
//                if (wait_command1 == pch)
//                if (std::strcmp(wait_command1,pch) == 0)
//                     *com_num = 3;
//                else if (wait_command2 == pch)
//                else if (std::strcmp(wait_command2,pch) == 0)
//                        *com_num = 4;
//                     else if (wait_command3 == pch)
//                     else if (std::strcmp(wait_command3,pch) == 0)
//                            *com_num = 5;
//            }
//            else
//            {
//                if (isdigit(*pch))
//                    {
//                        *tmp_N = atoi(pch);
//                        tmp_N_exist = true;
//                        break;
//                    }
//            }
//        }
//    case 3:
//        {
//        if (isdigit(*pch))
//            {
//                *tmp_N = atoi(pch);
//                tmp_N_exist = true;
//            }
//        }
//    }
//    pch = strtok (NULL, "/");
//  }
//    if (*com_num==2)
//    {
//        sprintf(str, "N = %u\n",N);
//        std::cout << str;
//        std::cout << strlen(str) << "\n";
//        return str;
//    }
//    if (tmp_N_exist)
//        switch (*com_num)
//        {
//            case 0:
//                {
//                    boost::recursive_mutex::scoped_lock scoped_lock(mutex);
//                    N= N + *tmp_N;
//                    return done;
//                }
//            case 1:
//                {
//                    boost::recursive_mutex::scoped_lock scoped_lock(mutex);
//                    N= N - *tmp_N;
//                    return done;
//                }
//            case 3:
//                {
//                     for (;;)
//                        {
//                            if (N==*tmp_N)
//                            {
//                                return done;
//                            }
//                        }
//                }
//             case 4:
//                {
//                     for (;;)
//                        {
//                            if (N>*tmp_N)
//                            {
//                                return done;
//                            }
//                        }
//                }
//            case 5:
//                {
//                     for (;;)
//                        {
//                            if (N<*tmp_N)
//                            {
//                                return done;
//                            }
//                        }
//                }
//        }
//    else if (*com_num!=2)
//        *com_num=-2;
//    if (*com_num==-2)
//        //return uncorrect;
//  }
//  catch (std::exception& e)
//  {
//    std::cerr << "Exception in thread: " << e.what() << "\n";
//    return uncorrect;
//  }
//}

bool request_parser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c)
{
  switch (c)
  {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}

} // namespace server
} // namespace http
