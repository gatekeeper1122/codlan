#include "ps4.h"
#include "sparse.h"
#include "process.h"
#include "freebsd.h"

typedef uintptr_t vm_offset_t;
typedef void* vm_map_t;

// 505
#define	KERN_XFAST_SYSCALL 0x1C0
#define KERN_PROCESS_ASLR 0x194875
#define KERN_PRISON_0 0x10986A0
#define KERN_ROOTVNODE 0x22C1A70
#define KERN_PRIV_CHECK 0x14A3990
#define KERN_PRIV_CHECK_CRED 0x2300B88
#define KERN_ALLOW_SYSTEM_LEVEL_DEBUGGING 0x1173D
#define KERN_COPYOUT 0x1EA630
#define KERN_COPYIN 0x1EA710
#define KERN_ALLPROC 0x2382FF8
#define KERN_PRINTF 0x436040
#define KERN_PROC_RWMEM 0x30D150
#define KERN_CREATE_THREAD 0x1BE1F0
#define KERN_KILLPROC 0xD41C0

#define	KERN_KMEM_ALLOC 0xFCC80
#define	KERN_KMEM_FREE 0xFCE50
#define KERN_KERNEL_MAP 0x1AC60E0

#define KERN_VMSPACE_ACQUIRE_REF 0x19EF90
#define KERN_VM_MAP_LOCK_READ 0x19f140
#define KERN_VM_MAP_UNLOCK_READ 0x19f190
#define	KERN_VMSPACE_ALLOC 0x19eb20
#define	KERN_VMSPACE_FREE 0x19edc0
#define KERN_VM_MAP_LOOKUP_ENTRY 0x19F760
#define KERN_VM_MAP_FINDSPACE 0x1A1F60
#define KERN_VM_MAP_INSERT 0x1A0280
#define KERN_VM_MAP_UNLOCK 0x19F060
#define KERN_VM_MAP_LOCK 0x19EFF0
#define KERN_VM_MAP_DELETE 0x1A19D0

#define KERN_M_TEMP 0x14B4110
#define KERN_FREE 0x10E460
#define KERN_MALLOC 0x10E250
#define KERN_STRCPY 0X8F250
#define KERN_STRCMP 0X1D0FD0
#define KERN_MEMCPY 0x1EA530
#define KERN_MEMSET 0x3205C0


#define KERN_SYS_MMAP 0x13D230
#define KERN_SYS_OPEN 0x33B990
#define KERN_SYS_DYNLIB_LOAD_PRX 0x237930

#define X86_CR0_WP (1 << 16)

struct kern_uap {
	uint64_t syscall;
	void* uap;
};

struct hook_string {
	char found[50];
	char rdi[20];
	char rsi[20];
	char rdx[20];
	char rcx[20];
	char r8[20];
	char r9[20];
	char show_string[50];
	char path_find[50];
	char new_path[50];
	char patch_done[50];
};


//////////////////
// Hook related
//////////////////

struct syscall_mmap {
	void *addr;
	size_t len;
	int prot;
	int flags;
	int fd;
	long pad;
	off_t pos;
};

struct syscall_open {
	const char *path;
	int flags;
	int mode;
};

struct kern_install_hook_args {
	uint64_t size;
	void* func_ptr;
};

//////////////////
// Process related
//////////////////

struct kern_process_list_args {
	int* process_nbr;
	Process* process_list;
};

struct kern_process_vm_map_args {
	int pid;
	int* map_nbr;
	Map* map_list;
};

struct kern_process_thread_args {
	int pid;
	int* thread_nbr;
	Thread* thread_list;
};

struct kern_process_allocate_args {
	int pid;
	uint64_t size;
	void* return_ptr;
};

struct kern_process_deallocate_args {
	int pid;
	uint64_t size;
	uint64_t data_ptr;
};

struct kern_process_readwrite_mem_args {
	int pid;
	uint64_t offset;
	uint64_t size;
	void* ubuff;
	int is_write;

};

struct kern_process_kill_args {
	int pid;
};

struct kern_create_thread_args {
	int pid;
	void* main_function;
	uint64_t function_size;
	uint64_t stack_size;
};

// Argument pour le syscall
struct sys_load_prx_args {
	char* lib_name;
	uint64_t unk1;
	int* idDestination;
	uint64_t unk2;
};

struct kern_process_load_prx_args {
	int pid;
	struct sys_load_prx_args* load_prx_args;
};


static inline __attribute__((always_inline)) uint64_t readCr0(void) {
	uint64_t cr0;
	
	__asm__ __volatile__ (
		"movq %0, %%cr0"
		: "=r" (cr0)
		: : "memory"
 	);
	
	return cr0;
}

static inline __attribute__((always_inline)) void writeCr0(uint64_t cr0) {
	__asm__ __volatile__ (
		"movq %%cr0, %0"
		: : "r" (cr0)
		: "memory"
	);
}

uint8_t* get_kptr();
int jailbreak(struct thread *td);
int remote_escape(struct thread *td);

// Kernel function
void* kern_process_allocate(int pid, uint64_t size);
void kern_process_deallocate(int pid, uint64_t data, uint64_t size);
void kern_process_readwrite_mem(struct thread* td, int pid, uint64_t offset, void* data, uint64_t size, int is_write);
Map* kern_process_vm_map(int pid, int* map_nbr);
void kern_process_kill(int pid);

// Process wrapper function
int process_list_wrapper(struct thread *td, struct kern_uap *args);
int process_allocate_wrapper(struct thread* td, struct kern_uap* args);
int process_deallocate_wrapper(struct thread* td, struct kern_uap* args);
int process_thread_wrapper(struct thread* td, struct kern_uap* args);
int process_vm_map_wrapper(struct thread *td, struct kern_uap *args);
int process_readwrite_mem_wrapper(struct thread *td, struct kern_uap *args);
int process_create_thread_wrapper(struct thread *td, struct kern_uap *args);
int process_kill_wrapper(struct thread *td, struct kern_uap *args);

