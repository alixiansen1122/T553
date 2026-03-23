#!/usr/bin/python3

import os
import sys
import shutil

def is_dir_empty(dir_path):
    return not bool(os.listdir(dir_path))

def help(cmd_name):
    print('Usage:%s [packet_opt] [dsp_version]' % cmd_name)
    print('Params:[packet_opt]: packet or unpacket')
    print('       [packet_opt]: packet to generate dsp_overlay.bin from %s' % (local_out_path))
    print('       [packet_opt]: unpacket to extract dsp_overlay.bin to separate bin in %s' % (local_out_path))
    print('Params:[dsp_version]: Select dsp version according to target')
    print('       [dsp_version]: max for target3/4/5')
    print('       [dsp_version]: min for target1')
    print('       [dsp_version]: normal for other target')
    print('Example:%s unpacket max' % cmd_name)
    print('Example:%s packet max' % cmd_name)

if __name__ == '__main__':
    dsp_version_enum = ['max', 'mini', 'normal']
    dsp_overlay_bin_name = "dsp_overlay.bin"
    local_out_path = os.path.join('out', 'overlay_bin')

    if len(sys.argv) < 2:
        print('Invalid params!')
        help(sys.argv[0])
        sys.exit()

    packet_opt = sys.argv[1]
    dsp_version = sys.argv[2]

    if dsp_version not in dsp_version_enum:
        print('Invalid dsp version!')
        help(sys.argv[0])
        sys.exit()

    dsp_bin_dir = os.path.join('..', '..', '..', 'tools', 'pkg', 'bin', 'brandy', 'dsp')
    dsp_bin_path = os.path.join(dsp_bin_dir, dsp_version)
    dsp_overlay_bin_src_path = os.path.join(dsp_bin_path, dsp_overlay_bin_name)
    dsp_overlay_bin_dst_path = os.path.join(local_out_path, dsp_overlay_bin_name)

    tool_dir = os.path.join('aef', 'build', 'xtensa', 'tool')
    pkt_tool = os.path.join(tool_dir, 'packet.py')
    unpkt_tool = os.path.join(tool_dir, 'unpacket.py')

    print('Operation: %s' % (packet_opt))
    print('dsp_version: %s' % (dsp_version))
    print('dsp_bin_path: %s' % (dsp_bin_path))
    print('local_out_path: %s' % (local_out_path))

    if packet_opt == "packet":
        if os.path.exists(dsp_overlay_bin_dst_path):
            os.remove(dsp_overlay_bin_dst_path)

        cmd = 'python %s %s' %(pkt_tool, dsp_overlay_bin_dst_path)
        for file_name in os.listdir(local_out_path):
            file_path = os.path.join(local_out_path, file_name)
            cmd = '%s %s' %(cmd, file_path)
        print('cmd: %s' % (cmd))
        result1 = os.system(cmd)

        print("copy %s -> %s" % (dsp_overlay_bin_dst_path, dsp_overlay_bin_src_path))
        shutil.copy(dsp_overlay_bin_dst_path, dsp_overlay_bin_src_path)

    elif packet_opt == "unpacket":
        if os.path.exists(local_out_path):

            # directory empty or not
            if is_dir_empty(local_out_path):
                print('The directory is empty')
            else:
                print('The directory is not empty')
                for file_name in os.listdir(local_out_path):
                    file_path = os.path.join(local_out_path, file_name)
                    os.remove(file_path)
        else:
            os.makedirs(local_out_path)

        print("copy %s -> %s" % (dsp_overlay_bin_src_path, dsp_overlay_bin_dst_path))
        shutil.copy(dsp_overlay_bin_src_path, dsp_overlay_bin_dst_path)

        cmd = 'python %s %s' %(unpkt_tool, dsp_overlay_bin_dst_path)
        print('cmd: %s' % (cmd))
        result1 = os.system(cmd)

        os.remove(dsp_overlay_bin_dst_path)
    else:
        print('Invalid cmd!')
        help(sys.argv[0])
        sys.exit()
