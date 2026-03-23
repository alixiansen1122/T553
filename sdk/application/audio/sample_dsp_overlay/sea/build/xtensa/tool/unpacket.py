#!/usr/bin/python3

import io
import os
import sys
from pathlib import Path


class BinFile:
    def __init__(self, file_path: str) -> None:
        self.file = io.open(Path(file_path), 'rb')

    def __del__(self):
        self.file.close()

    def read_int(self) -> int:
        data = self.file.read(4)
        return int.from_bytes(data, 'little')

    def read_str(self) -> str:
        data = self.file.read(16)
        return data.decode('utf-8').strip('\000')

    def seek(self, size: int) -> None:
        self.file.seek(size, 1)

    def read(self, size: int) -> bytes:
        return self.file.read(size)


def unpacket_file(lib_name: str) -> None:
    lib = BinFile(lib_name)
    num = lib.read_int()
    file_dir = os.path.dirname(lib_name)

    for i in range(num):
        name = lib.read_str()
        size = lib.read_int()
        print("%-16s: 0x%08x, %d" % (name, size, size))
        spr_file_path = os.path.join(file_dir, name)
        spr_file = io.open(spr_file_path, 'wb')
        spr_file.write(lib.read(size))


if __name__ == '__main__':
    unpacket_file(sys.argv[1])
