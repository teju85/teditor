#include "question.h"
#include <arpa/inet.h>

namespace teditor {
namespace dns {

Question Question::ArecordQuery(const std::string& u) {
  Question q;
  q.url = u;
  q.qtype = 1;
  q.qclass = 1;
  return q;
}

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
