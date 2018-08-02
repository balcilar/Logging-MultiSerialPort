# Multi SerialPort Read&Write

This code is for logging all data comes from multi serial ports without missing any data on ubuntu systems.
Round robin buffer system was used in that code. Reader thread writes read-data into round robin buffer. Writer thread picks up the data from buffer and writes them into file.

## Compile

```
$ gcc main.cpp -o test  -pthread
```

## Run

Executables accept one additional argument which specify how many second wait before launching threads.

```
$ ./test 1
```


