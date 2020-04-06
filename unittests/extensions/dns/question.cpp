#include "extensions/dns/question.h"
#include "catch.hpp"

namespace teditor {
namespace dns {

TEST_CASE("Question::Conversion") {
  auto ref = Question::ArecordQuery("teju85.github.io");
  REQUIRE(ref.size() == 22);
  char *buff = new char[ref.size()];
  ref.serialize(buff);
  Question obtained;
  obtained.deserialize(buff);
  REQUIRE(obtained == ref);
  delete [] buff;
}

} // end namespace dns
} // end namespace teditor
