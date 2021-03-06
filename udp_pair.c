/**
 * @file
 * @brief
 * Defines the UDP pair type and its various associated functions.
 */
#include "udp_pair.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static struct udp_pair_result ok_result = {1, 0, NULL};


static
struct udp_pair_result
udp_pair_create_result_from_errno_with_func_name(
  const char* func_name)
{
  struct udp_pair_result result;
  result.ok = (errno == 0);
  result.err_num = errno;
  result.err_msg = strerror(errno);
  result.func_name = func_name;
  return result;
}


#define CREATE_ERRNO_RESULT() \
  (udp_pair_create_result_from_errno_with_func_name(__PRETTY_FUNCTION__))


struct udp_pair {
  int recv_sockfd;
  int send_sockfd;
  struct sockaddr_in recv_addr;
  struct sockaddr_in dest_addr;
};


struct udp_pair*
udp_pair_create(
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port)
{
  struct udp_pair* self = malloc(sizeof(struct udp_pair));
  struct udp_pair_result res = udp_pair_construct(
    self, recv_port, dest_ip4, dest_port);
  if (!res.ok) {
    return NULL;
  }
  return self;
}


void
udp_pair_free(
  struct udp_pair* self)
{
  udp_pair_destruct(self);
  free(self);
}


struct udp_pair_result
udp_pair_construct(
  struct udp_pair* self,
  uint16_t recv_port,
  const char* dest_ip4,
  uint16_t dest_port)
{
  // Create the receive socket and save the file descriptor.
  self->recv_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (self->recv_sockfd == -1) {
    return CREATE_ERRNO_RESULT();
  }

  // Create the send socket and save the file descriptor.
  self->send_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (self->send_sockfd == -1) {
    return CREATE_ERRNO_RESULT();
  }

  int ret;
  int enable = 1;

  ret = setsockopt(
    self->recv_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
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

  // Define our destination address struct.
  memset(&(self->dest_addr), 0, sizeof(self->dest_addr));
  self->dest_addr.sin_family = AF_INET;
  self->dest_addr.sin_addr.s_addr = inet_addr(dest_ip4);
  self->dest_addr.sin_port = htons(dest_port);

  // Even though UDP is connectionless we bind our socket to the receive
  // address so that the kernel knows that this process wants UDP packets going
  // to that receive address. This allows us to just use the read() or recv()
  // API instead of recvfrom().
  ret = bind(
    self->recv_sockfd,
    (const struct sockaddr*)(&self->recv_addr),
    sizeof(self->recv_addr));
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }

  return ok_result;
}


void
udp_pair_destruct(
  struct udp_pair* self)
{
  udp_pair_shutdown(self);
}


void
udp_pair_shutdown(
  struct udp_pair* self)
{
  shutdown(self->recv_sockfd, SHUT_RDWR);
  close(self->recv_sockfd);
}


int
udp_pair_get_fd(
  const struct udp_pair* self)
{
  return self->recv_sockfd;
}


struct udp_pair_result
udp_pair_send(
  struct udp_pair* self,
  const void* buf,
  const size_t size,
  ssize_t* bytes_sent_out)
{
  ssize_t ret = sendto(
    self->send_sockfd, buf, size, 0,
    (const struct sockaddr*)(&self->dest_addr),
    sizeof(self->dest_addr));
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }
  if (bytes_sent_out) {
    *bytes_sent_out = ret;
  }
  return ok_result;
}


struct udp_pair_result
udp_pair_recv(
  struct udp_pair* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out)
{
  ssize_t ret = recv(self->recv_sockfd, buf, size, 0);
  if (ret == -1) {
    return CREATE_ERRNO_RESULT();
  }
  if (bytes_recvd_out) {
    *bytes_recvd_out = ret;
  }
  return ok_result;
}


struct udp_pair_result
udp_pair_send_nonblock(
  struct udp_pair* self,
  const void* buf,
  const size_t size,
  ssize_t* bytes_sent_out)
{
  ssize_t ret = sendto(
    self->send_sockfd, buf, size, MSG_DONTWAIT,
    (const struct sockaddr*)(&self->dest_addr),
    sizeof(self->dest_addr));
  if (ret == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      return CREATE_ERRNO_RESULT();
    }
    if (bytes_sent_out) {
      *bytes_sent_out = 0;
    }
  } else {
    if (bytes_sent_out) {
      *bytes_sent_out = ret;
    }
  }
  return ok_result;
}


struct udp_pair_result
udp_pair_recv_nonblock(
  struct udp_pair* self,
  void* buf,
  const size_t size,
  ssize_t* bytes_recvd_out)
{
  ssize_t ret = recv(self->recv_sockfd, buf, size, MSG_DONTWAIT);
  if (ret == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      return CREATE_ERRNO_RESULT();
    }
    if (bytes_recvd_out) {
      *bytes_recvd_out = 0;
    }
  } else {
    if (bytes_recvd_out) {
      *bytes_recvd_out = ret;
    }
  }
  return ok_result;
}
