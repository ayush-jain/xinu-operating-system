/* load_program.c - load_program */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load_program  -  Dynamically load a program
 *------------------------------------------------------------------------
 */
void*	load_program(char *path)
{
	
	Elf32_Ehdr eh;		
	Elf32_Shdr* sh_tbl;
	void* result;
	uint32 symtbl_idx;	

	int fd = open(RFILESYS, path, "or");
	//////kprintfff("FD is %d---\n", fd);
	gfd = open(RFILESYS, "xinu.elf", "or");
	
	if(fd<0 || gfd < 0) {
		return (void *)SYSERR;
	}
	//////kprintfff("%d %dable to open\n", fd, gfd);
	store_xinuinfo(gfd);

	read_elf_header(fd, &eh);
	
	if(!is_ELF(eh)) {
		return (void *)SYSERR;
	}
	//print_elf_header(eh);

	  
	sh_tbl = (Elf32_Shdr *)getmem(eh.e_shentsize * eh.e_shnum);
	if(!sh_tbl) {
		//////kprintfff("Failed to allocate %d bytes\n",(eh.e_shentsize * eh.e_shnum));
		return (void *)SYSERR;
	}
	read_section_header_table(fd, eh, sh_tbl);

	char *add = load_file(fd, eh, sh_tbl);

	if(!is_ELF(*(Elf32_Ehdr *)add)) {
		////kprintff("ELF File cannot be loaded.\n");
		return (void *)SYSERR;
	}

	Elf32_Sym* symtab = modify_symtab(fd, add, eh, sh_tbl, &symtbl_idx);

	if(symtab == (void *)SYSERR || entry == NULL){
		return (void *)SYSERR;
	}

	////kprintff("stage 2 beginning\n");
	result = elf_load(fd, add, &eh, sh_tbl, symtab, symtbl_idx);
	if(result == (void *)ELF_RELOC_ERR) {
		//////kprintfff("Unable to load ELF file.\n");
		return result;
	}

	//////kprintfff("finish\n");
	
	close(gfd);
	//////kprintfff("0x%08x ", result);
	
	return result;

}