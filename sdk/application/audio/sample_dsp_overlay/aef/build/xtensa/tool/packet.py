#!/usr/bin/python3

import io
import os
import sys
from pathlib import Path
from typing import List


class BinFile:
    size = 0
    name = ""

    def __init__(self, file_path: str) -> None:
        self.file = io.open(Path(file_path), 'rb')
        self.size = os.path.getsize(file_path)
        self.name = os.path.basename(file_path).ljust(16, '\000')

    def __del__(self):
        self.file.close()

    def read(self) -> bytes:
        return self.file.read(self.size)


class ElfPacket:
    def __init__(self, file_path: str) -> None:
        self.path = Path(file_path)
        self.file = io.open(self.path, 'wb')

    def __del__(self):
        self.file.close()

    def write_int(self, n: int) -> None:
        self.file.write(n.to_bytes(4, 'little'))

    def write_str(self, s: str) -> None:
        self.file.write(s.encode('utf-8'))

    def packet(self, file_path: str) -> None:
        lib_file = BinFile(file_path)
        if lib_file.size == 0:
            raise FileNotFoundError
        self.write_str(lib_file.name)
        self.write_int(lib_file.size)
        self.file.write(lib_file.read())


def packet_file(lib_name: str, file_list: List[str]) -> None:
    lib = ElfPacket(lib_name)
    lib.write_int(len(file_list))
    for file in file_list:
        lib.packet(file)


if __name__ == '__main__':
    print(sys.argv)
    packet_file(sys.argv[1], sys.argv[2:])
