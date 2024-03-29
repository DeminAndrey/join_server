#include "data_base.h"

#include <iterator>
#include <stdexcept>
#include <vector>

const std::string table_a = "A";
const std::string table_b = "B";

namespace msg {
const std::string ok = "OK";
const std::string unknown_name = "unknown table name";
}

using namespace my;

bool data_base::table::insert_row(size_t id, const std::string &name) {
  if (m_table.count(id)) {
    std::string error = "duplicate " + std::to_string(id);
    throw std::invalid_argument(error);
  }
  else {
    m_table[id] = name;
  }
}

void data_base::table::truncate() {
  m_table.clear();
}

std::string data_base::table::name(size_t id) const {
  return m_table.at(id);
}

auto data_base::table::begin() const {
  return m_table.begin();
}

auto data_base::table::end() const {
  return m_table.end();
}

data_base::data_base() {
  m_data[table_a];
  m_data[table_b];
}

std::pair<bool, std::string> data_base::insert(
    const std::string &table_name,
    size_t id, const std::string &name) {
  try {
    m_data.at(table_name).insert_row(id, name);
  }
  catch (const std::out_of_range &err) {
    return std::make_pair(false, msg::unknown_name);
  }
  catch (const std::invalid_argument &err) {
    return std::make_pair(false, std::string(err.what()));
  }
  return std::make_pair(true, msg::ok);
}

std::pair<bool, std::string> data_base::truncate(
    const std::string &table_name) {
  try {
    m_data.at(table_name).truncate();
  }
  catch (const std::out_of_range &err) {
    return std::make_pair(false, msg::unknown_name);
  }
  return std::make_pair(true, msg::ok);
}

std::map<size_t, std::string> data_base::intersection() const {
  if (is_empty()) {
    return {};
  }
  std::vector<std::pair<size_t, std::string>> intersection;
  std::set_intersection(m_data.at(table_a).begin(), m_data.at(table_a).end(),
                        m_data.at(table_b).begin(), m_data.at(table_b).end(),
                        std::back_inserter(intersection),
                        [](const auto &lhs, const auto &rhs) {
    return lhs.first < rhs.first;
  });
  std::set_intersection(m_data.at(table_b).begin(), m_data.at(table_b).end(),
                        m_data.at(table_a).begin(), m_data.at(table_a).end(),
                        std::back_inserter(intersection),
                        [](const auto &lhs, const auto &rhs) {
    return lhs.first < rhs.first;
  });

  std::map<size_t, std::string> result;
  for (const auto &i : intersection) {
    auto str = result.count(i.first) ?
          " " + i.second :
          i.second;
    result[i.first].append(str);
  }

  return result;
}

std::map<size_t, std::string> data_base::symm_difference() const {
  if (is_empty()) {
    return {};
  }

  std::vector<std::pair<size_t, std::string>> difference;
  std::set_symmetric_difference(m_data.at(table_a).begin(), m_data.at(table_a).end(),
                                m_data.at(table_b).begin(), m_data.at(table_b).end(),
                                std::back_inserter(difference),
                                [](const auto &lhs, const auto &rhs) {
    return lhs.first < rhs.first;
  });

  std::map<size_t, std::string> result;
  for (const auto &d : difference) {
    result.emplace(d);
  }

  return result;
}

bool data_base::is_empty() const {
  return m_data.empty();
}
