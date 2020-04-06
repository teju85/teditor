#pragma once

#include <cstring>
#include <stdint.h>

namespace teditor {
namespace dns {

/**
 * @brief Contains the DNS message header
 *
 * Ref: https://tools.ietf.org/html/rfc1035#page-26
 */
struct Header {
  Header() : id(0), qr(0), opcode(0), aa(0), tc(0), rd(0), ra(0), z(0),
             rcode(0), qdcount(0), ancount(0), nscount(0), arcount(0) {
  }

  static Header Query(uint16_t nQuestions = 1);

  int serialize(char *buff);

  int deserialize(char *buff);

  bool operator==(const Header &that) const;

  uint16_t id;
  uint8_t qr     :1;
  uint8_t opcode :4;
  uint8_t aa     :1;
  uint8_t tc     :1;
  uint8_t rd     :1;
  uint8_t ra     :1;
  uint8_t z      :3;
  uint8_t rcode  :4;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;

  static const int NumFields;
};  // struct Header

}  // namespace dns
}  // namespace teditor
