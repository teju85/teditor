#include "question.h"
#include <arpa/inet.h>

namespace teditor {
namespace dns {

int Question::serialize(char *buff) {
  return 0;
}

int Question::deserialize(char *buff) {
  return 0;
}

bool Question::operator==(const Question &that) const {
  return url == that.url && qtype == that.qtype && qclass == that.qclass;
}

}  // namespace dns
}  // namespace teditor
