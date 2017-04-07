# udp_conn

A very simple C object that wraps the BSD socket API to behave like a UDP
connection even though UDP is connectionless.

## Example

```c
struct udp_conn* conn = udp_conn_create(recv_port, dest_ip4, dest_port);
char buf[512];
while (1) {
  ssize_t bytes_recvd = 0;
  udp_conn_recv(conn, (void*)buf, sizeof(buf), &bytes_recvd);
  if (bytes_recvd > 0) {
    udp_conn_send(conn, (void*)buf, bytes_recvd, NULL);
  }
}
udp_conn_free(conn);
```

The example sources are more verbose than this snippet due to error checking.
There are three examples provided:

- `echo_server.c`
- `example.c`
- `udpcat.c`

## Building

The usual CMake/Make dance.

```sh
mkdir build
cd build
cmake ..
make
```
