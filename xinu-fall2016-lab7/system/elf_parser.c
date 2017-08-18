#include <xinu.h>


int gfd;
void* entry = NULL;

info_elem xinu_info[SYMSIZE];
info_elem func_info[SYMSIZE];

int func_size = 0;
int xinu_size = 0;

int lib_count = 0;

bool8 is_ELF(Elf32_Ehdr eh)
{
	/* ELF first 4 bytes are 0x7f,'E','L','F' */
	 
	if(!strncmp((char*)eh.e_ident, "\177ELF", 4)) {
		kprintf("ELF first 4 bytes \t= ELF\n");
		return TRUE;
	} else {
		kprintf("Not ELF file!\n");
		return FALSE;
	}
}

bool8 check_xinu(char *name){
	if(strncmp("main", name, strlen(name)) == 0){
		return FALSE;
	}
	int i;
	for(i=0;i<xinu_size;i++){
		if(strncmp(xinu_info[i].str, name, strlen(name)) == 0 && (strlen(name) == strlen(xinu_info[i].str)) ){
			return TRUE;
		}
	}
	return FALSE;
}

bool8 check_func(char *name){
	if(strncmp("main", name, strlen(name)) == 0){
		return FALSE;
	}
	int i;
	for(i=0;i<func_size;i++){
		if(strncmp(func_info[i].str, name, strlen(name)) == 0 && (strlen(name) == strlen(func_info[i].str)) ){
			return TRUE;
		}
	}
	return FALSE;
}



void read_elf_header(int32 fd, Elf32_Ehdr *elf_header)
{
	if(elf_header != NULL){
		seek(fd, 0);
		if(read(fd, (void *)elf_header, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr)){
			kprintf("Success");
		} else {
			kprintf("failure.....\n");
		}
	}
}

void print_elf_header(Elf32_Ehdr elf_header)
{

 
	kprintf("Storage type\t= ");
	switch(elf_header.e_ident[EI_CLASS])
	{
		case ELFCLASS32:
			kprintf("32-bit \n");
			break;

		case ELFCLASS64:
			kprintf("64-bit\n");
			break;

		default:
			kprintf("Invalid storage\n");
			break;
	}

	kprintf("Data format\t= ");
	switch(elf_header.e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
			kprintf("2's complement, little endian\n");
			break;

		case ELFDATA2MSB:
			kprintf("2's complement, big endian\n");
			break;

		default:
			kprintf("Invalid Format\n");
			break;
	}

 
	kprintf("OS ABI\t\t= ");
	switch(elf_header.e_ident[EI_OSABI])
	{
				
		default:
			kprintf("Unknown (0x%x)\n", elf_header.e_ident[EI_OSABI]);
			break;
	}

	// ELF filetype 
	kprintf("Filetype \t= ");
	switch(elf_header.e_type)
	{
		case ET_NONE:
			kprintf("N/A (0x0)\n");
			break;

		case ET_REL:
			kprintf("Relocatable\n");
			break;

		case ET_EXEC:
			kprintf("Executable\n");
			break;

		case ET_DYN:
			kprintf("Shared Object\n");
			break;
		default:
			kprintf("Unknown (0x%x)\n", elf_header.e_type);
			break;
	}

	kprintf("Machine\t\t= ");
	switch(elf_header.e_machine)
	{
		case EM_NONE:
			kprintf("None (0x0)\n");
			break;

		case EM_386:
			kprintf("INTEL x86 (0x%x)\n", EM_386);
			break;

		case EM_ARM:
			kprintf("ARM (0x%x)\n", EM_ARM);
			break;
		default:
			kprintf("Machine\t= 0x%x\n", elf_header.e_machine);
			break;
	}

	// Entry point 
	kprintf("Entry point\t= 0x%08x\n", elf_header.e_entry);

	// ELF header size in bytes 
	kprintf("ELF header size\t= 0x%08x\n", elf_header.e_ehsize);

	 
	kprintf("\nProgram Header\t= ");
	kprintf("0x%08x\n", elf_header.e_phoff);		// start 
	kprintf("\t\t  %d entries\n", elf_header.e_phnum);	// num entry 
	kprintf("\t\t  %d bytes\n", elf_header.e_phentsize);	// size/entry 

	
	kprintf("\nSection Header\t= ");
	kprintf("0x%08x\n", elf_header.e_shoff);	 
	kprintf("\t\t  %d entries\n", elf_header.e_shnum);	
	kprintf("\t\t  %d bytes\n", elf_header.e_shentsize);	 
	kprintf("\t\t  0x%08x (string table offset)\n", elf_header.e_shstrndx);

	
	kprintf("\nFile flags \t= 0x%08x\n", elf_header.e_flags);

	 
	int32 ef = elf_header.e_flags;
	kprintf("\t\t  ");
	kprintf("\n");

	kprintf("ARM EABI\t= Version %d\n", (ef & EF_ARM_EABI_MASK)>>24);

	kprintf("\n");	
}



void read_section_header_table(int32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
	uint32 i;

	if(seek(fd, eh.e_shoff) == eh.e_shoff){
		kprintf("Success\n");
	}

	for(i=0; i<eh.e_shnum; i++) {
		read(fd, (void *)&sh_table[i], eh.e_shentsize);
	}

}

char * read_section(int32 fd, Elf32_Shdr sh)
{
	char* buff = (char *)getmem(sh.sh_size);
	if(!buff) {
		kprintf(":Failed to allocate %dbytes\n",sh.sh_size);
	}
	char *ptr = buff;
	int count = sh.sh_size;

	if(buff != NULL){
		seek(fd, sh.sh_offset);
		while(count > 1024){
			read(fd, (void *)ptr, 1024);
			count = count - 1024;	
			ptr = ptr + 1024;
		}
		read(fd, (void *)ptr, count); 
	}
	return buff;
}

void print_section_headers(int32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
	uint32 i;
	char* sh_str;	//section-header string-table is also a section. 

	// Read section-header string-table 
	kprintf("eh.e_shstrndx = 0x%x\n", eh.e_shstrndx);
	sh_str = read_section(fd, sh_table[eh.e_shstrndx]);

	kprintf("========================================\n");
	kprintf(" idx offset     load-addr  size       algn"
		 " flags      type       section\n");
	kprintf("========================================\n");
	
	for(i=0; i<eh.e_shnum; i++) {
		kprintf(" %03d ", i);
		kprintf("0x%08x ", sh_table[i].sh_offset);
		kprintf("0x%08x ", sh_table[i].sh_addr);
		kprintf("0x%08x ", sh_table[i].sh_size);
		kprintf("%4d ", sh_table[i].sh_addralign);
		kprintf("0x%08x ", sh_table[i].sh_flags);
		kprintf("0x%08x ", sh_table[i].sh_type);
		kprintf("%s\t", (sh_str + sh_table[i].sh_name));
		kprintf("\n");
	}
	kprintf("....................................");
	
	kprintf("\n");	
}

void print_symbol_table(int32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[], uint32 symbol_table)
{

	char *str_tbl;
	Elf32_Sym* sym_tbl;
	uint32 i, symbol_count;

	sym_tbl = (Elf32_Sym*)read_section(fd, sh_table[symbol_table]);

	
	 // Section containing the string table having names of
	// symbols of this section
	 
	uint32 str_tbl_ndx = sh_table[symbol_table].sh_link;
	kprintf("str_table_ndx = 0x%x\n", str_tbl_ndx);
	str_tbl = read_section(fd, sh_table[str_tbl_ndx]);

	symbol_count = (sh_table[symbol_table].sh_size/sizeof(Elf32_Sym));
	kprintf("%d symbols\n", symbol_count);

	for(i=0; i< symbol_count; i++) {
		kprintf("0x%08x ", sym_tbl[i].st_value);
		kprintf("0x%02x ", ELF32_ST_BIND(sym_tbl[i].st_info));
		kprintf("0x%02x ", ELF32_ST_TYPE(sym_tbl[i].st_info));
		kprintf("%s\n", (str_tbl + sym_tbl[i].st_name));
	}
}

char *lookup_symbol_table(int32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[], uint32 symbol_table, const char *name)
{

	char *str_tbl;
	Elf32_Sym* sym_tbl;
	uint32 i, symbol_count;

	sym_tbl = (Elf32_Sym*)read_section(fd, sh_table[symbol_table]);

	
	 // Section containing the string table having names of
	// symbols of this section
	 
	uint32 str_tbl_ndx = sh_table[symbol_table].sh_link;
	kprintf("str_table_ndx = 0x%x\n", str_tbl_ndx);
	str_tbl = read_section(fd, sh_table[str_tbl_ndx]);

	symbol_count = (sh_table[symbol_table].sh_size/sizeof(Elf32_Sym));
	kprintf("%d symbols\n", symbol_count);
	kprintf("%s...----\n", name);
	for(i=0; i< symbol_count; i++) {
		int len = strlen((char *)name);
		if((strncmp(str_tbl + sym_tbl[i].st_name, name, len) == 0) && (len == strlen(str_tbl + sym_tbl[i].st_name))){
			kprintf("%d\n", i);
			kprintf("0x%08x ", sym_tbl[i].st_value);
			kprintf("0x%02x ", ELF32_ST_BIND(sym_tbl[i].st_info));
			kprintf("0x%02x ", ELF32_ST_TYPE(sym_tbl[i].st_info));
			kprintf("%s\n", (str_tbl + sym_tbl[i].st_name));
			return (char *)sym_tbl[i].st_value;
		}
	}
	return NULL;
}

void print_symbols(int32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
	uint32 i;

	for(i=0; i<eh.e_shnum; i++) {
		if ((sh_table[i].sh_type==SHT_SYMTAB)
		 || (sh_table[i].sh_type==SHT_DYNSYM)) {
			kprintf("\n[Section %03d]", i);
			print_symbol_table(fd, eh, sh_table, i);
		}
	}
}

char* load_file(int fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[]){

	int32 filesize = control(RFILESYS, RFS_CTL_SIZE, fd, 0);
	char* buff = (char *)getmem(filesize);
	if(!buff) {
		kprintf(":Failed to allocate %dbytes\n",filesize);
	}
	char *ptr = buff;
	int count = filesize;

	if(buff != NULL){
		seek(fd, 0);
		while(count > 1024){
			read(fd, (void *)ptr, 1024);
			count = count - 1024;	
			ptr = ptr + 1024;
		}
		read(fd, (void *)ptr, count); 
	}
	kprintf("done with loading...\n");
	return buff;
}

Elf32_Sym* modify_symtab(int fd, char *start, Elf32_Ehdr eh, Elf32_Shdr sh_table[], uint32* symtbl_idx){
	uint32 i,j, symbol_count;
	
	char *str_tbl;
	Elf32_Sym* sym_tbl;

	for(i=0; i<eh.e_shnum; i++) {
		if ((sh_table[i].sh_type==SHT_SYMTAB) || (sh_table[i].sh_type==SHT_DYNSYM)) {
			kprintf("\n[Section %03d]", i);
			*symtbl_idx = i;
			
			sym_tbl = (Elf32_Sym *)read_section(fd, sh_table[i]);

	
			 // Section containing the string table having names of
			// symbols of this section
	 
			uint32 str_tbl_ndx = sh_table[i].sh_link;
			kprintf("str_table_ndx = 0x%x\n", str_tbl_ndx);
			str_tbl = read_section(fd, sh_table[str_tbl_ndx]);	

			symbol_count = (sh_table[i].sh_size/sizeof(Elf32_Sym));
			kprintf("%d symbols\n", symbol_count);

			for(j=0; j< symbol_count; j++) {
				sym_tbl[j].st_value = sym_tbl[j].st_value + sh_table[sym_tbl[j].st_shndx].sh_offset + (uint32)start;
				kprintf("0x%08x ", start);
				kprintf("0x%08x ", sh_table[i].sh_offset);
				kprintf("0x%08x ", sym_tbl[j].st_value);
				kprintf("0x%08x ", sym_tbl[j].st_shndx);
				kprintf("0x%02x ", ELF32_ST_BIND(sym_tbl[j].st_info));
				kprintf("0x%02x ", ELF32_ST_TYPE(sym_tbl[j].st_info));
				kprintf("0x%08x ", sym_tbl[j].st_name);
				char *name = str_tbl + sym_tbl[j].st_name;
				if(ELF32_ST_TYPE(sym_tbl[j].st_info) == STT_FUNC){
					
					func_info[func_size].add = sym_tbl[j].st_value;
					strncpy(func_info[func_size].str, name, strlen(name));	
					kprintf("storing...%s\n", func_info[func_size].str);
					kprintf("%d..\n", func_size);
					func_size += 1;		
				}
				kprintf("%s\n", name);
				if(strncmp("main", (const char *)name, 4) == 0){
					kprintf("%s\n", name);
					entry = (void *)sym_tbl[j].st_value;		
					kprintf("0x%08x ", entry);

				}
			}

			return sym_tbl;
		}
	}
	return (void *)SYSERR;	
}


/*

void text_section(int32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
	uint32 i;	
	char* sh_str;	 
	char* buf;	// buffer for contents of the .text section 

	//  string-table header
	kprintf("eh.e_shstrndx = 0x%x\n", eh.e_shstrndx);
	sh_str = read_section(fd, sh_table[eh.e_shstrndx]);

	for(i=0; i<eh.e_shnum; i++) {
		if(!strncmp(".text", (sh_str + sh_table[i].sh_name),5)) {
			kprintf("Found section\t\".text\"\n");
			kprintf("at offset\t0x%08x\n", sh_table[i].sh_offset);
			kprintf("of size\t\t0x%08x\n", sh_table[i].sh_size);

			break;
		}
	}

	seek(fd, sh_table[i].sh_offset);
	buf = (char*)getmem(sh_table[i].sh_size);
	
	if(!buf) {
		kprintf("Failed  %dbytes\n allocation", sh_table[i].sh_size);
	}
	kprintf("0x%08x ...\n", sh_table[i].sh_offset);
	read(fd, buf, sh_table[i].sh_size);
	
}
*/
char *elf_lookup_symbol(const char *name) {
	uint32 i;

	for(i=0; i<SYMSIZE; i++) {
		/*if ((sh_table[i].sh_type==SHT_SYMTAB) || (sh_table[i].sh_type==SHT_DYNSYM)) {
			kprintf("\n[Section %03d]", i);
			return lookup_symbol_table(fd, *hdr, sh_table, i, name);
		}*/
		if((strncmp(xinu_info[i].str, name, strlen((char *)name)) == 0) && (strlen((char *)name) == strlen(xinu_info[i].str))){
			kprintf("%s..newyo\n", name);
			kprintf("0x%08x ...ne\n", xinu_info[i].add);
			return (char *)xinu_info[i].add;
		}	
	}
	return NULL;
}

/*
int elf_load_stage1(int fd, Elf32_Ehdr *hdr, Elf32_Shdr *shdr) {

	read_section_header_table(fd, *hdr, shdr);
 
	unsigned int i,j;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 		kprintf("0x%08x....hello\n", section->sh_type);
		// If the section isn't present in the file
		if(section->sh_type == SHT_NOBITS) {
			// Skip if it the section is empty
			if(!section->sh_size) continue;
			// If the section should appear in memory
			if(section->sh_flags & SHF_ALLOC) {
				kprintf("%s\n", section->sh_name);
				// Allocate and zero some memory
				char *mem = getmem(section->sh_size);
				for(j=0;j<section->sh_size;j++){
 					mem[j] = 0;
 				}
				mem = (void *)mem;
				// Assign the memory offset to the section offset
				section->sh_offset = (int)mem - (int)hdr;
			}
		}
	}
	return 0;
}
*/
int store_xinuinfo(int gfd){
	
	char *str_tbl;
	Elf32_Sym* sym_tbl;
	uint32 i, symbol_count;
	Elf32_Ehdr new_ehdr;
	uint32 j;

	read_elf_header(gfd, &new_ehdr);
 	Elf32_Shdr *new_sh_tbl = (Elf32_Shdr *)getmem(new_ehdr.e_shentsize * new_ehdr.e_shnum);
	if(!new_sh_tbl) {
		kprintf("Failed to allocate %d bytes\n",(new_ehdr.e_shentsize * new_ehdr.e_shnum));
	}
	read_section_header_table(gfd, new_ehdr, new_sh_tbl);

	for(i=0; i<new_ehdr.e_shnum; i++) {
		if ((new_sh_tbl[i].sh_type==SHT_SYMTAB) || (new_sh_tbl[i].sh_type==SHT_DYNSYM)) {
			kprintf("\n[Section %03d]", i);
			sym_tbl = (Elf32_Sym*)read_section(gfd, new_sh_tbl[i]);

	
			 // Section containing the string table having names of
			// symbols of this section
	 
			uint32 str_tbl_ndx = new_sh_tbl[i].sh_link;
			kprintf("str_table_ndx = 0x%x\n", str_tbl_ndx);
			str_tbl = read_section(gfd, new_sh_tbl[str_tbl_ndx]);

			symbol_count = (new_sh_tbl[i].sh_size/sizeof(Elf32_Sym));
			kprintf("%d symbols\n", symbol_count);
			//kprintf("%s...----\n", name);
			for(j=0; j< symbol_count; j++) {
				int len = strlen(str_tbl + sym_tbl[j].st_name);
				kprintf("%d\n", j);
				kprintf("0x%08x ", sym_tbl[j].st_value);
				kprintf("0x%02x ", ELF32_ST_BIND(sym_tbl[j].st_info));
				kprintf("0x%02x ", ELF32_ST_TYPE(sym_tbl[j].st_info));
				kprintf("%s\n", (str_tbl + sym_tbl[j].st_name));
				strncpy(xinu_info[j].str, (str_tbl + sym_tbl[j].st_name), len);
				xinu_info[j].add = sym_tbl[j].st_value;
				kprintf("0x%08x ", xinu_info[j].add);	
			}

			xinu_size = symbol_count;
		}
	}

	return 0;
}

int elf_get_symval(int fd, char* start, Elf32_Ehdr *hdr, Elf32_Shdr sh_table[], Elf32_Sym *symtab, int strtbl_idx, uint32 symtbl_idx, uint32 idx) {
	//Elf32_Ehdr new_ehdr;
	
	if(symtbl_idx == SHN_UNDEF || idx == SHN_UNDEF) return 0;
	kprintf("%d..%d.%d\n", strtbl_idx, symtbl_idx, idx);
	uint32 symtab_entries = (sh_table[symtbl_idx].sh_size/sizeof(Elf32_Sym));
	if(idx >= symtab_entries) {
		kprintf("Symbol Index out of Range (%d:%u).\n", strtbl_idx, idx);
		return ELF_RELOC_ERR;
	}
	 
	//int symaddr = (int)start + sh_table[symtbl_idx].sh_offset;
	Elf32_Sym *symbol = &(symtab[idx]);
	kprintf("yoooooooooooooooooooo%d\n", symbol->st_size);
	kprintf("0x%02x ", ELF32_ST_BIND(symbol->st_info));
	kprintf("0x%02x \n", ELF32_ST_TYPE(symbol->st_info));
	
	char *strtab = read_section(fd, sh_table[strtbl_idx]);
	kprintf("reading strtab done\n");		
	const char *name = (const char *)(strtab + symbol->st_name);
	kprintf("0x%08x ", symbol->st_name);
	kprintf("name please %s...----\n", name);
 		

	if(symbol->st_shndx == SHN_UNDEF) {
		// External symbol, lookup value
		
		//kprintf("0x%08x ", strtab->sh_offset);		
		kprintf("hello.....\n");
		
		kprintf("continuing...\n");
 		/*
 		read_elf_header(gfd, &new_ehdr);
 		Elf32_Shdr *new_sh_tbl = (Elf32_Shdr *)getmem(new_ehdr.e_shentsize * new_ehdr.e_shnum);
		if(!new_sh_tbl) {
			kprintf("Failed to allocate %d bytes\n",(new_ehdr.e_shentsize * new_ehdr.e_shnum));
		}
		read_section_header_table(gfd, new_ehdr, new_sh_tbl);
		void *target = elf_lookup_symbol(gfd, &new_ehdr, new_sh_tbl, name);
 		*/
 		void *target = elf_lookup_symbol(name);
		if(target == NULL) {
			// Extern symbol not found
			if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK) {
				// Weak symbol initialized as 0
				return 0;
			} else {
				kprintf("Undefined External Symbol : %s.\n", name);
				return ELF_RELOC_ERR;
			}
		} else {
			return (int)target;
		}
	} 
	//else if(symbol->st_shndx == SHN_ABS) {
		// Absolute symbol
	//	return symbol->st_value;
	//} 
	else {
		/*
		// Internally defined symbol
		//Elf32_Shdr *target = (Elf32_Shdr *)read_section(fd, sh_table[symbol->symtbl_idx]);
		//return (int)start + symbol->st_value + target->sh_offset;
		if(name == NULL){
			char *sh_str = read_section(fd, sh_table[(*hdr).e_shstrndx]);
			name = (const char *)(sh_str + sh_table[idx].sh_name);
			kprintf("QQQQQQQ....%s\n", name);			
			if(name != NULL){
				return (int)(start + sh_table[idx].sh_offset);	
			}	
		} 
		*/
		return symbol->st_value;
		
		//return -1;
	}
}

int elf_do_reloc(int fd, char* start, Elf32_Ehdr *hdr, Elf32_Shdr sh_table[], Elf32_Rel *rel, Elf32_Shdr *reltab, Elf32_Sym *symtab, uint32 symtbl_idx) {
 	kprintf("there...\n");
	int addr = (int)start + sh_table[reltab->sh_info].sh_offset;
	kprintf("%d..\n", reltab->sh_info);
	int *ref = (int *)(addr + rel->r_offset);
	kprintf("0x%08x \n", rel->r_offset);
	kprintf("%d..\n", addr);
	// Symbol value
	int symval = 0;
	if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
		//check with section name first if matches, then start + section offset
		//if not comparing then get_symval

		symval = elf_get_symval(fd, start, hdr, sh_table, symtab, sh_table[symtbl_idx].sh_link, symtbl_idx, ELF32_R_SYM(rel->r_info));
		kprintf("symval....%d\n", symval);		
		if(symval == ELF_RELOC_ERR) {
			kprintf("don't know\n");
			return ELF_RELOC_ERR;
		}
	}

	
	// Relocate based on type
	switch(ELF32_R_TYPE(rel->r_info)) {
		case R_386_NONE:
			// No relocation
			break;
		case R_386_32:
			kprintf("yo...0x%08x\n", (void *)symval);
			// Symbol + Offset
			*ref = DO_386_32(symval, *ref);
			break;
		case R_386_PC32:
			kprintf("hello...0x%08x\n", (void *)symval);
			kprintf("%d..should be -4\n", *ref);
			kprintf("0x%08x\n", (void *)ref);
			// Symbol + Offset - Section Offset
			*ref = DO_386_PC32(symval, *ref, (int)ref);
			break;
		default:
			// Relocation type not supported, display error and return
			kprintf("Unsupported Relocation Type (%d).\n", ELF32_R_TYPE(rel->r_info));
			return ELF_RELOC_ERR;
	}
	
	return symval;
}


void* elf_load(int fd, char *start, Elf32_Ehdr *hdr, Elf32_Shdr *shdr, Elf32_Sym *symtab, uint32 symtbl_idx) {
	
	//read_section_header_table(fd, *hdr, shdr);
 
	unsigned int i, idx;
	kprintf("fd...is..%d\n", fd);
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) {
		Elf32_Shdr *section = &shdr[i];
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
			kprintf("%d\n", section->sh_size / section->sh_entsize);
			// Process each entry in the table
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)start + section->sh_offset))[idx];
				int result = elf_do_reloc(fd, start, hdr, shdr, reltab, section, symtab, symtbl_idx);
				// On error, display a message and return
				/*
				if(result == ELF_RELOC_ERR) {
					kprintf("Failed to relocate symbol.\n");
					return (void *)-1;
				}
				*/
			}
		}
	}
	return entry;
}
/*
void *elf_load_rel(int fd, Elf32_Ehdr *hdr, Elf32_Shdr* sh_tbl) {
	int result;
	result = elf_load_stage1(fd, hdr, sh_tbl);
	if(result == ELF_RELOC_ERR) {
		kprintf("Unable to load ELF file.\n");
		return NULL;
	}
	
	//result = elf_load(fd, hdr, &(*sh_tbl));
	if(result == ELF_RELOC_ERR) {
		kprintf("Unable to load ELF file.\n");
		return NULL;
	}
	// TODO : Parse the program header (if present)
		
	return (void *)hdr->e_entry;
}

 
void * elf_load_file(int fd, Elf32_Ehdr *hdr, Elf32_Shdr* sh_tbl) {
	if(!is_ELF(*hdr)) {
		kprintf("ELF File cannot be loaded.\n");
		return NULL;
	}
	switch(hdr->e_type) {
		case ET_EXEC:
			// TODO : Implement
			return NULL;
		case ET_REL:
			return elf_load_rel(fd, hdr, sh_tbl);
	}
	return NULL;
}
*/

void * parse(int fd)
{

	Elf32_Ehdr eh;		
	Elf32_Shdr* sh_tbl;
	void* result;
	uint32 symtbl_idx;	

	gfd = open(RFILESYS, "xinu.elf", "or");
	
	if(fd<0 || gfd < 0) {
		kprintf("error...... %d %dUnable to open\n", fd, gfd);
		return 0;
	}
	kprintf("%d %dable to open\n", fd, gfd);
	store_xinuinfo(gfd);
//-----------------------------
/*
	// ELF header 
	read_elf_header(gfd, &eh);
	
	if(!is_ELF(eh)) {
		return -1;
	}
	print_elf_header(eh);

	  
	sh_tbl = (Elf32_Shdr *)getmem(eh.e_shentsize * eh.e_shnum);
	if(!sh_tbl) {
		kprintf("Failed to allocate %d bytes\n",(eh.e_shentsize * eh.e_shnum));
	}
	read_section_header_table(gfd, eh, sh_tbl);
	print_section_headers(gfd, eh, sh_tbl);

	
	print_symbols(gfd, eh, sh_tbl);
*/
//----------------------------


	// ELF header 
	read_elf_header(fd, &eh);
	
	if(!is_ELF(eh)) {
		return (void *)-1;
	}
	print_elf_header(eh);

	  
	sh_tbl = (Elf32_Shdr *)getmem(eh.e_shentsize * eh.e_shnum);
	if(!sh_tbl) {
		kprintf("Failed to allocate %d bytes\n",(eh.e_shentsize * eh.e_shnum));
	}
	read_section_header_table(fd, eh, sh_tbl);
	print_section_headers(fd, eh, sh_tbl);

	
	print_symbols(fd, eh, sh_tbl);


	
	//elf_load_file(fd, &eh, sh_tbl);

	char *add = load_file(fd, eh, sh_tbl);

	if(!is_ELF(*(Elf32_Ehdr *)add)) {
		kprintf("ELF File cannot be loaded.\n");
		return NULL;
	}

	Elf32_Sym* symtab = modify_symtab(fd, add, eh, sh_tbl, &symtbl_idx);

	kprintf("stage 2 beginning\n");
	result = elf_load(fd, add, &eh, sh_tbl, symtab, symtbl_idx);
	if(result == (void *)ELF_RELOC_ERR) {
		kprintf("Unable to load ELF file.\n");
		return NULL;
	}

	kprintf("finish\n");
	
		  
	//text_section(fd, eh, sh_tbl);

	
	close(gfd);
	kprintf("0x%08x ", result);
	kprintf("0x%08x ", entry);

	return result;
}

