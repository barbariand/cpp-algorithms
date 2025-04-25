{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-24.11";
    unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {
    nixpkgs,
    unstable,
    flake-utils,
    ...
  } @ inputs:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
      unstable_pkgs = import unstable {inherit system;};
    in {
      devShells.default = pkgs.mkShell (with pkgs; {
        packages = [gdb unstable_pkgs.zig clang-tools];
        shellHook = ''
          export CC="zig c++"
        '';
      });
    });
}
