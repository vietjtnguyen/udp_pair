/**
 * @file
 * @brief
 * Simple UDP pair example that behaves like the cat utility.
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


int g_alive = 1;


void* read_thread_func(
  void* thread_data)
{
  struct udp_pair* pair = (struct udp_pair*)thread_data;
  char buf[512];
  while (g_alive) {

    // Read data from the UDP pair.
    ssize_t bytes_recvd = 0;
    struct udp_pair_result res =
      udp_pair_recv(pair, (void*)buf, sizeof(buf), &bytes_recvd);
    if (!res.ok) {
      udp_pair_result_fprint(stderr, res);
      break;
    }

    // Write whatever data we got from the UDP connection to stdout.
    if (bytes_recvd > 0) {
      ssize_t bytes_written = write(1, buf, bytes_recvd);
      if (bytes_written == -1) {
        break;
      }
    }
  }

  return NULL;
}


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
    fprintf(stderr, "Could not establish UDP pair\n");
    exit(EXIT_FAILURE);
  }

  // Start read thread.
  pthread_t read_thread;
  if (pthread_create(&read_thread, NULL, read_thread_func, (void*)pair) != 0) {
    fprintf(stderr, "pthread_join() failed\n");
    exit(EXIT_FAILURE);
  }

  char buf[512];
  while (g_alive) {

    // Read data from stdin.
    ssize_t bytes_read = read(0, buf, sizeof(buf));
    if (bytes_read == 0 || bytes_read == -1) {
      break;
    }

    // Send the data we read from stdin over the UDP pair.
    struct udp_pair_result res =
      udp_pair_send(pair, (void*)buf, bytes_read, NULL);
    if (!res.ok) {
      udp_pair_result_fprint(stderr, res);
      exit(EXIT_FAILURE);
    }
  }

  // Make sure the read thread dies too.
  g_alive = 0;
  udp_pair_shutdown(pair);

  // Join the read thread.
  if (pthread_join(read_thread, NULL) != 0) {
    fprintf(stderr, "pthread_join() failed\n");
    exit(EXIT_FAILURE);
  }
 
  // Clean up heap allocated memory.
  udp_pair_free(pair);

  exit(EXIT_SUCCESS);
}
