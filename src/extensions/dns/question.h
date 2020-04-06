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
   * @brief Helper method to construct A-record queries
   * @param u the url whose IP needs to be queried
   * @return the question
   */
  static Question ArecordQuery(const std::string& u);

  /**
   * @brief Serialize header contents to the network
   * @param buff buffer which will be transmitted out
   */
  void serialize(char *buff);

  /**
   * @brief Deserialize header contents from the network
   * @param buff buffer which has been received
   */
  void deserialize(char *buff);

  int size() const;

  bool operator==(const Question &that) const;

  std::string url;
  uint16_t qtype;
  uint16_t qclass;

 private:
  void writeShort(uint16_t val, char *buff);
  uint16_t readShort(char *buff);
};  // struct Question

}  // namespace dns
}  // namespace teditor
