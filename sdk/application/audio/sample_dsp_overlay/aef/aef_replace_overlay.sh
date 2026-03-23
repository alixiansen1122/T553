###########################################################################
 # @File Name            :   aef_replace_overlay.sh
 # @Author               :   audio
 # @Date                 :   2023-08-16 22:00:00
 # @Last Modified time   :   2023-08-26 16:49:16
 # @Description          :   aef_replace_overlay
 ##########################################################################
#!/bin/bash

dsp_version_name=$1
aef_lib_name=$2

#1. cp from tools/pkg....../dsp_overlay.bin
mkdir out/overlay_bin -p
cp ../../../../tools/pkg/bin/brandy/dsp/${dsp_version_name}/dsp_overlay.bin out/overlay_bin/dsp_overlay.bin

#2. unpacket dsp_overlay.bin
./build/xtensa/tool/unpacket.py out/overlay_bin/dsp_overlay.bin

#3. make aef.bin
make clean
make lib
make all

#4. replace aef.bin
rm -rf out/overlay_bin/aef.bin out/overlay_bin/dsp_overlay.bin
cp out/bin/${aef_lib_name} out/overlay_bin/${aef_lib_name}
mv out/overlay_bin/${aef_lib_name} out/overlay_bin/aef.bin

#5. re packet dsp_overlay.bin
./build/xtensa/tool/packet.py out/overlay_bin/dsp_overlay.bin out/overlay_bin/*.bin

#6. cp to tools/pkg....../dsp_overlay.bin
cp out/overlay_bin/dsp_overlay.bin ../../../../tools/pkg/bin/brandy/dsp/${dsp_version_name}/dsp_overlay.bin
rm -rf out/overlay_bin
