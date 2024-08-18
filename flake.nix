{
  description = "A Nix-flake-based C embedded development environment";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    # The systems supported for this flake
    supportedSystems = [
      "x86_64-linux" # 64-bit Intel/AMD Linux
      "aarch64-linux" # 64-bit ARM Linux
      "x86_64-darwin" # 64-bit Intel macOS
      "aarch64-darwin" # 64-bit ARM macOS
    ];

    # Helper to provide system-specific attributes
    forEachSupportedSystem = f:
      nixpkgs.lib.genAttrs supportedSystems (system:
        f {
          pkgs = import nixpkgs {inherit system;};
        });
  in {
    devShells = forEachSupportedSystem ({pkgs}: {
      # default to aarch64 shell
      default = pkgs.pkgsCross.aarch64-embedded.pkgsStatic.callPackage ({
        mkShell,
        zlib,
        pkg-config,
        file,
      }:
        mkShell {
          # these tools run on the build platform, but are configured to target the host platform
          nativeBuildInputs = [pkg-config file];

          # libraries needed for the host platform
          buildInputs = [];
        }) {};

      # alternative 32-bit arm shell
      arm = pkgs.pkgsCross.arm-embedded.pkgsStatic.callPackage ({
        mkShell,
        zlib,
        pkg-config,
        file,
      }:
        mkShell {
          # these tools run on the build platform, but are configured to target the host platform
          nativeBuildInputs = [pkg-config file];

          # libraries needed for the host platform
          buildInputs = [];

          packages = with pkgs; [qemu];
        }) {
      };
    });
  };
}
