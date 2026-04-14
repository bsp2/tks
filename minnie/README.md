`Minnie` is a 2D vector graphics library with an OpenVG-like API, distributed under terms of the [MIT license](https://opensource.org/license/mit).

The documentation is work-in-progress. Also [see here](http://tkscript.org/minnie/minnie.html)

See `native/build/` for Linux / macOS / Windows(msvc) Makefiles.


# Debian GNU/Linux build instructions

## Install package dependencies
~~~
$ sudo apt-get install build-essential libsdl2-dev
~~~

## Clone the repo
~~~
$ git clone https://github.com/bsp2/minnie.git
~~~

## Build the test cases
~~~
$ cd minnie
$ . ./setenv_linux.sh
$ cd minnie/native/build/linux/`
$ m clean ; m bin
~~~

This will create the `test_min`, `test_mib`, and `test_shadervg` executables.

## Running the test cases
~~~
$ ./test_min
$ ./test_mib
$ ./test_shadervg
~~~

### Interactive keys
~~~
    UP : Next test
  DOWN : Previous test
  LEFT : Decrease line width
 RIGHT : Increase line width
ESCAPE : Quit
~~~
Please refer to the test source codes for a full list of (debug) keyboard shortcuts.

### Benchmark mode
Each test case executable supports the following command line options:
~~~
$ ./test_[min|mib|shadervg] [<test_idx> [<num_frames> [<num_iterations_per_frame>]]]
~~~
The `<num_frames>` and `<num_iterations_per_frame>` options enable the benchmark mode.

Since the swap-buffers operation usually takes a significant amount of time on a desktop PC and caps the framerate to a few thousand FPS, it is recommended to set a low `<num_frames>` and a high `<num_iterations_per_frame>`, e.g.:
~~~
$ ./test_shadervg 177 5 50000
~~~

Note: vertical blank synchronisation will automatically be disabled in benchmark mode.
