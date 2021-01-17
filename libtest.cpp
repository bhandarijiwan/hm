#include "fmt/core.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

#include "lib.h"

struct key_a {
  explicit key_a(const char *str) : m_str(str) {
    fmt::print("{}\n", __PRETTY_FUNCTION__);
  }
  std::string m_str;
};

TEST(hm, Insert) {
  lite::hm<int, int> hm_{};
  hm_.insert(20, 90);
  ASSERT_EQ(hm_.exists(20), true);
}

TEST(hm, PreservesOnResize) {
  lite::hm<int, int> hm_{};
  hm_.insert(20, 20);
  hm_.insert(10, 10);
  hm_.insert(30, 30);
  hm_.insert(40, 40);
  hm_.insert(50, 50);
  hm_.insert(60, 60);
  hm_.insert(70, 70);
  hm_.insert(80, 80);
  hm_.insert(10, 11);
  fmt::print(stderr, "printing the hashmap {}\n", hm_);
  // ASSERT_EQ(hm_.exists(40), true);
  // ASSERT_EQ(hm_.exists(70), false);
}