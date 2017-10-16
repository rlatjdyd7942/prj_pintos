#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <kernel/console.h>
#include "devices/input.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "process.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void exit(int status) {
	struct thread *cur = thread_current();
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_exit();
}

static void check_valid (void *ptr) {
	if (!is_user_vaddr(ptr)) {
		exit (-1);
	}
}

static void get_args(uint32_t *esp, int count, uint32_t **args) {
	int i = 0;
	esp += 1;
	while (i < count) {
		args[i++] = *(uint32_t*)esp;
		++esp;
	}
}

static int pibonacci(int n) {
	int a, b, temp;
	if (n == 1 || n == 2) {
		return 1;
	} else {
		a = b = 1;
		while (n-- > 2) {
			temp = b;
			b += a;
			a = temp;
		}
		return b;
	}

}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	uint32_t *args[4];
	uint32_t *esp = f->esp;
	int syscall_num = *esp;
	struct thread *t = thread_current();
	int i;

	printf("\nsyscall %d\n", syscall_num);

	switch (syscall_num) {
	case SYS_HALT:
		shutdown_power_off();
		break;
	case SYS_EXIT:
		get_args(esp, 1, &args);
		t->exit_status = (int)args[0];
		exit(f->eax);
		thread_exit();
		break;
	case SYS_EXEC:
		get_args(esp, 1, &args);
		f->eax = process_execute(args[0]);
		break;
	case SYS_WAIT:
		get_args(esp, 1, &args);
		f->eax = process_wait((int)args[0]);
		break;
	case SYS_CREATE:
	case SYS_REMOVE:
	case SYS_OPEN:
	case SYS_FILESIZE:
	case SYS_READ:
		get_args(esp, 3, &args);
		if ((int)args[0] == 0) {
			for (i = 0; i < (unsigned)args[2]; ++i) {
				((uint8_t*)args[1])[i] = input_getc();
			}
		}
		f->eax = (unsigned)args[2];
		break;
	case SYS_WRITE:
		get_args(esp, 3, &args);
		if ((int)args[0] == 1) {
			//printf("\nargs : %s\n", (const char*)args[1]);
			putbuf((const char*)args[1], (unsigned)args[2]);
		}
		f->eax = (unsigned)args[2];
		break;
	case SYS_SEEK:
	case SYS_TELL:
	case SYS_CLOSE:
		break;
	case SYS_PIBONACCI:
		get_args(esp, 1, &args);
		f->eax = pibonacci((int)args[0]);
		break;
	case SYS_SUM_OF_FOUR:
		printf("sum of four\n");
		get_args(esp, 4, &args);
		printf("get_args\n");
		f->eax = (int)args[0] + (int)args[1] + (int)args[2] + (int)args[3];
		break;
	default:
		break;
	}
  thread_exit ();
}
