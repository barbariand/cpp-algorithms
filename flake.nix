{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-24.11";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {
    nixpkgs,
    flake-utils,
    ...
  } @ inputs:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
    in {
      devShells.default = pkgs.mkShell (with pkgs; {
        packages = [clang clang-tools premake5 inja];
        shellHook = ''
          export LD_LIBRARY_PATH=${inja}:${clang}/lib
          export ISYSTEM=${clang}/bin/clang++
          premake5 gmake
          premake5 ecc
        '';
      });
    });
}
