source ${BUILD_ROOT}/src/linux/build/lib/dep-helper.sh
exit_on_error $?

installer_build_manifest()
{
    local name="${1}"
    local manifest="${2}"
    local native_packages="`product_native_packages ${name}`"
    local native_packages_compat="`product_native_packages_compat ${name}`"
    local native_obsolete_packages="`product_native_obsolete_packages ${name}`"
    local daemons="`product_daemons ${name}`"
    local obsolete_daemons="`product_obsolete_daemons ${name}`"
    local autostart="`product_autostart ${name}`"
    local regfiles="`product_regfiles ${name}`"

    mkdir -p "`dirname ${manifest}`"
    
    echo "PREFIX=\"${BUILD_PREFIX}\"" >${manifest}
    echo "PACKAGES=\"${native_packages}\"" >>${manifest}
    echo "PACKAGES_COMPAT=\"${native_packages_compat}\"" >>${manifest}
    echo "OBSOLETE_PACKAGES=\"${native_obsolete_packages}\"" >>${manifest}
    echo "DAEMONS=\"${daemons}\"" >>${manifest}
    echo "OBSOLETE_DAEMONS=\"${obsolete_daemons}\"" >>${manifest}
    echo "AUTOSTART=\"${autostart}\"" >>${manifest}
    echo "REGFILES=\"${regfiles}\"" >>${manifest}
}
