/**
 * @file
 * @brief
 * Simple UDP pair echo server example.
 */
#include "udp_pair.h"

#include <pthread.h>
#include <sys/types.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(
  int argc,
  char** argv)
{
  // Check command line arguments.
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <recv_port> <dest_ip4> <dest_port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Parse command line arguments.
  uint16_t recv_port = (uint16_t)atoi(argv[1]);
  char* dest_ip4 = argv[2];
  uint16_t dest_port = (uint16_t)atoi(argv[3]);

  // Establish UDP pair.
  struct udp_pair* pair = udp_pair_create(recv_port, dest_ip4, dest_port);
  if (!pair) {
    fprintf(stderr, "Could not establish UDP connection\n");
    exit(EXIT_FAILURE);
  }

  char buf[512];
  while (1) {

    // Read data from the UDP pair.
    ssize_t bytes_recvd = 0;
    struct udp_pair_result res =
      udp_pair_recv(pair, (void*)buf, sizeof(buf), &bytes_recvd);
    if (!res.ok) {
      udp_pair_result_fprint(stderr, res);
      break;
    }

    // Send the message over the UDP pair.
    if (bytes_recvd > 0) {
      struct udp_pair_result res =
        udp_pair_send(pair, (void*)buf, bytes_recvd, NULL);
      if (!res.ok) {
        udp_pair_result_fprint(stderr, res);
        exit(EXIT_FAILURE);
      }
    }
  }

  udp_pair_free(pair);

  exit(EXIT_SUCCESS);
}
