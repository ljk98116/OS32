#include "../../libs/elf.h"
#include "../../libs/kstring.h"
#include "../../libs/vmm.h"
#include "../../libs/thread.h"

elf_t elf_from_multiboot(MultiBoot_t *mb)
{
    int i;
    elf_t elf;
    elf_section_header_t *sh = (elf_section_header_t *)(mb->addr);
    uint shstrtab = sh[mb->shndx].addr;
    for (i = 0; i < mb->num; i++) {
        const char *name = (const char *)(shstrtab + sh[i].name) + PAGE_OFFSET;
        if (strcmp(name, ".strtab") == 0) {
            elf.strtab = (const char *)(sh[i].addr) + PAGE_OFFSET;
            elf.strtabsz = sh[i].size;
        }
        if (strcmp(name, ".symtab") == 0) {
            elf.symtab = (elf_symbol_t *)(sh[i].addr + PAGE_OFFSET);
            elf.symtabsz = sh[i].size;
        }
    }
    return elf;
}

const char *elf_lookup_symbol(uint addr, elf_t *elf)
{
    int i;

    for (i = 0; i < (elf->symtabsz / sizeof(elf_symbol_t)); i++) {
        if (ELF32_ST_TYPE(elf->symtab[i].info) != ELF_PROG_FLAG_WRITE) {
            continue;
        }
        // get func name from func addr
        if ( (addr >= elf->symtab[i].value) && (addr < (elf->symtab[i].value + elf->symtab[i].size)) ) {
            return (const char *)((uint)elf->strtab + elf->symtab[i].name);
        }
    }

    return NULL;
}
