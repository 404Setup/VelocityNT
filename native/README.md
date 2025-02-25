# velocity-natives

This directory contains native acceleration code for Velocity, along with traditional Java fallbacks.

Compression is based on the `libdeflate` library, which is wire-compatible with zlib but significantly faster.

Encryption is based on OpenSSL for Linux, which is the most widely-used encryption library in the world.
OpenSSL has had several different ABIs over the years, so we provide multiple versions of the native
library. Currently we compile against OpenSSL 1.1.x and OpenSSL 3.x.x. For macOS, we use the built-in
CommonCrypto library.

## Supported Platforms

`velocity-natives` is built for the following platforms:

- Windows x86_64
- Linux x86_64
- Linux aarch64
- macOS aarch64 ("Apple Silicon")

For Linux platforms, we provide two versions of the native library: one built against OpenSSL 1.1.x and one built against OpenSSL 3.x.x.
All native libraries are built on various versions of Ubuntu and Alpine:

- Windows 11 26120.3291 for OpenSSL 3.1.1 (and 1.1.1w) support and for compression
- Ubuntu 20.04 for OpenSSL 1.1.x support and for compression
- Ubuntu 22.04 for OpenSSL 3.x.x support
- Alpine 3.18 for OpenSSL 3.x.x support and compression (musl libc users only)

## Building

Since I tweaked some internal code, this may perform a little differently on non-Windows platforms, 
although I don't think it will affect it too much

### On Windows
I'm still tweaking the build scripts, and the way they're all built might be a little fiddly

#### OpenSSL
Install OpenSSL-Win64 3.1+ (https://slproweb.com/products/Win32OpenSSL.html) and leave them in the default path.

Then in the native module, rename `CMakeLists.openssl.txt` to `CMakeLists.txt`, 
and then execute the "cmake -B build && cmake --build build".

Find it in the `native/src/main/resources/windows_x86_64/Debug/velocity-cipher-ossl30x.dll` and move it to the
 `native/src/main/resources/windows_x86_64/velocity-cipher-ossl30x.dll`

#### Deflate
TODO )

### On Linux

To build the native libraries, you need to have Docker installed and have it set up to perform [multi-platform builds](https://docs.docker.com/build/building/multi-platform/). Then, run the following command:

```bash
./build-support/build-all-linux-natives.sh
```

This will build the native libraries for both OpenSSL 1.1.x and OpenSSL 3.x.x on both x86_64 and aarch64.

### On macOS

To build the native libraries on macOS, you need to have `cmake` installed. You can install it using Homebrew:

```bash
brew install cmake
```

Then, run the following command:

```bash
./build-support/compile-macos.sh
```

This will build the native libraries for macOS aarch64. x86_64 has not been tested, but it should work.

### On any other operating system?

If your OS of choice is a Unix of some sort, you can use the individual Linux build scripts as a base:

- `build-support/compile-linux-compress.sh`
- `build-support/compile-linux-crypto.sh`

You will need to have the necessary build tools installed (a C/C++ toolchain and `cmake`), and you will need to have OpenSSL installed. You will also need to adjust the script to your needs.