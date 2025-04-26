{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-24.11";
    unstable_nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {
    nixpkgs,
    unstable_nixpkgs,
    flake-utils,
    ...
  } @ inputs:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
      unstable = import unstable_nixpkgs {inherit system;};
    in {
      devShells.default = pkgs.mkShell (with pkgs; {
        packages = [gdb unstable.zig unstable.zls unstable.clang-tools];
        shellHook = ''
          export CC="zig c++"
          export CPLUS_INCLUDE_PATH="$(jj workspace root)/zig-out/include:$CPLUS_INCLUDE_PATH"
        '';
      });
    });
}
