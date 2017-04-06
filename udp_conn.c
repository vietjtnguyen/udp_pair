/**
 * @file
 * @brief
 * Defines the UDP connection type and its various associated functions.
 */
#include "udp_conn.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static struct udp_conn_result ok_result = {1, 0, NULL};


static
struct udp_conn_result
udp_conn_create_result_from_errno_with_func_name(
  const char* func_name)
{
  struct udp_conn_result result;
  result.ok = (errno == 0);
  result.err_num = errno;
  result.err_msg = strerror(errno);
  result.func_name = func_name;
  return result;
}


#define CREATE_ERRNO_RESULT() \
  (udp_conn_create_result_from_errno_with_func_name(__PRETTY_FUNCTION__))


struct udp_conn {
  int sock;
  struct sockaddr_in recv_addr;
  struct sockaddr_in dest_addr;
};


struct udp_conn*
udp_conn_create(
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port)
{
  struct udp_conn* self = malloc(sizeof(struct udp_conn));
  struct udp_conn_result res = udp_conn_construct(
    self, recv_port, dest_ip4, dest_port);
  if (!res.ok) {
    return NULL;
  }
  return self;
}


void
udp_conn_free(
  struct udp_conn* self)
{
  udp_conn_destruct(self);
  free(self);
}


struct udp_conn_result
udp_conn_construct(
  struct udp_conn* self,
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port)
{
  // Create the socket and save the file descriptor. Does not do anything else
  // like binding to an endpoint, connecting to something, handshaking,
  // listening, etc.
  self->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (self->sock == -1) {
    return CREATE_ERRNO_RESULT();
  }

  int ret;
  int enable = 1;

  ret = setsockopt(
    self->sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }

  // Define our received address struct which is used for binding the socket
  // for receiving UDP messages directed at this address. The address 0.0.0.0
  // means it will received messages from all network interfaces.
  memset(&(self->recv_addr), 0, sizeof(self->recv_addr));
  self->recv_addr.sin_family = AF_INET;
  self->recv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
  self->recv_addr.sin_port = htons(recv_port);

  // Define our destination address struct which is used for "connecting" the
  // socket to a destination end point.
  memset(&(self->dest_addr), 0, sizeof(self->dest_addr));
  self->dest_addr.sin_family = AF_INET;
  self->dest_addr.sin_addr.s_addr = inet_addr(dest_ip4);
  self->dest_addr.sin_port = htons(dest_port);

  // Even though UDP is connectionless we bind our socket to the receive
  // address so that the kernel knows that this process wants UDP packets going
  // to that receive address. This allows us to just use the read() or recv()
  // API instead of recvfrom().
  ret = bind(
    self->sock,
    (const struct sockaddr*)(&self->recv_addr),
    sizeof(self->recv_addr));
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }

  // UDP is connectionless but if we call connect() on a UDP socket it lets us
  // use just the write() or send() API instead of sendto().
  ret = connect(
    self->sock,
    (const struct sockaddr*)(&self->dest_addr),
    sizeof(self->dest_addr));
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }

  return ok_result;
}


void
udp_conn_destruct(
  struct udp_conn* self)
{
  shutdown(self->sock, SHUT_RDWR);
  close(self->sock);
}


int
udp_conn_get_fd(
  const struct udp_conn* self)
{
  return self->sock;
}


struct udp_conn_result
udp_conn_send(
  struct udp_conn* self,
  const void* buf,
  const size_t size,
  ssize_t* bytes_sent_out)
{
  ssize_t ret = send(self->sock, buf, size, 0);
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }
  bytes_sent_out && (*bytes_sent_out = ret);
  return ok_result;
}


struct udp_conn_result
udp_conn_recv(
  struct udp_conn* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out)
{
  ssize_t ret = recv(self->sock, buf, size, 0);
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }
  bytes_recvd_out && (*bytes_recvd_out = ret);
  return ok_result;
}


struct udp_conn_result
udp_conn_send_nonblock(
  struct udp_conn* self,
  const void* buf,
  const size_t size,
  ssize_t* bytes_sent_out)
{
  ssize_t ret = send(self->sock, buf, size, MSG_DONTWAIT);
  if (ret == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      return CREATE_ERRNO_RESULT();
    }
    bytes_sent_out && (*bytes_sent_out = 0);
  } else {
    bytes_sent_out && (*bytes_sent_out = ret);
  }
  return ok_result;
}


struct udp_conn_result
udp_conn_recv_nonblock(
  struct udp_conn* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out)
{
  ssize_t ret = recv(self->sock, buf, size, MSG_DONTWAIT);
  if (ret == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      return CREATE_ERRNO_RESULT();
    }
    bytes_recvd_out && (*bytes_recvd_out = 0);
  } else {
    bytes_recvd_out && (*bytes_recvd_out = ret);
  }
  return ok_result;
}
