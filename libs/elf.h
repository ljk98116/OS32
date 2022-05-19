#ifndef ELF_H
#define ELF_H

#include "types.h"
#include "multiboot.h"

// Format of an ELF executable file
#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

// File header
struct elfhdr {
  uint magic;  // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint version;
  uint entry;
  uint phoff;
  uint shoff;
  uint flags;
  ushort ehsize;
  ushort phentsize;
  ushort phnum;
  ushort shentsize;
  ushort shnum;
  ushort shstrndx;
};

// Program section header
struct proghdr {
  uint type;
  uint off;
  uint vaddr;
  uint paddr;
  uint filesz;
  uint memsz;
  uint flags;
  uint align;
};

// Values for Proghdr type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

#define ELF32_ST_TYPE(i) ((i)&0xf)

typedef struct elf_section_header_t {
  uint name;
  uint type;
  uint flags;
  uint addr;
  uint offset;
  uint size;
  uint link;
  uint info;
  uint addralign;
  uint entsize;
} __attribute__((packed)) elf_section_header_t;

typedef struct elf_symbol_t {
  uint name;
  uint value;
  uint size;
  uchar  info;
  uchar  other;
  ushort shndx;
} __attribute__((packed)) elf_symbol_t;

typedef struct elf_t {
  elf_symbol_t *symtab;
  uint      symtabsz;
  const char   *strtab;
  uint      strtabsz;
} elf_t;

// get elf info from multiboot
elf_t elf_from_multiboot(MultiBoot_t *mb);

//get elf symbol infos
const char *elf_lookup_symbol(uint addr, elf_t *elf);

#endif