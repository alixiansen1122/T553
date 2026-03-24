#!/usr/bin/env python3
"""
generate_lfs_image.py
生成 LittleFS 镜像，参数与设备端 littlefs_cfg.h 完全一致

用法:
    python3 generate_lfs_image.py [源目录] [输出文件]
    python3 generate_lfs_image.py                          # 默认 lfs_root/ → littlefs.bin
    python3 generate_lfs_image.py my_files/ output.bin     # 自定义
"""

import os
import sys

try:
    from littlefs import LittleFS
except ImportError:
    print("请先安装: pip install littlefs-python")
    sys.exit(1)

# ===== 与 littlefs_cfg.h 完全一致 =====
BLOCK_SIZE      = 4096
BLOCK_COUNT     = 1536      # 6MB / 4096  （原来是 768）
READ_SIZE       = 256
PROG_SIZE       = 256
CACHE_SIZE      = 512
LOOKAHEAD_SIZE  = 256
IMAGE_SIZE      = BLOCK_SIZE * BLOCK_COUNT  # 6291456

# Flash 烧录地址（仅供打印提示）
FLASH_OFFSET    = 0xA00000

# ===== 默认路径 =====
DEFAULT_SOURCE  = "lfs_root"
DEFAULT_OUTPUT  = "littlefs.bin"


def create_image(source_dir, output_file):
    if not os.path.isdir(source_dir):
        print(f"错误: 源目录 '{source_dir}' 不存在")
        print(f"请创建目录并放入预置文件，例如:")
        print(f"  mkdir -p {source_dir}/audio")
        print(f"  cp *.ogg {source_dir}/audio/")
        sys.exit(1)

    # 自动创建输出目录
    output_dir = os.path.dirname(output_file)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # 创建 LittleFS 实例
    fs = LittleFS(
        block_size=BLOCK_SIZE,
        block_count=BLOCK_COUNT,
        read_size=READ_SIZE,
        prog_size=PROG_SIZE,
    )

    file_count = 0
    dir_count = 0
    total_bytes = 0

    # 递归遍历源目录
    for root, dirs, files in os.walk(source_dir):
        rel_root = os.path.relpath(root, source_dir)
        if rel_root == ".":
            rel_root = ""

        # 创建子目录（根目录不需要创建）
        for d in dirs:
            if rel_root:
                dir_path = f"/{rel_root}/{d}"
            else:
                dir_path = f"/{d}"
            dir_path = dir_path.replace("\\", "/")

            print(f"  [DIR]  {dir_path}")
            fs.mkdir(dir_path)
            dir_count += 1

        # 写入文件
        for f in files:
            src_path = os.path.join(root, f)

            if rel_root:
                dst_path = f"/{rel_root}/{f}"
            else:
                dst_path = f"/{f}"
            dst_path = dst_path.replace("\\", "/")

            with open(src_path, "rb") as fp:
                data = fp.read()

            print(f"  [FILE] {dst_path}  ({len(data)} bytes)")

            with fs.open(dst_path, "wb") as fp:
                fp.write(data)

            file_count += 1
            total_bytes += len(data)

    # 导出镜像数据
    image = bytes(fs.context.buffer)

    # 确保镜像大小正确（补 0xFF 到完整 3MB）
    if len(image) < IMAGE_SIZE:
        image += b'\xff' * (IMAGE_SIZE - len(image))
    elif len(image) > IMAGE_SIZE:
        print(f"警告: 镜像 {len(image)} 字节超过 {IMAGE_SIZE} 字节，截断!")
        image = image[:IMAGE_SIZE]

    # 写入文件
    with open(output_file, "wb") as fp:
        fp.write(image)

    # 打印摘要
    print(f"\n{'='*50}")
    print(f"LittleFS 镜像生成成功!")
    print(f"{'='*50}")
    print(f"  输出文件:     {output_file}")
    print(f"  镜像大小:     {len(image)} bytes ({len(image)//1024} KB)")
    print(f"  目录数:       {dir_count}")
    print(f"  文件数:       {file_count}")
    print(f"  文件总大小:   {total_bytes} bytes")
    print(f"{'='*50}")
    print(f"  block_size:   {BLOCK_SIZE}")
    print(f"  block_count:  {BLOCK_COUNT}")
    print(f"  read_size:    {READ_SIZE}")
    print(f"  prog_size:    {PROG_SIZE}")
    print(f"{'='*50}")
    print(f"  烧录地址:     0x{FLASH_OFFSET:06X} ({FLASH_OFFSET//1024//1024}MB)")
    print(f"  烧录大小:     0x{IMAGE_SIZE:06X} ({IMAGE_SIZE//1024//1024}MB)")
    print(f"{'='*50}")

    # 使用统计
    used_blocks = 0
    for i in range(BLOCK_COUNT):
        block = image[i*BLOCK_SIZE : (i+1)*BLOCK_SIZE]
        if block != b'\xff' * BLOCK_SIZE:
            used_blocks += 1

    print(f"  已用块数:     {used_blocks} / {BLOCK_COUNT}")
    print(f"  已用空间:     {used_blocks * BLOCK_SIZE // 1024} KB")
    print(f"  剩余空间:     {(BLOCK_COUNT - used_blocks) * BLOCK_SIZE // 1024} KB")
    print(f"{'='*50}")

    return 0


if __name__ == "__main__":
    source = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_SOURCE
    output = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_OUTPUT
    ret = create_image(source, output)
    sys.exit(ret)
