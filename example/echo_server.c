/**
 * @file
 * @brief
 * Simple UDP connection echo server example.
 */
#include "udp_conn.h"

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

  // Establish UDP connection.
  struct udp_conn* conn = udp_conn_create(recv_port, dest_ip4, dest_port);
  if (!conn) {
    fprintf(stderr, "Could not establish UDP connection\n");
    exit(EXIT_FAILURE);
  }

  char buf[512];
  while (1) {

    // Read data from the UDP connection.
    ssize_t bytes_recvd = 0;
    struct udp_conn_result res =
      udp_conn_recv(conn, (void*)buf, sizeof(buf), &bytes_recvd);
    if (!res.ok) {
      udp_conn_result_fprint(stderr, res);
      break;
    }

    // Send the message over the UDP connection.
    if (bytes_recvd > 0) {
      struct udp_conn_result res =
        udp_conn_send(conn, (void*)buf, bytes_recvd, NULL);
      if (!res.ok) {
        udp_conn_result_fprint(stderr, res);
        exit(EXIT_FAILURE);
      }
    }
  }

  udp_conn_free(conn);

  exit(EXIT_SUCCESS);
}
