{
  description = "HelloTriangle - a Vulkan/GLFW (Wayland) triangle in C";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems =
        f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      packages = forAllSystems (pkgs: rec {
        default = hello-triangle;

        hello-triangle = pkgs.stdenv.mkDerivation {
          pname = "hello-triangle";
          version = "0.1.0";
          src = self;

          nativeBuildInputs = with pkgs; [
            clang
            shaderc # provides glslc
            makeWrapper
          ];

          buildInputs = with pkgs; [
            glfw
            cglm
            vulkan-headers
            vulkan-loader
            wayland
            libxkbcommon
            libdecor
          ];

          buildPhase = ''
            runHook preBuild

            # Compile the shaders to SPIR-V.
            glslc shaders/triangle_shader.vert -o shaders/vert.spv
            glslc shaders/gradient_shader.frag -o shaders/frag.spv

            make CC=clang

            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall

            # Shaders are loaded with paths relative to the CWD, so ship them
            # alongside the binary and run from that directory.
            mkdir -p $out/bin $out/share/hello-triangle/shaders
            cp VulkanTest $out/share/hello-triangle/
            cp shaders/vert.spv shaders/frag.spv $out/share/hello-triangle/shaders/

            makeWrapper $out/share/hello-triangle/VulkanTest $out/bin/hello-triangle \
              --chdir $out/share/hello-triangle \
              --prefix LD_LIBRARY_PATH : "${
                pkgs.lib.makeLibraryPath [
                  pkgs.vulkan-loader
                  pkgs.wayland
                  pkgs.libxkbcommon
                  pkgs.libdecor
                ]
              }" \
              --set-default VK_LAYER_PATH "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"

            runHook postInstall
          '';
        };
      });

      apps = forAllSystems (pkgs: {
        default = {
          type = "app";
          program = "${self.packages.${pkgs.stdenv.hostPlatform.system}.hello-triangle}/bin/hello-triangle";
        };
      });

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            clang
            gnumake
            pkg-config
            shaderc # glslc
            glfw
            cglm
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            vulkan-tools # vulkaninfo, for debugging
            wayland
            wayland-protocols
            libxkbcommon
            libdecor
          ];

          shellHook = ''
            export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"
            export LD_LIBRARY_PATH="${
              pkgs.lib.makeLibraryPath [
                pkgs.vulkan-loader
                pkgs.wayland
                pkgs.libxkbcommon
                pkgs.libdecor
              ]
            }''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

            echo "HelloTriangle dev shell (Wayland)"
            echo "  build : make"
            echo "  run   : ./VulkanTest   (from the repo root, shaders are loaded relatively)"
          '';
        };
      });
    };
}
