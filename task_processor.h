#pragma once

#include "data_base.h"

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

struct connection_with_data : boost::noncopyable {
  boost::asio::ip::tcp::socket socket;
  std::string data;
  my::data_base handler;

  explicit connection_with_data(boost::asio::io_context &ioc)
    : socket(ioc) {}

  void shutdown() {
    boost::system::error_code ignore;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignore);
    socket.close(ignore);
  }

  ~connection_with_data() {
    shutdown();
  }
};

template<typename T>
struct task_wrapped_with_conection {

private:
  std::unique_ptr<connection_with_data> connect_;
  T task_unwrapped_;

public:
  explicit task_wrapped_with_conection(
      std::unique_ptr<connection_with_data> &&connect, const T &f)
    : connect_(std::move(connect)),
      task_unwrapped_(f) {}

  void operator()(const boost::system::error_code &code, size_t bytes_count) {
    connect_->data.resize(bytes_count);
    task_unwrapped_(std::move(connect_), code);
  }
};

class task_processor {
  typedef boost::asio::ip::tcp::acceptor acceptor_t;
  typedef boost::function<void(std::unique_ptr<connection_with_data>,
                               const boost::system::error_code &)> on_accept_func_t;

  struct tcp_listener {
    acceptor_t acceptor_;
    const on_accept_func_t func_;
    std::unique_ptr<connection_with_data> new_c_;

    template<typename Functor>
    tcp_listener(boost::asio::io_context &io_context,
                 unsigned short port,
                 const Functor &task_unwrapped)
      : acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::tcp::v4(), port)),
        func_(task_unwrapped) {
    }
  };

  struct handle_accept {
    std::unique_ptr<tcp_listener> listener;

    explicit handle_accept(std::unique_ptr<tcp_listener> &&l)
      : listener(std::move(l)) {
    }

    void operator()(const boost::system::error_code &error) {
      task_wrapped_with_conection<on_accept_func_t> task(
            std::move(listener->new_c_), listener->func_);

      start_accepting_connection(std::move(listener));
      task(error, 0);
    }
  };

protected:
  static boost::asio::io_context &get_ioc() {
    static boost::asio::io_context ioc;
    static boost::asio::io_context::work work(ioc);

    return ioc;
  }

public:
  static void start() {
    get_ioc().run();
  }

  static void stop() {
    get_ioc().stop();
  }

  static std::unique_ptr<connection_with_data> create_connection(
      const char *addr, unsigned short port_num) {
    std::unique_ptr<connection_with_data> connect(
          new connection_with_data(get_ioc()));
    connect->socket.connect(
          boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address_v4::from_string(addr), port_num));

    return connect;
  }

  template<typename Functor>
  static void add_listener(unsigned port_num,
                           const Functor &f) {
    std::unique_ptr<tcp_listener> listener(
          new tcp_listener(get_ioc(), port_num, f));

    start_accepting_connection(std::move(listener));
  }

private:
  static void start_accepting_connection(
      std::unique_ptr<tcp_listener> && listener) {
    if(!listener->acceptor_.is_open()) {
      return;
    }
    listener->new_c_.reset(new connection_with_data(get_ioc()));

    boost::asio::ip::tcp::socket &s = listener->new_c_->socket;
    acceptor_t &a = listener->acceptor_;
    a.async_accept(s, task_processor::handle_accept(std::move(listener)));
  }
};
