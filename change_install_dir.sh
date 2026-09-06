# change installation directory (m all_install)
#  (note) e.g. % . ./change_install_dir.sh `realpath local_install/`
if [ -z "${1}" ]; then
#export TKS_PREFIX=/d/bsp/tools/tks/
export TKS_PREFIX="`realpath local_install/`"
else
export TKS_PREFIX="${1}"
fi
export TKS_SITE_PREFIX="${TKS_PREFIX}"
export TKS_TARGET_PREFIX="${TKS_PREFIX}"
export TKS_TARGET_SITE_PREFIX="${TKS_PREFIX}"
# libtks
export TKS_TARGET_INCLUDE_PREFIX="${TKS_PREFIX}/include/"
export TKS_TARGET_LIB_PREFIX="${TKS_PREFIX}/lib/"
export TKS_INCLUDE_PREFIX=${TKS_TARGET_INCLUDE_PREFIX}
export TKS_LIB_PREFIX="${TKS_TARGET_LIB_PREFIX}"
echo "install dir set to \"${TKS_PREFIX}\""

# for all_install target
export PATH="$PATH:${TKS_PREFIX}"
