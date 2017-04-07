/**
 * @file
 * @brief
 * Defines a simple wrapper interface for a UDP "connection". UDP is
 * connectionless but sometimes you want to treat it like a connection.
 */
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
 * udp_conn_result_fprint() can be used to quickly printing the result as an
 * error message to a file stream.
 */
struct udp_conn_result {
  int ok;
  int err_num;
  const char* err_msg;
  const char* func_name;
};

/**
 * @brief
 * Convenience function for printing the given result to the specified stream
 * in a simple format (for example: <tt>udp_conn_recv_nonblock() error:
 * Connection refused</tt>).
 */
static inline void udp_conn_result_fprint(
  FILE* stream,
  const struct udp_conn_result res)
{
  if (!res.ok) {
    fprintf(stream, "%s() error: %s\n", res.func_name, res.err_msg);
  }
}

/**
 * @brief
 * Opaque type that represnts a UDP "connection".
 */
struct udp_conn;

/**
 * @brief
 * Creates a new UDP connection object.
 *
 * Allocates the object on the heap using malloc and constructing it using @ref
 * udp_conn_construct().
 */
struct udp_conn* udp_conn_create(
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port);

/**
 * @brief
 * Frees an object created using @ref udp_conn_create().
 *
 * Basically calls @ref udp_conn_destruct() on the object and the frees the
 * heap allocated object.
 */
void udp_conn_free(
  struct udp_conn* self);

/**
 * @brief
 * Constructs and initializes a UDP connection object "in place".
 *
 * This assumes that your object memory already exists and you want to
 * initialize the object at the given memory location.
 */
struct udp_conn_result udp_conn_construct(
  struct udp_conn* self,
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port);

/**
 * @brief
 * Cleans up the UDP connection.
 *
 * Just calls @ref close() on the socket.
 */
void udp_conn_destruct(
  struct udp_conn* self);

/**
 * @brief
 * Gets the file descriptor for the underlying socket.
 */
int udp_conn_get_fd(
  const struct udp_conn* self);

/**
 * @brief
 * Sends data in a blocking manner.
 *
 * @param bytes_sent_out
 * is set to the number of bytes sent if result is ok, can be set to NULL
 */
struct udp_conn_result udp_conn_send(
  struct udp_conn* self,
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
struct udp_conn_result udp_conn_recv(
  struct udp_conn* self,
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
struct udp_conn_result udp_conn_send_nonblock(
  struct udp_conn* self,
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
struct udp_conn_result udp_conn_recv_nonblock(
  struct udp_conn* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out);

#ifdef __cplusplus
} // extern "C"
#endif
