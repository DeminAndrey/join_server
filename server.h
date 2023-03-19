#pragma once

#include "session.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace Response {
static const std::string OK = "OK";
static const std::string ERR = "ERR";
static const std::string EMPTY = " empty request";
static const std::string DUPLICATE_ERR = " duplicate ";
} // namespace Response

enum RequestType {
  Table_name = 0,
  Id = 1,
  Name = 2
};

class data_handler {

public:
  static void on_connection_accept(
      std::unique_ptr<connection_with_data> &&connection,
      const boost::system::error_code &error) {
    assert(!error);
    async_read_data_at_least(std::move(connection),
                             &data_handler::on_datarecieve, 1, 1024);
  }

  static void on_datarecieve(
      std::unique_ptr<connection_with_data> &&connection,
      const boost::system::error_code& error) {
    if (error) {
      std::cerr << "handler_.on_datarecieve: error during recieving response: "
                << error << '\n';
      assert(false);
    }

    if (connection->data.size() == 0) {
      std::cerr << "handler_.on_datarecieve: zero bytes recieved\n";
      assert(false);
    }

    const auto &request = connection->data;
    auto response = handle_request(connection->handler, request);
    connection->data = std::move(response);
    async_write_data(std::move(connection), &data_handler::on_datasend);
  }

  static void on_datasend(std::unique_ptr<connection_with_data>&& connection,
                          const boost::system::error_code& error) {
    if (error) {
      std::cerr << "handler_.on_datasend: error during sending response: "
                << error << '\n';
      assert(false);
    }

    connection->shutdown();
  }

private:
  static std::string handle_request(my::data_base &handler,
                                    const std::string &request) {
    auto request_ = request;
    boost::algorithm::trim(request_);
    std::string response;
    if (request_.empty()) {
      response.append(Response::ERR).append(Response::EMPTY);
    }
    else if (request == protocol::INTERSECTION) {
      auto result = handler.intersection();
      response = lexical_cast(result);
    }
    else if (request == protocol::SYMMETRIC_DIFFERENCE) {
      auto result = handler.symm_difference();
      response = lexical_cast(result);
    }
    else if (request.find(protocol::TRUNCATE) != std::string::npos) {
      auto results = parse_request(request);
      if (results.empty()) {
        response.append(Response::ERR).append(Response::EMPTY);
      }
      else {
        auto [success, msg] = handler.truncate(results[(size_t)Table_name]);
        response = success ? Response::OK
                           : Response::ERR + " " + msg;
      }
    }
    else if (request.find(protocol::INSERT) != std::string::npos) {
      auto results = parse_request(request);
      if (results.size() != 3) {
        response = Response::ERR;
      }
      else {
        auto table_name = results[(size_t)Table_name];
        auto id = std::atoi(results[(size_t)Id].c_str());
        auto name = results[(size_t)Name];

        auto [success, msg] = handler.insert(table_name, id, name);
        response = success ? Response::OK
                           : Response::ERR + msg;
      }
    }
    else {
      response = Response::ERR;
    }

    return response;
  }

  static std::vector<std::string> parse_request(
      const std::string &request) {
    std::vector<std::string> results;
    boost::algorithm::split(
          results, request, boost::algorithm::is_any_of(" "),
          boost::token_compress_on);

    return {results.begin() + 1, results.end()};
  }

  static std::string lexical_cast(
      const std::map<size_t, std::string> &data) {
    std::string result;
    for (const auto &row : data) {
      result.append(std::to_string(row.first))
            .append(" ")
            .append(row.second)
            .append("/n");
    }
    return result;
  }
};
