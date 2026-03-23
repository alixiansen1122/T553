###########################################################################
 # @File Name            :   build_idp.sh
 # @Author               :   audio
 # @Date                 :   2023-03-01 08:00:00
 # @Last Modified time   :   2023-03-01 18:00:00
 # @Description          :   build_idp
 ##########################################################################
#!/bin/bash -x

set -e

APP_NAME=$0
CUR_DIR=`dirname "$0"`
PROJECT_ROOT=`realpath ${CUR_DIR}/../../../../`
SAP_SDK_DIR=`realpath ${PROJECT_ROOT}/drivers/drivers/driver/audio/source`
SAP_IDP_DIR=`realpath ${PROJECT_ROOT}/application/brandy/audio/sample_dsp_overlay/sea`
SAP_CORE_DIR=`realpath ${SAP_SDK_DIR}/core`
SAP_OUT_DIR=`realpath ${SAP_IDP_DIR}/output`

copy_chip_cfg()
{
    local chip=$1
    local board=$2
    local sap_xt_dir=${SAP_CORE_DIR}/build/xtensa   
    local idp_xt_dir=${SAP_IDP_DIR}/build/xtensa

    mkdir -p $idp_xt_dir/arch/lsp
    cp $sap_xt_dir/arch/$chip/lsp_sea $idp_xt_dir/arch/lsp/ -R
    rm -rf $idp_xt_dir/arch/lsp/lsp_sea/reset-vector.S
}

idp_install()
{
    local chip=$1

    local sap_xt_dir=${SAP_CORE_DIR}/build/xtensa
    local idp_xt_dir=${SAP_IDP_DIR}/build/xtensa

    local sap_inc_dir=${SAP_SDK_DIR}/include
    local core_inc_dir=${SAP_CORE_DIR}/include
    local core_com_dir=${SAP_CORE_DIR}/common/include
    local sea_comp_dir=${SAP_CORE_DIR}/component/sea/imedia/include
    local core_ovl_dir=${SAP_CORE_DIR}/overlay
    local idx_inc_dir=${SAP_IDP_DIR}/include
    local idx_ovl_dir=${SAP_IDP_DIR}/overlay

    echo "Install IDP : $chip"

    mkdir -p $idp_xt_dir/tool
    cp $sap_xt_dir/tool/*.py $idp_xt_dir/tool
    cp $sap_xt_dir/tool/*.sh $idp_xt_dir/tool
    rm $idp_xt_dir/tool/hso_xml.sh

    mkdir -p $idx_inc_dir
    mkdir -p $idx_ovl_dir
    cp $sap_inc_dir/td_type.h $idx_inc_dir
    cp $sap_inc_dir/audio_sea.h $idx_inc_dir
    cp $sap_inc_dir/audio_alg.h $idx_inc_dir
    cp $core_ovl_dir/component.h $idx_ovl_dir
    cp $core_ovl_dir/core_overlay.h $idx_ovl_dir
    cp $core_ovl_dir/core_overlay.c $idx_ovl_dir
}

idp_clean()
{
    local chip=$1

    local idp_xt_dir=${SAP_IDP_DIR}/build/xtensa
    local idp_out_dir=${SAP_IDP_DIR}/output
    local idp_inc_dir=${SAP_IDP_DIR}/include
    local idp_ovl_dir=${SAP_IDP_DIR}/overlay

    echo "Clean IDP : $chip"

    rm -rf $idp_xt_dir/tool
    rm -rf $idp_xt_dir/arch
    rm -rf $idp_out_dir
    rm -rf $idp_cfg_dir

    rm -f $idp_inc_dir/td_type.h
    rm -f $idp_inc_dir/audio_struct.h
    rm -f $idp_inc_dir/audio_alg.h
    rm -f $idp_inc_dir/audio_sea.h
    rm -rf $idp_ovl_dir
}

######################################################################
run_every_chip()
{
    local chips=("brandy" "socmn1")
    local boards=("fpga" "evb")
    local chip
    local board
    local exe_fun=$1

    for chip in ${chips[@]}; do
        for board in ${boards[@]}; do
            if [ "$chip" = "socmn1" ] && [ "$board" = "fpga" ]; then
                continue
            fi
            copy_chip_cfg $chip $board
        done
        $exe_fun $chip
    done
}
######################################################################
usage_chip()
{
    local chip=$1
    local board=$2

    if [ "$chip" = "socmn1" ] && [ "$board" = "fpga" ]; then
        return
    fi

    echo ""
    echo "Build   ${chip} ${board} main: ${APP_NAME} ${chip} ${board}"
    echo "Clean   ${chip} ${board} main: ${APP_NAME} ${chip} ${board}_clean"
}

build_idp_help()
{
    local chips=("brandy" "socmn1")
    local boards=("fpga" "evb")
    local chip
    local board

    echo "Usage of ${APP_NAME}"
    echo "For help:         ${APP_NAME} -h"
    echo ""
    echo "Build   all: ${APP_NAME}"
    echo "Clean   all: ${APP_NAME} clean"

    for chip in ${chips[@]}; do
        for board in ${boards[@]}; do
            usage_chip $chip $board
        done
    done

    return 1
}
######################################################################
main_entry_arg_0()
{
    run_every_chip idp_install
}

main_entry_arg_1()
{
    local args=("clean" "all")

    declare -A funs=( \
        ["-c"]="idp_clean" \
        ["clean"]="idp_clean" \
        ["all"]="idp_install" \
    )

    case $1 in
        "-h")
        build_idp_help
        ;;

        "clean" | "all")
        run_every_chip ${funs[${1}]}
        ;;

        *)
        build_idp_help
        ;;
    esac

    unset funs
}

main_entry_arg_2()
{
    local chip=$1
    local board=`echo ${2%%_*}`

    declare -A funs=( \
        ["${board}"]="idp_install" \
        ["${board}_clean"]="idp_clean"
        ["${board}_all"]="idp_install"
    )

    case $2 in
        "${board}" | "${board}_clean" | "${board}_all")
        copy_chip_cfg $chip $board
        ${funs[$2]} $chip
        ;;

        *)
        build_idp_help
        ;;
    esac

    unset funs
}

if [ $# -eq 0 ];then
    main_entry_arg_0
elif [ $# -eq 1 ];then
    main_entry_arg_1 $1
elif [ $# -eq 2 ];then
    main_entry_arg_2 $1 $2
else
    build_idp_help
    exit 1
fi
