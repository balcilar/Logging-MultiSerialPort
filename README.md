# Multi SerialPort Read&Write

This code is for logging all data comes from multi serial ports without missing any data on ubunut systems.
Round robin buffer system was used in that code. Reader threads writes the data into round robin buffer. Writer threads picks up the data from buffer and writes them into file.

## Compile

```
$ gcc main.cpp -o test  -pthread

```

## Run

```
$ ./test
```


