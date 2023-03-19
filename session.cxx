#include "session.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

static const char *HELP = "help";
static const char *PORT = "port";
static const char *RECEIVE = "rec";

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Not enough parameters to run!" << std::endl;

    return 1;
  }

  po::options_description opt_desc("Allowed options");
  opt_desc.add_options()
      (HELP,                                "Print this message")
      (RECEIVE, po::value<std::string>(),   "Expression to send to the server")
      (PORT, po::value<std::size_t>()->required(), "Server port");

  po::variables_map var_map;
  try {
    auto parsed = po::command_line_parser(argc, argv)
        .options(opt_desc)
        .run();
    po::store(parsed, var_map);
    if (var_map.count(HELP)) {
      std::cout << opt_desc << "\n";

      return 0;
    }
    if (!var_map.count(RECEIVE)) {
      std::cerr << "Not enough parameters to send to server\n"
                << "Please use --help to see help message\n";

      return 1;
    }
    po::notify(var_map);
  }
  catch (const po::error &err) {
    std::cerr << "Error while parsing command-line arguments: "
              << err.what() << "\nPlease use --help to see help message\n";
    return 1;
  }


  auto buffer = var_map[RECEIVE].as<std::string>();
  auto port = var_map[PORT].as<unsigned>();

  if (!buffer.empty()) {
    auto connection = session::create_connection(port);
    session::send_data(connection, buffer);
  }

  return 0;
}
