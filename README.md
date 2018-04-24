# Relativistic Red Black Trees

## Structure
We are using Userspace's RCU library. Attain this library by pulling the git
and installing it like so:
```
$ git clone https://github.com/urcu/userspace-rcu.git
$ ./bootstrap # skip if using tarball
$ ./configure
$ make
$ make install
$ ldconfig
```

Then, run our program with the following commands:
```
$ g++ -std=c++11 -pthread -lurcu Relativistic-red-black-trees.cpp
$ ./a.out
```
