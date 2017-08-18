/* load_library.c - load_library */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  load_library  -  Dynamically load a library
 *------------------------------------------------------------------------
 */
syscall	load_library(char *path)
{
	Elf32_Ehdr eh;		
	Elf32_Shdr* sh_tbl;
	void* result;
	uint32 symtbl_idx;	

	int fd = open(RFILESYS, path, "or");
	//kprintf("FD is %d---\n", fd);
	gfd = open(RFILESYS, "xinu.elf", "or");
	
	if(fd<0 || gfd < 0) {
		return SYSERR;
	}
	//kprintf("%d %dable to open\n", fd, gfd);
	store_xinuinfo(gfd);

	read_elf_header(fd, &eh);
	
	if(!is_ELF(eh)) {
		return SYSERR;
	}
	//print_elf_header(eh);

	  
	sh_tbl = (Elf32_Shdr *)getmem(eh.e_shentsize * eh.e_shnum);
	if(!sh_tbl) {
		//kprintf("Failed to allocate %d bytes\n",(eh.e_shentsize * eh.e_shnum));
		return SYSERR;
	}
	read_section_header_table(fd, eh, sh_tbl);

	char *add = load_file(fd, eh, sh_tbl);

	if(!is_ELF(*(Elf32_Ehdr *)add)) {
		//kprintf("ELF File cannot be loaded.\n");
		return SYSERR;
	}

	Elf32_Sym* symtab = modify_symtab(fd, add, eh, sh_tbl, &symtbl_idx);

	if(symtab == (void *)SYSERR){
		return (void *)SYSERR;
	}

	//kprintf("stage 2 beginning\n");
	result = elf_load(fd, add, &eh, sh_tbl, symtab, symtbl_idx);
	if(result == (void *)ELF_RELOC_ERR) {
		//kprintf("Unable to load ELF file.\n");
		return (int)result;
	}

	//kprintf("finish\n");
	
	close(gfd);
	//kprintf("0x%08x ", result);
	if(func_size > 10){
		return SYSERR;
	}
	lib_count++;

	if(lib_count > 3){
		lib_count--;
		return SYSERR;
	}
	return OK;
}