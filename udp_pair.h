/**
 * @file
 * @brief
 * Declares a simple wrapper interface for a UDP pair where you have two UDP
 * end points talking directly to each other.
 */
#pragma once

#ifndef UDP_CONN_UDP_CONN_H
#define UDP_CONN_UDP_CONN_H

#include <sys/types.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * A convenience struct that encapsulates function results. To check if result
 * is good simply check the @ref ok field. If that is @p false (0) then @ref
 * err_num, @ref err_msg, and @ref func_name should be populated. @ref
 * udp_pair_result_fprint() can be used to quickly printing the result as an
 * error message to a file stream.
 */
struct udp_pair_result {
  int ok;
  int err_num;
  const char* err_msg;
  const char* func_name;
};

/**
 * @brief
 * Convenience function for printing the given result to the specified stream
 * in a simple format (for example: <tt>udp_pair_recv_nonblock() error:
 * Connection refused</tt>).
 */
static inline void udp_pair_result_fprint(
  FILE* stream,
  const struct udp_pair_result res)
{
  if (!res.ok) {
    fprintf(stream, "%s error: %s\n", res.func_name, res.err_msg);
  }
}

/**
 * @brief
 * Opaque type that represents a UDP pair.
 */
struct udp_pair;

/**
 * @brief
 * Creates a new UDP pair object.
 *
 * Allocates the object on the heap using malloc and constructing it using @ref
 * udp_pair_construct().
 */
struct udp_pair* udp_pair_create(
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port);

/**
 * @brief
 * Frees an object created using @ref udp_pair_create().
 *
 * Basically calls @ref udp_pair_destruct() on the object and the frees the
 * heap allocated object.
 */
void udp_pair_free(
  struct udp_pair* self);

/**
 * @brief
 * Constructs and initializes a UDP pair object "in place".
 *
 * This assumes that your object memory already exists and you want to
 * initialize the object at the given memory location.
 */
struct udp_pair_result udp_pair_construct(
  struct udp_pair* self,
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port);

/**
 * @brief
 * Cleans up the UDP pair by calling @ref udp_pair_shutdown().
 */
void udp_pair_destruct(
  struct udp_pair* self);

/**
 * @brief
 * Shutdown and close the underlying socket so that blocking reads/writes are
 * interrupted.
 *
 * Calls @ref shutdown() and @ref close() on the socket.
 */
void udp_pair_shutdown(
  struct udp_pair* self);

/**
 * @brief
 * Gets the file descriptor for the underlying socket.
 */
int udp_pair_get_fd(
  const struct udp_pair* self);

/**
 * @brief
 * Sends data in a blocking manner.
 *
 * @param bytes_sent_out
 * is set to the number of bytes sent if result is ok, can be set to NULL
 */
struct udp_pair_result udp_pair_send(
  struct udp_pair* self,
  const void* buf,
  const size_t size,
  ssize_t* bytes_sent_out);

/**
 * @brief
 * Receives data in a blocking manner.
 *
 * @param bytes_recvd_out
 * is set to the number of bytes received if result is ok, can be set to NULL
 */
struct udp_pair_result udp_pair_recv(
  struct udp_pair* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out);

/**
 * @brief
 * Sends data in a non-blocking manner. If data is not sent simply because of
 * the non-blocking behavior then the result will be ok and @ref bytes_sent_out
 * will be set to 0.
 *
 * @param bytes_sent_out
 * is set to the number of bytes sent if result is ok, can be set to NULL
 */
struct udp_pair_result udp_pair_send_nonblock(
  struct udp_pair* self,
  const void* buf,
  const size_t size,
  ssize_t* bytes_sent_out);

/**
 * @brief
 * Receives data in a non-blocking manner. If data is not received simply
 * because of the non-blocking behavior then the result will be ok and @ref
 * bytes_recvd_out will be set to 0.
 *
 * @param bytes_recvd_out
 * is set to the number of bytes received if result is ok, can be set to NULL
 */
struct udp_pair_result udp_pair_recv_nonblock(
  struct udp_pair* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // UDP_CONN_UDP_CONN_H
