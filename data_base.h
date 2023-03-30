#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <optional>

namespace Protocol {
static const std::string INSERT = "INSERT";
static const std::string TRUNCATE = "TRUNCATE";
static const std::string INTERSECTION = "INTERSECTION";
static const std::string SYMMETRIC_DIFFERENCE = "SYMMETRIC_DIFFERENCE";
} // namespace Protocol

namespace my {

class data_base {

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

  std::map<std::string, table> m_data;

public:
  data_base();

  /**
   * @brief операция вставки нового элемента в таблицу
   * @param table_name таблица, в которую необходимо добавить новый элемент
   * @param id индивидуальный ключ нового элемента (не дублируется)
   * @param name наименование нового элемента
   * @return возвращает результат вставки, bool - добавлен/не добавлен,
   * если успешно добавлен, второй элемент (string) - пустая строка,
   *  если нет - то сообщение об ошибке
   */
  std::pair<bool, std::string> insert(
      const std::string &table_name,
      size_t id, const std::string &name);

  /**
   * @brief операция очистки соответсвующей таблицы
   * @param table_name наименование таблицы
   * @return результат очистки, bool - очищена/не очищена,
   * если успешно очищена, второй элемент (string) - пустая строка,
   *  если нет - то сообщение об ошибке
   */
  std::pair<bool, std::string> truncate(
      const std::string &table_name);

  /**
   * @brief операция поиска пересечений элементов в таблице
   * @return результат выполнения
   */
  std::map<size_t, std::string> intersection() const;

  /**
   * @brief операция посика симметричной разности элементов
   * в таблице
   * @return результат выполнения
   */
  std::map<size_t, std::string> symm_difference() const;

private:
  bool is_empty() const;
};
} // namespace my
