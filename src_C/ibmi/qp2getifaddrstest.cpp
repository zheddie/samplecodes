#include <stdio.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <as400_protos.h>

int main(void)
{
  struct ifaddrs_pase *ifap = NULL, *cur;
  int inet6, r = 0;
  char ipaddr[INET_ADDRSTRLEN];
  if (Qp2getifaddrs(&ifap))
    return 1;  /* The second loop to fill in the array */
  for (cur = ifap; cur; cur = cur->ifa_next) {
    if (!(cur->ifa_addr->sa_family == AF_INET6 ||
          cur->ifa_addr->sa_family == AF_INET))
      continue;
    // if (!(cur->ifa_flags & IFF_UP && cur->ifa_flags & IFF_RUNNING))
    //   continue;
    printf("Name: %s\n", cur->ifa_name);
    inet6 = (cur->ifa_addr->sa_family == AF_INET6);
    if (inet6) {
      inet_ntop(AF_INET6, &((struct sockaddr_in6*)cur->ifa_addr)->sin6_addr, ipaddr, INET_ADDRSTRLEN);
      printf("IPv6: %s\n", ipaddr);
    } else {
      inet_ntop(AF_INET, &((struct sockaddr_in*)cur->ifa_addr)->sin_addr, ipaddr, INET_ADDRSTRLEN);
      printf("IPv4: %s\n", ipaddr);
    }
    printf("Flags: %x\n", cur->ifa_flags);
  }
  Qp2freeifaddrs(ifap);
  return r;
}