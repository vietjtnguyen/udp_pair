# udp_pair

A very simple C object that wraps the BSD socket API to provide a UDP socket
pair abstraction. Basically say you have two known endpoints that need to talk
to each other over UDP. You know the port you are receiving on and the
address/port of the other end point. You can define a UDP pair object that
represents your end point and simply send/receive for that set up.

If you look at the code you'll see it's mostly a toy because performing such a
set up is very straightforward (`bind()`, `connect()`, `send()`, `recv()`), but
it can be a handy abstraction for those who are not so socket savvy.

## Example

```c
struct udp_pair* pair = udp_pair_create(recv_port, dest_ip4, dest_port);
char buf[512];
while (1) {
  ssize_t bytes_recvd = 0;
  udp_pair_recv(pair, (void*)buf, sizeof(buf), &bytes_recvd);
  if (bytes_recvd > 0) {
    udp_pair_send(pair, (void*)buf, bytes_recvd, NULL);
  }
}
udp_pair_free(pair);
```

The example sources are more verbose than this snippet due to error checking.
There are three examples provided:

- [`example/echo_server.c`](./example/echo_server.c)
- [`example/spam_hello_world.c`](./example/spam_hello_world.c)
- [`bin/udpcat.c`](./bin/udpcat.c)

## Building

The usual CMake/Make dance.

```sh
mkdir build
cd build
cmake ..
make
```
