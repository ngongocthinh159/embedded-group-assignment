# Getting Started

This project by default compiles for Raspberry Pi 4 with UART1.

Alternatively, these are different commands for Raspberry Pi 3 and UART0:"

```sh
make r3_u0
make r3_u1
make r4_u0
make r4_u1
```

For NixOS or system that uses Nixpkgs, cross-compile and develop by

```sh
nix develop
```

For others, install the according aarch64 toolchain
