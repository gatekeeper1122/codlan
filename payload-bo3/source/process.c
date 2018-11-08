#include "debug.h"
#include "process.h"
#include "kernel_func.h"

Process* process_get_list(int* proc_nbr) {
	int nbr = -1;
	Process* list = (Process*)malloc(0x100000);

	struct kern_process_list_args process_list_args;
	process_list_args.process_nbr = &nbr;
	process_list_args.process_list = list;

	syscall(11, process_list_wrapper, &process_list_args);

	list = (Process*)realloc(list, sizeof(Process) * nbr);
	*proc_nbr = nbr;

	return (uint64_t)list;
}

Process process_by_name(char* name) {
	Process found;
	found.pid = -1;

	int proc_nbr = 0;	
	Process* process_list = (Process*)process_get_list(&proc_nbr);

	for (int i = 0; i < proc_nbr; i++) {
		if (!strcmp(name, process_list[i].name)) {
			found = process_list[i];
			break;
		}
	}	

	free(process_list);
	return found;
}

Map* process_vm_list(int pid, int* map_nbr) {
	int nbr = 0;
	Map* list = (Map*)malloc(0x100000);

	struct kern_process_vm_map_args process_vm_map_args;
	process_vm_map_args.pid = pid;
	process_vm_map_args.map_nbr = &nbr;
	process_vm_map_args.map_list = list;

	syscall(11, process_vm_map_wrapper, &process_vm_map_args);

	list = (Map*)realloc(list, sizeof(Map) * nbr);
	*map_nbr = nbr;
	return list;
}

Thread* process_thread_list(int pid, int* thread_nbr) {
	int nbr = -1;
	Thread* list = (Thread*)malloc(0x100000);

	struct kern_process_thread_args process_thread_args;
	process_thread_args.pid = pid;
	process_thread_args.thread_nbr = &nbr;
	process_thread_args.thread_list = list;

	syscall(11, process_thread_wrapper, &process_thread_args);

	list = (Thread*)realloc(list, sizeof(Thread) * nbr);
	*thread_nbr = nbr;
	return list;
}

void* process_allocate(int pid, uint64_t size) {
	void* addr = -1;

	struct kern_process_allocate_args process_allocate_args;
	process_allocate_args.pid = pid;
	process_allocate_args.size = size;
	process_allocate_args.return_ptr = &addr;

	syscall(11, process_allocate_wrapper, &process_allocate_args);
	return addr;
}

void process_deallocate(int pid, void* data_addr, uint64_t size) {
	struct kern_process_deallocate_args process_deallocate_args;
	process_deallocate_args.pid = pid;
	process_deallocate_args.size = size;
	process_deallocate_args.data_ptr = (uint64_t)data_addr;

	syscall(11, process_deallocate_wrapper, &process_deallocate_args);
}

void process_read_mem(int pid, uint64_t offset, void* buffer, uint64_t size) {
	struct kern_process_readwrite_mem_args read_mem_args;
	read_mem_args.pid = pid;
	read_mem_args.offset = offset;
	read_mem_args.size = size;
	read_mem_args.ubuff = buffer;
	read_mem_args.is_write = 0;

	syscall(11, process_readwrite_mem_wrapper, &read_mem_args);
}

void process_write_mem(int pid, uint64_t offset, void* buffer, uint64_t size) {
	struct kern_process_readwrite_mem_args write_mem_args;
	write_mem_args.pid = pid;
	write_mem_args.offset = offset;
	write_mem_args.size = size;
	write_mem_args.ubuff = buffer;
	write_mem_args.is_write = 1;

	syscall(11, process_readwrite_mem_wrapper, &write_mem_args);
}

void process_kill(int pid) {
	struct kern_process_kill_args kill_args;
	kill_args.pid = pid;

	syscall(11, process_kill_wrapper, &kill_args);
}
