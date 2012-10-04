#include <stdlib.h>
#include <elf.h>

Elf32_Phdr* get_elf_phdr(Elf32_Ehdr *hdr) {
    Elf32_Phdr* phdr;
    if ((hdr->e_ident[EI_MAG0] !=  ELFMAG0) || 
            (hdr->e_ident[EI_MAG1] !=  ELFMAG1) || 
            (hdr->e_ident[EI_MAG2] !=  ELFMAG2) ||
            (hdr->e_ident[EI_MAG3] !=  ELFMAG3)) {
        return NULL;
    }
    if (hdr->e_type != ET_EXEC) { return NULL; }
    if (hdr->e_machine != EM_386) { return NULL; }
    if (hdr->e_version != EV_CURRENT) { return NULL; }
    if (hdr->e_flags != 0) { return NULL; }
    phdr = (Elf32_Phdr *) (hdr->e_phoff + (unsigned int) hdr);

    if (hdr->e_phnum <= 0) {
        return NULL;
    }

    return phdr;

}
