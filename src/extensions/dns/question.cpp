#include "question.h"
#include <arpa/inet.h>
#include <algorithm>
#include <cstring>

namespace teditor {
namespace dns {

Question Question::ArecordQuery(const std::string& u) {
  Question q;
  q.url = u;
  q.qtype = 1;
  q.qclass = 1;
  return q;
}

void Question::serialize(char *buff) {
  int len = (int)url.size();
  auto *ptr = url.data();
  const int maxLen = 255;
  for (int i = 0; i < len; i += maxLen) {
    int currLen = std::min(maxLen, len - i);
    buff[0] = (char)currLen;
    std::strncpy(buff + 1, ptr + i, currLen);
    buff += currLen + 1;
  }
  buff[0] = '\0';
  ++buff;
  writeShort(qtype, buff);
  buff += 2;
  writeShort(qclass, buff);
}

void Question::deserialize(char *buff) {
  url.clear();
  while (buff[0] != '\0') {
    int len = buff[0];
    url.append(buff + 1, len);
    buff += len + 1;
  }
  ++buff;  // '0' truncated
  qtype = readShort(buff);
  buff += 2;
  qclass = readShort(buff);
}

int Question::size() const {
  int len = (int)url.size();
  const int maxLen = 255;
  int ret = (len / maxLen) * (maxLen + 1);
  if (len % maxLen) {
    ret += (len % maxLen) + 1;
  }
  ++ret;  // '0' truncated
  ret += 2 * sizeof(uint16_t);
  return ret;
}

bool Question::operator==(const Question &that) const {
  return url == that.url && qtype == that.qtype && qclass == that.qclass;
}

void Question::writeShort(uint16_t val, char *buff) {
  auto net = htons(val);
  auto *ptr = (char*)&net;
  buff[0] = ptr[0];
  buff[1] = ptr[1];
}

uint16_t Question::readShort(char *buff) {
  uint16_t ret;
  auto *ptr = (char*)&ret;
  ptr[0] = buff[0];
  ptr[1] = buff[1];
  return ntohs(ret);
}

}  // namespace dns
}  // namespace teditor
