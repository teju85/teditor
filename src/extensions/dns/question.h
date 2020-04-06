#pragma once

#include <string>
#include <stdint.h>

namespace teditor {
namespace dns {

/**
 * @brief Contains the DNS question
 *
 * Ref: https://tools.ietf.org/html/rfc1035#page-26
 */
struct Question {
  Question() : url(), qtype(0), qclass(0) {
  }

  /**
   * @brief Serialize header contents to the network
   * @param buff buffer which will be transmitted out
   * @return number of bytes written to this buffer
   */
  int serialize(char *buff);

  /**
   * @brief Deserialize header contents from the network
   * @param buff buffer which has been received
   * @return number of bytes read from this buffer
   */
  int deserialize(char *buff);

  bool operator==(const Question &that) const;

  std::string url;
  uint16_t qtype;
  uint16_t qclass;
};  // struct Question

}  // namespace dns
}  // namespace teditor
