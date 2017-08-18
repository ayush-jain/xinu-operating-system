#include <xinu.h>

int main(){
	
	int rc;
	int32 dirfd = open(RFILESYS, ".", "ro");
	struct rfdirent rfdentry;


	rc = read(dirfd, (char *)&rfdentry, sizeof(struct rfdirent));
	while(rc != 0){
		if(rc == SYSERR) { // error accured while reading 
			kprintf("Error\n");
			return -1;
		} else { // Handle next entry in the directory 
			if(rfdentry.d_type == 1){
				kprintf("%s\n", rfdentry.d_name);	
			} else {
				kprintf("%s/\n", rfdentry.d_name);
			}			
			//read fields of rfdirent
		}
		rc = read(dirfd, (char *)&rfdentry, sizeof(struct rfdirent));	
	}
	
	close(rc);	
	return OK;
}

