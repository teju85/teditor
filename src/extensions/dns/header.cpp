#include "header.h"
#include <arpa/inet.h>

namespace teditor {
namespace dns {

const int Header::NumFields = sizeof(Header) / sizeof(uint16_t);

Header Header::Query(uint16_t nQuestions) {
  Header h;
  h.rd = 1;
  h.qdcount = nQuestions;
  return h;
}

int Header::serialize(char *buff) {
  auto *src = reinterpret_cast<uint16_t*>(this);
  auto *dst = reinterpret_cast<uint16_t*>(buff);
  for (int i = 0; i < NumFields; ++i) {
    dst[i] = htons(src[i]);
  }
  return sizeof(Header);
}

int Header::deserialize(char *buff) {
  auto *src = reinterpret_cast<uint16_t*>(buff);
  auto *dst = reinterpret_cast<uint16_t*>(this);
  for (int i = 0; i < NumFields; ++i) {
    dst[i] = ntohs(src[i]);
  }
  return sizeof(Header);
}

bool Header::operator==(const Header &that) const {
  return id == that.id && qr == that.qr && opcode == that.opcode &&
    aa == that.aa && tc == that.tc && rd == that.rd && ra == that.ra &&
    z == that.z && rcode == that.rcode && qdcount == that.qdcount &&
    ancount == that.ancount && nscount == that.nscount &&
    arcount == that.arcount;
}

}  // namespace dns
}  // namespace teditor
