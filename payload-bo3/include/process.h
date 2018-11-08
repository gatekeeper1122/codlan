#ifndef	_PROCESS_H
#define	_PROCESS_H

#include "ps4.h"
#include "sparse.h"

#define	CTL_KERN			1

#define	KERN_PROC			14
#define	KERN_PROC_VMMAP		32
#define	KERN_PROC_PATHNAME	12

#define	KVME_PROT_READ		0x00000001
#define	KVME_PROT_WRITE		0x00000002
#define	KVME_PROT_EXEC		0x00000004

typedef struct {
	int  pid;
	char name[35];
	char app_id[10];
} Process;

typedef struct {
	int tid;
	char name[35];
} Thread;

typedef struct {
	char name[32];
	uint64_t start;
	uint64_t stop;
	int protection;
} Map;

extern Process* process_get_list(int* proc_nbr);
extern Process process_by_name(char* name);
extern Map* process_vm_list(int pid, int* map_nbr);
extern void* process_allocate(int pid, uint64_t size);
extern void process_deallocate(int pid, void* data_addr, uint64_t size);
extern void process_read_mem(int pid, uint64_t offset, void* buffer, uint64_t size);
extern void process_write_mem(int pid, uint64_t offset, void* buffer, uint64_t size);
extern Thread* process_thread_list(int pid, int* thread_nbr);
extern void process_kill(int pid);

#endif