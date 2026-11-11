#include "http.hpp"
int main(int argc, char **argv) {
  try {
    int port = 18741;
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "-h" || arg == "--help") {
        std::cout << "usage: frame_debug_agent [-h] [--port PORT]\n\nRead-only "
                     "Linux diagnostic endpoint for the Frame research "
                     "workspace.\n\noptions:\n  -h, --help   show this help "
                     "message and exit\n  --port PORT\n";
        return 0;
      }
      std::string value;
      if (arg == "--port" && i + 1 < argc)
        value = argv[++i];
      else if (arg.rfind("--port=", 0) == 0)
        value = arg.substr(7);
      else {
        std::cerr << "unrecognized arguments: " << arg << '\n';
        return 2;
      }
      size_t used = 0;
      try {
        port = std::stoi(value, &used);
        if (used != value.size())
          throw std::invalid_argument("port");
      } catch (const std::exception &) {
        std::cerr << "argument --port: invalid int value: '" << value << "'\n";
        return 2;
      }
    }
    if (port < 0 || port > 65535)
      throw std::runtime_error("port must be 0-65535");
    frame::Agent agent;
    frame::Server server(agent, static_cast<unsigned short>(port));
    server.serve();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
