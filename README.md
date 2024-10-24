# Overview
fast-sync, is a header-only sync library that aims to implement sync mechanisms as wait-free and blazingly fast!

# Building
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

# Details
Common sync structs have been implemented by atomic operations.

# Comparision
In the benchmarks/ folder you can see the demo code and the results against POSIX sync mechanisms.