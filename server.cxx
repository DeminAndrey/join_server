#include "server.h"

int main(int argc, char *argv[]) {
  if (argc != 1) {
    std::cerr << "Wrong parameters to run!" << std::endl;

    return 1;
  }
  auto port = static_cast<unsigned>(std::atoi(argv[0]));

  task_processor::add_listener(
        port, &data_handler::on_connection_accept);
  task_processor::start();

  return 0;
}
