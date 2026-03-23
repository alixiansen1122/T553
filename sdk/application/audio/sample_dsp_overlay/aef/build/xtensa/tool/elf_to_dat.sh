#!/bin/sh

hexdump_data_file()
{
    num=0
    in_file=${1}
    out_file=${2}
    
    value_list=`hexdump -v -e '1/1 "0x%02X "' ${in_file}`
    
    rm -rf ${out_file}
    
    for value in ${value_list}
    do
        echo -n "${value}," >> ${out_file}
        (( num++ ))
        if [ $num -eq 16 ]; then
            echo "" >> ${out_file}
            num=0
        else
            echo -n " " >> ${out_file}
        fi
    done

    echo "" >> ${out_file}
}

hexdump_data_file2()
{
    in_file=${1}
    out_file=${2}

    rm -rf ${out_file}
    hexdump -v -e '1/1 "0x%02X ,"' ${in_file} >> ${out_file}
}

hexdump_data_file2 $1 $2
