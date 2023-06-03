# Web_server_project
---
## Usage
```
$ make
$ ./server

...other terminal

$ ./client

$ ./test.sh (for testing persistence connection)
```

## IO multiflexing principle

```
loop:
  select -> find ready state sockets
  for socket in socket set:
    if there are ready state socket:
      if it is server socket:
        accept new connection
        add new connection to socket set(fd_set)
      else:
        handle its own request 
```
