#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <optional>

namespace protocol {
static const std::string INSERT = "INSERT";
static const std::string TRUNCATE = "TRUNCATE";
static const std::string INTERSECTION = "INTERSECTION";
static const std::string SYMMETRIC_DIFFERENCE = "SYMMETRIC_DIFFERENCE";

enum request_type {
  INSERT_,
  TRUNCATE_,
  INTERSECTION_,
  SYMMETRIC_DIFFERENCE_
};

static const std::map<std::string, request_type> request {
  {INSERT, request_type::INSERT_},
  {TRUNCATE, request_type::TRUNCATE_},
  {INTERSECTION, request_type::INTERSECTION_},
  {SYMMETRIC_DIFFERENCE, request_type::SYMMETRIC_DIFFERENCE_}
};
} // namespace protocol

class table {
  std::map<size_t, std::string> m_table;

public:
  table() = default;

  bool insert_row(size_t id, const std::string &name);

  void truncate();

  std::string name(size_t id) const;

  auto begin() const;

  auto end() const;
};

namespace my {

class data_base {
  std::map<std::string, table> m_data;

public:
  data_base();

  /**
   * @brief insert
   * @param table_name
   * @param id
   * @param name
   * @return
   */
  std::pair<bool, std::string> insert(
      const std::string &table_name,
      size_t id, const std::string &name);

  /**
   * @brief truncate
   * @param table_name
   * @return
   */
  std::pair<bool, std::string> truncate(
      const std::string &table_name);

  /**
   * @brief intersection
   * @return
   */
  std::map<size_t, std::string> intersection() const;

  /**
   * @brief symm_difference
   * @return
   */
  std::map<size_t, std::string> symm_difference() const;

  /**
   * @brief is_empty
   * @return
   */
  bool is_empty() const;
};
} // namespace my
