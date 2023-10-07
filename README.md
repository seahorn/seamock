![os](https://img.shields.io/badge/os-linux-orange?logo=linux)
[![CI](https://github.com/seahorn/seamock/actions/workflows/main.yml/badge.svg)](https://github.com/seahorn/seamock/actions?query=workflow%3ACI)

<a href="url"><img src="https://github.com/seahorn/seamock/blob/main/assets/seahorse.png" align="left" height="48" width="48" ></a>

# SeaMock
SeaMock is a mocking framework for verification.

SeaMock is a header-only framework. 
It depends on Boost Hana and requires for it to be installed.
See the CMake project on how to check that Boost is installed during configuration. 

## Running examples

A `ipc` example is provided to compare fakes, function summaries and vMocks. 


### Configuration 
1. Instructions for building and running a docker image of SeaMock is available in this [README](https://github.com/seahorn/seamock/blob/main/docker/README.md).
Instructions for setting up the build locally follow.

1. Setup SEAHORN as described in https://github.com/seahorn/verifyTrusty/tree/fmcad23.

1. Use CMake to configure project.

```sh
  mkdir build && cd build && cmake \
   -DSEA_LINK=llvm-link-14 \
   -DCMAKE_C_COMPILER=clang-14 \
   -DCMAKE_CXX_COMPILER=clang++-14 \
   -DSEAHORN_ROOT=<SEAHORN_ROOT> -DTRUSTY_TARGET=x86_64 \
   ../ -GNinja
```

### Compilation and verification
1. To compile and verify the fake (in the `build` dir)

``` sh
ninja && ./verify  examples/seahorn/ipc/fake/
```

1. To compile and verify the fake (in the `build` dir)

``` sh
ninja && ./verify  examples/seahorn/ipc/summary/
```
> **_NOTE:_**  The function summary unit proof will return `sat` since the model is expected to cause false positives.
1. To compile and verify the fake (in the `build` dir)

``` sh
ninja && ./verify  examples/seahorn/ipc/vmock/
```

