#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <boost/noncopyable.hpp>

namespace http {
namespace server {

struct reply;
struct request;
//Global
extern size_t N;

extern size_t max_length;

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler();

  /// Handle a request and produce a reply.
  /// and DELETE rep.buff
  void handle_request(request& req, reply& rep);

private:
/// Parse command between GET and HTTP...
std::string parse_command(request& req);

  //// Perform URL-decoding on a string. Returns false if the encoding was
  //// invalid.
  //static bool url_decode(const std::string& in, std::string& out);
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
