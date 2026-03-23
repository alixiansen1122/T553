#!/usr/bin/python3

import io
import sys
import reader
from pathlib import Path


class BinFile:
    def __init__(self, file_path: str) -> None:
        self.file = io.open(Path(file_path), 'wb')

    def __del__(self):
        self.file.close()

    def write_int(self, n: int) -> None:
        self.file.write(n.to_bytes(4, 'little'))

    def write(self, data):
        self.file.write(data)


def clip_elf(elf: str, out: str):
    rh = reader.ELFReader(elf)
    e_phnum = rh.get_header_field('EI_PHNUM')
    section_num = 0
    for i in range(e_phnum):
        p_type = rh.get_progheader_field('P_TYPE', i)
        p_filesz = rh.get_progheader_field('P_FILESZ', i)
        if p_type[0] != 1 or p_filesz == 0:
            continue
        section_num += 1

    out_file = BinFile(out)
    out_file.write_int(section_num)
    for i in range(e_phnum):
        p_type = rh.get_progheader_field('P_TYPE', i)
        p_paddr = rh.get_progheader_field('P_ADDR', i)
        p_filesz = rh.get_progheader_field('P_FILESZ', i)
        if p_type[0] != 1 or p_filesz == 0:
            continue
        out_file.write_int(p_paddr)
        out_file.write_int(p_filesz)
        data = rh.get_raw_segment(i)
        out_file.write(data)


if __name__ == '__main__':
    clip_elf(sys.argv[1], sys.argv[2])
