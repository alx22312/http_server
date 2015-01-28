#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
  try
  {
//    // Check command line arguments.
//    if (argc != 4)
//    {
//      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
//      std::cerr << "  For IPv4, try:\n";
//      std::cerr << "    receiver 0.0.0.0 80 .\n";
//      std::cerr << "  For IPv6, try:\n";
//      std::cerr << "    receiver 0::0 80 .\n";
//      return 1;
//    }
    std::string def_port = "2015";
    // Initialise the server.
    if(argv[1] != NULL)
        def_port = argv[1];
    http::server::server s(def_port);
    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
