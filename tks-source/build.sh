# % . ./build.sh
m clean; m OPT_LTO=y bin && m install; m clean; m clean_lib; m OPT_LTO=y bin_lib && m install_lib ; m clean_lib
