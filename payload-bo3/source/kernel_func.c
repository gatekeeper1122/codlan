#include "kernel_func.h"

////////////////////////////
// NEEDED PART
////////////////////////////

uint64_t rdmsr(unsigned int msr_index) {
	uint32_t low, high;
	__asm__ __volatile__("rdmsr" : "=a" (low), "=d" (high) : "c" (msr_index));
	return (low | ((uint64_t)high << 32));
}

uint8_t* get_kptr() {
	return (uint8_t *)(rdmsr(0xC0000082) - KERN_XFAST_SYSCALL);
}

// Jailbreak PS4 :)
int jailbreak(struct thread *td) {
	uint8_t* ptrKernel = get_kptr();
	struct ucred* cred = td->td_proc->p_ucred;
	struct filedesc* fd = td->td_proc->p_fd;

	// Escalate privileges
	cred->cr_uid = 0;
	cred->cr_ruid = 0;
	cred->cr_rgid = 0;
	cred->cr_groups[0] = 0;

	// Escape sandbox
	void** prison0 = (void**)&ptrKernel[KERN_PRISON_0];
	void** rootvnode = (void**)&ptrKernel[KERN_ROOTVNODE];
	cred->cr_prison = *prison0;
	fd->fd_rdir = fd->fd_jdir = *rootvnode;

	void *td_ucred = *(void **)(((char *)td) + 304); // p_ucred == td_ucred

	// sceSblACMgrIsSystemUcred
	uint64_t *sonyCred = (uint64_t *)(((char *)td_ucred) + 96);
	*sonyCred = 0xFFFFFFFFFFFFFFFFULL;

	// sceSblACMgrGetDeviceAccessType
	uint64_t *sceProcType = (uint64_t *)(((char *)td_ucred) + 88);
	*sceProcType = 0x3801000000000013; // Max access

	// sceSblACMgrHasSceProcessCapability
	uint64_t *sceProcCap = (uint64_t *)(((char *)td_ucred) + 104);
	*sceProcCap = 0xFFFFFFFFFFFFFFFFULL; // Sce Process

	// Disable write protection
	uint64_t cr0 = readCr0();
	writeCr0(cr0 & ~X86_CR0_WP);

	*(uint16_t *)&ptrKernel[KERN_PROCESS_ASLR] = 0x9090;

	// allow kmem_alloc to map rwx
	*(uint8_t *)&ptrKernel[0xFCD48] = 0x7;

	// debug dlsym
	*(uint8_t *)&ptrKernel[0x237E95] = 0x50;

	// Allow mount busy device
	*(uint16_t *)&ptrKernel[0x1DEEDB] = 0x9090;

	// Enable write protection
	writeCr0(cr0);

	return 0;
}

// Remote sandbox escape (without root edit) and add root to the sandbox
int remote_escape(struct thread *td) {
	uint32_t low, high;
	__asm__ __volatile__("rdmsr" : "=a" (low), "=d" (high) : "c" (0xC0000082));
	uint8_t* ptrKernel = ( (low | ((uint64_t)high << 32)) - KERN_XFAST_SYSCALL);

	struct ucred* cred = td->td_proc->p_ucred;
	struct filedesc* fd = td->td_proc->p_fd;

	// Escalate privileges
	cred->cr_uid = 0;
	cred->cr_ruid = 0;
	cred->cr_rgid = 0;
	cred->cr_groups[0] = 0;

	// Escape sandbox
	void** prison0 = (void**)&ptrKernel[KERN_PRISON_0];
	void** rootvnode = (void**)&ptrKernel[KERN_ROOTVNODE];
	cred->cr_prison = *prison0;
	fd->fd_jdir = NULL;

	// Max access
	void *td_ucred = *(void **)(((char *)td) + 304); // p_ucred == td_ucred

	// sceSblACMgrIsSystemUcred
	uint64_t *sonyCred = (uint64_t *)(((char *)td_ucred) + 96);
	*sonyCred = 0xFFFFFFFFFFFFFFFFULL;

	// sceSblACMgrGetDeviceAccessType
	uint64_t *sceProcType = (uint64_t *)(((char *)td_ucred) + 88);
	*sceProcType = 0x3801000000000013; // Max access

	// sceSblACMgrHasSceProcessCapability
	uint64_t *sceProcCap = (uint64_t *)(((char *)td_ucred) + 104);
	*sceProcCap = 0xFFFFFFFFFFFFFFFFULL; // Sce Process

	return 0;
}

// Get proc by pid
struct proc *proc_find_by_pid(int pid) {
	struct proc *p;

	uint8_t* ptrKernel = get_kptr();
	p = *(struct proc **)&ptrKernel[KERN_ALLPROC];

	do {
		if (p->pid == pid) {
			return p;
		}
	} while ((p = p->p_forw));

	return NULL;
}

////////////////////////////
// KERNEL FUNCTION
////////////////////////////

// Allocate memory in remote process
void* kern_process_allocate(int pid, uint64_t size) {
	void* addr = 0;
	int r = 0;

	int8_t* ptrKernel = get_kptr();

	int (*vm_map_insert)(struct vm_map * map, uint64_t object, uint64_t offset, uint64_t start, uint64_t end, int prot, int max, int cow) = (void *)&ptrKernel[KERN_VM_MAP_INSERT];
	int (*vm_map_findspace) (vm_map_t map, vm_offset_t start, vm_size_t length, vm_offset_t *addr) = (void *)&ptrKernel[KERN_VM_MAP_FINDSPACE];
	void (*vm_map_lock)(struct vm_map * map) = (void *)&ptrKernel[KERN_VM_MAP_LOCK];
	void (*vm_map_unlock)(struct vm_map * map) = (void *)&ptrKernel[KERN_VM_MAP_UNLOCK];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	struct proc* p = proc_find_by_pid(pid);
	if (!p) {
		return 0;
	}

	struct thread* rtd = p->p_threads.tqh_first;
	struct vmspace *vm = p->p_vmspace;
	struct vm_map *map = &vm->vm_map;

	printfkernel("locking map ...\n");
	vm_map_lock(map);

	printfkernel("size: %lu\n", size);

	printfkernel("finding space in map ...\n");
	r = vm_map_findspace(map, 0x400000, size, &addr);
	if (r) {
		addr = 0xFFFFFFFFFFFFFFFF;
		vm_map_unlock(map);
		return addr;
	}

	printfkernel("inserting space in map ...\n");
	r = vm_map_insert(map, NULL, NULL, (uint64_t)addr, (uint64_t)(addr + size), VM_PROT_ALL, VM_PROT_ALL, 0);
	if (r) {
		addr = 0xFFFFFFFFFFFFFFFF;
		vm_map_unlock(map); 
		return addr;
	}

	printfkernel("unlocking space in map ...\n");
	vm_map_unlock(map);

	return addr;
}

// Deallocate memory in remote process
void kern_process_deallocate(int pid, uint64_t data, uint64_t size) {
	int8_t* ptrKernel = get_kptr();

	void (*vm_map_lock)(struct vm_map * map) = (void *)&ptrKernel[KERN_VM_MAP_LOCK];
	void (*vm_map_unlock)(struct vm_map * map) = (void *)&ptrKernel[KERN_VM_MAP_UNLOCK];
	int (*vm_map_delete)(struct vm_map * map, uint64_t start, uint64_t end) = (void *)&ptrKernel[KERN_VM_MAP_DELETE];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	struct proc* p = proc_find_by_pid(pid);
	if (!p) {
		return 0;
	}

	struct thread* rtd = p->p_threads.tqh_first;
	struct vmspace *vm = p->p_vmspace;
	struct vm_map *map = &vm->vm_map;

	vm_map_lock(map);

	vm_map_delete(map, data, data + size);

	vm_map_unlock(map);
}

// Read/Write memory inside remote process
void kern_process_readwrite_mem(struct thread* td, int pid, uint64_t offset, void* data, uint64_t size, int is_write) {
	int8_t* ptrKernel = get_kptr();

	int (*proc_rwmem)(struct proc *p, struct uio *uio) = (void *)&ptrKernel[KERN_PROC_RWMEM];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	struct proc* p = proc_find_by_pid(pid);
	if (!p) {
		printfkernel("k_process_read_mem: process not found\n");
		return 0;
	}

	struct uio uio;
	struct iovec iov;
	iov.iov_base = data;
	iov.iov_len = size;

	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = offset;
	uio.uio_resid = size;
	uio.uio_segflg = UIO_SYSSPACE;
	if (!is_write) {
		uio.uio_rw = UIO_READ;
	} else {
		uio.uio_rw = UIO_WRITE;
	}
	uio.uio_td = td;

	proc_rwmem(p, &uio);
}

// Get virtual memory map of remote process (free this after use)
Map* kern_process_vm_map(int pid, int* map_nbr) {
	struct vm_map_entry *entry;
	int error = 0;

	int8_t* ptrKernel = get_kptr();

	void *M_TEMP = (void *)&ptrKernel[KERN_M_TEMP];
	void* (*k_malloc)(unsigned long size, void *type, int flags) = (void *)&ptrKernel[KERN_MALLOC];
	void (*k_free)(void *addr, void *type) = (void *)&ptrKernel[KERN_FREE];
	void (*k_memcpy)(void *dst, const void *src, size_t len) = (void*)&ptrKernel[KERN_MEMCPY];
	struct vmspace *(*vmspace_acquire_ref)(struct proc *p) = (void *)&ptrKernel[KERN_VMSPACE_ACQUIRE_REF];
	int (*vm_map_lookup_entry)(struct vm_map *map, uint64_t address, struct vm_map_entry **entries) = (void *)&ptrKernel[KERN_VM_MAP_LOOKUP_ENTRY];
	void (*vmspace_free)(struct vmspace *vm) = (void *)&ptrKernel[KERN_VMSPACE_FREE];
	void (*vm_map_lock_read)(struct vm_map *map) = (void *)&ptrKernel[KERN_VM_MAP_LOCK_READ];
	void (*vm_map_unlock_read)(struct vm_map *map) = (void *)&ptrKernel[KERN_VM_MAP_UNLOCK_READ];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	printfkernel("kern_process_vm_map: process %i\n", pid);

	printfkernel("kern_process_vm_map: finding process ...\n");
	struct proc* p = proc_find_by_pid(pid);
	if (!p) {
		printfkernel("kern_process_vm_map: process not found\n");
		return 0;
	}
	printfkernel("kern_process_vm_map: process found !\n");

	struct vmspace *vm = vmspace_acquire_ref(p);
	if (!vm) {
		printfkernel("kern_process_vm_map: vmspace_acquire_ref failed\n");
		return 0;
	}
	printfkernel("kern_process_vm_map: vm space found !\n");

	struct vm_map *map = &vm->vm_map;

	int num = map->nentries;
	if (!num) {
		vmspace_free(vm);
		printfkernel("kern_process_vm_map: no entries found !\n");
		return 0;
	}

	vm_map_lock_read(map);
	if (vm_map_lookup_entry(map, NULL, &entry)) {
		*map_nbr = num;
		printfkernel("kern_process_vm_map: unable to read entrie !\n");

		vm_map_unlock_read(map);
		vmspace_free(vm);
		return 0;
	}

	printfkernel("kern_process_vm_map: checking all entries ...\n");

	Map* map_list = k_malloc(sizeof(Map)*num, M_TEMP, 2);

	for (int i = 0; i < num; i++) {
		int prot = entry->prot & (entry->prot >> 8);

		uint64_t start = (uint64_t)entry->start;
		uint64_t stop = (uint64_t)entry->end;

		map_list[i].start = start;
		map_list[i].stop = stop;
		map_list[i].protection = prot;
		k_memcpy(map_list[i].name, entry->name, 32);

		if (!(entry = entry->next)) {
			break;
		}
	}

	printfkernel("kern_process_vm_map: done (%i entries) \n", num);
	
	*map_nbr = num;
	vm_map_unlock_read(map);
	vmspace_free(vm);

	return map_list;
}

// Kill remote process
void kern_process_kill(int pid) {
	int8_t* ptrKernel = get_kptr();
	void (*killproc) (struct proc *p, char *why) = (void *)&ptrKernel[KERN_KILLPROC];

	struct proc* p = proc_find_by_pid(pid);
	if (!p) {
		return;
	}

	killproc(p, "kill");
}

////////////////////////////
// USERLAND WRAPPER
////////////////////////////

// Get process list
int process_list_wrapper(struct thread *td, struct kern_uap *args) {
	uint8_t* ptrKernel = get_kptr();

	int (*k_copyout) (const void *kaddr, void *uaddr, size_t len) = (void *)&ptrKernel[KERN_COPYOUT];
	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	// Get all argument via copyin (needed for fix a bug)
	struct kern_process_list_args process_args;
	k_copyin(args->uap, &process_args, sizeof(struct kern_process_list_args));

	uint64_t procCount = 0;
	struct proc* p = 0;

	p = *(struct proc **)&ptrKernel[KERN_ALLPROC];

	do {
		k_copyout(&p->pid, &process_args.process_list[procCount].pid, sizeof(int));
		k_copyout(p->p_comm, &process_args.process_list[procCount].name, 35);	
		k_copyout(p->p_appid, &process_args.process_list[procCount].app_id, 10);	

		procCount++;
	} while ((p = p->p_forw));

	k_copyout(&procCount, process_args.process_nbr, sizeof(int));
}

// Allocate memory on a userland process
int process_allocate_wrapper(struct thread* td, struct kern_uap* args) {
	uint64_t addr = 0;
	int r = 0;

	int8_t* ptrKernel = get_kptr();

	int (*k_copyout) (const void *kaddr, void *uaddr, size_t len) = (void *)&ptrKernel[KERN_COPYOUT];
	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	struct kern_process_allocate_args allocate_args;
	k_copyin(args->uap, &allocate_args, sizeof(struct kern_process_allocate_args));

	addr = kern_process_allocate(allocate_args.pid, allocate_args.size);

	k_copyout(&addr, allocate_args.return_ptr, sizeof(uint64_t));
	return 0;
}

// Deallocate momory on a userland process
int process_deallocate_wrapper(struct thread* td, struct kern_uap* args) {
	uint64_t addr = 0;

	int8_t* ptrKernel = get_kptr();

	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	struct kern_process_deallocate_args deallocate_args;
	k_copyin(args->uap, &deallocate_args, sizeof(struct kern_process_deallocate_args));

	kern_process_deallocate(deallocate_args.pid, deallocate_args.data_ptr, deallocate_args.size);

	return 0;
}

// Get thread list
int process_thread_wrapper(struct thread* td, struct kern_uap* args) {
	int8_t* ptrKernel = get_kptr();

	int (*k_copyout) (const void *kaddr, void *uaddr, size_t len) = (void *)&ptrKernel[KERN_COPYOUT];
	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	struct kern_process_thread_args thread_args;
	k_copyin(args->uap, &thread_args, sizeof(struct kern_process_thread_args));

	struct proc* p = proc_find_by_pid(thread_args.pid);
	if (!p) {
		return 0;
	}

	int thread_count = 0;
	struct thread* rtd = p->p_threads.tqh_first;

	do {
		k_copyout(&rtd->tid, &thread_args.thread_list[thread_count].tid, sizeof(int));
		k_copyout(rtd->td_name, &thread_args.thread_list[thread_count].name, 35);	

		thread_count++;
	} while ((rtd = rtd->td_plist.tqe_next));

	printfkernel("%i threads in process\n", thread_count);
	k_copyout(&thread_count, thread_args.thread_nbr, sizeof(int));
	return 0;
}

// Get VM Map list
int process_vm_map_wrapper(struct thread *td, struct kern_uap *args) {
	struct vm_map_entry *entry = NULL;
	int error = 0;

	int8_t* ptrKernel = get_kptr();

	int (*k_copyout) (const void *kaddr, void *uaddr, size_t len) = (void *)&ptrKernel[KERN_COPYOUT];
	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	// Get all argument via copyin (needed for fix a bug)
	struct kern_process_vm_map_args map_args;
	k_copyin(args->uap, &map_args, sizeof(struct kern_process_vm_map_args));

	int map_nbr = 0;
	Map* list = kern_process_vm_map(map_args.pid, &map_nbr);

	k_copyout(list, map_args.map_list, map_nbr*sizeof(Map));
	k_copyout(&map_nbr, map_args.map_nbr, sizeof(int));
	printfkernel("kern_process_vm_map: done (%i entries) \n", map_nbr);

	return 0;
}

// Write / Read memory
int process_readwrite_mem_wrapper(struct thread *td, struct kern_uap *args) {
	int8_t* ptrKernel = get_kptr();

	void *M_TEMP = (void *)&ptrKernel[KERN_M_TEMP];
	void* (*k_malloc)(unsigned long size, void *type, int flags) = (void *)&ptrKernel[KERN_MALLOC];
	void (*k_free)(void *addr, void *type) = (void *)&ptrKernel[KERN_FREE];
	int (*k_copyout) (const void *kaddr, void *uaddr, size_t len) = (void *)&ptrKernel[KERN_COPYOUT];
	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel)(const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	// Get all argument via copyin (needed for fix a bug)
	struct kern_process_readwrite_mem_args readwrite_mem_args;
	k_copyin(args->uap, &readwrite_mem_args, sizeof(struct kern_process_readwrite_mem_args));

	// Allocating space in kernel for read / write memory
	void* kbuff = k_malloc(readwrite_mem_args.size, M_TEMP, 2);

	// If is_write, copy data from user in kernel space
	if (readwrite_mem_args.is_write) {
		k_copyin(readwrite_mem_args.ubuff, kbuff, readwrite_mem_args.size);
	}

	kern_process_readwrite_mem(td, readwrite_mem_args.pid, readwrite_mem_args.offset, kbuff, readwrite_mem_args.size, readwrite_mem_args.is_write);

	// If not is_write, copy kernel data in user space
	if (!readwrite_mem_args.is_write) {
		k_copyout(kbuff, readwrite_mem_args.ubuff, readwrite_mem_args.size);
	}

	k_free(kbuff, M_TEMP);

	return 0;
}

// Kill process
int process_kill_wrapper(struct thread *td, struct kern_uap *args)
{
	int8_t* ptrKernel = get_kptr();

	int (*k_copyin) (const void *uaddr, void *kaddr, size_t len) = (void *)&ptrKernel[KERN_COPYIN];
	int (*printfkernel) (const char *fmt, ...) = (void *)&ptrKernel[KERN_PRINTF];

	// Get all argument via copyin (needed for fix a bug)
	struct kern_process_kill_args process_kill_args;
	k_copyin(args->uap, &process_kill_args, sizeof(struct kern_process_kill_args));

	kern_process_kill(process_kill_args.pid);

	return 0;
}
