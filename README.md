## Prerequisites

- Download and install [sodium](https://doc.libsodium.org/installation)
- Clone and install [mbedtls](https://github.com/Mbed-TLS/mbedtls) (Version 3.x at least is needed). Installing can be done by just running
  -     make && make install
- Download [yojimbo](https://github.com/networkprotocol/yojimbo) into the root of the directory:
  - Copy the **CMakeLists.txt.yojimbo** file in the **resources** directory into the **yojimbo** directory
  - Rename **CMakeLists.txt.yojimbo** to **CMakeLists.txt**
- Download [Imgui](https://github.com/ocornut/imgui) into the root of the directory:
  - Copy the **CMakeLists.txt.imgui** file in the **resources** directory into the **imgui** directory
  - Rename **CMakeLists.txt.imgui** to **CMakeLists.txt**
- Install [SDL2](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php) and [SDL2_image](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php)
- Download and install [stduuid](https://github.com/mariusbancila/stduuid)
- Download and install [fast-wfc](https://github.com/math-fehr/fast-wfc)
  - Move **fast-wfc** into the root of the directory