# Getting Started
This project by default compiles for Raspberry Pi 4.

To compile for Raspberry Pi 3
```sh
make target_rpi3
```

For NixOS or system that uses Nixpkgs, cross-compile and develop by
```sh
nix develop
```

For others, install the according aarch64 toolchain
