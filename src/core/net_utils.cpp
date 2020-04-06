#include "net_utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sstream>
#include "utils.h"

namespace teditor {

std::string dnsLookup(const std::string& url, const std::string& domain) {
  struct addrinfo *res, hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  int flag = getaddrinfo(url.c_str(), domain.c_str(), &hints, &res);
  ASSERT(flag == 0, "dnsLookup failed! flag=%d getaddrinfo=%s\n", flag,
         gai_strerror(flag));
  std::stringstream oss;
  for(auto *p = res; p != NULL; p = p->ai_next) {
    auto *sa = p->ai_addr;
    const int maxlen = 2048;
    char s[maxlen];
    s[0]= '\0';
    switch(sa->sa_family) {
    case AF_INET:
      oss << "IPv4: ";
      inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen);
      oss << s;
      break;
    case AF_INET6:
      oss << "IPv6: ";
      inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen);
      oss << s;
      break;
    default:
      oss << "Bad IP version!";
    };
  }
  freeaddrinfo(res);
  return oss.str();
}

}  // namespace teditor
