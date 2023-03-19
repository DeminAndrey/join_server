#include "data_base.h"

#include <gtest/gtest.h>

TEST(database_test, base_test) {
  my::data_base db;

  // insert to A
  ASSERT_TRUE(db.insert("A", 0, "lean").first);
  ASSERT_TRUE(db.insert("A", 1, "sweater").first);
  ASSERT_TRUE(db.insert("A", 2, "frank").first);
  ASSERT_TRUE(db.insert("A", 3, "violation").first);
  ASSERT_TRUE(db.insert("A", 4, "quality").first);
  ASSERT_TRUE(db.insert("A", 5, "precision").first);
  ASSERT_FALSE(db.insert("A", 0, "understand").first);

  // insert to B
  ASSERT_TRUE(db.insert("B", 3, "proposal").first);
  ASSERT_TRUE(db.insert("B", 4, "example").first);
  ASSERT_TRUE(db.insert("B", 5, "lake").first);
  ASSERT_TRUE(db.insert("B", 6, "flour").first);
  ASSERT_TRUE(db.insert("B", 7, "wonder").first);
  ASSERT_TRUE(db.insert("B", 8, "selection").first);

  // intersection
  std::map<size_t, std::string> intersection = {
    {3, "violation proposal"},
    {4, "quality example"},
    {5, "precision lake"}
  };
  auto s = db.intersection();
//  ASSERT_EQ(intersection, db.intersection());

  // symm_difference
  std::map<size_t, std::string> difference = {
    {0, "lean "},
    {1, "sweater "},
    {2, "frank "},
    {6, " flour"},
    {7, " wonder"},
    {8, " selection"}
  };
  auto d = db.symm_difference();
//  ASSERT_EQ(difference, db.symm_difference());

  // truncate
  ASSERT_TRUE(db.truncate("A").first);
  ASSERT_TRUE(db.truncate("B").first);

  ASSERT_TRUE(db.is_empty());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
