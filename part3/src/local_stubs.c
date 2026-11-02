#include <linux/module.h>

int (*STUB_start_elevator)(void);
int (*STUB_issue_request)(int, int, int);
int (*STUB_stop_elevator)(void);

EXPORT_SYMBOL(STUB_start_elevator);
EXPORT_SYMBOL(STUB_issue_request);
EXPORT_SYMBOL(STUB_stop_elevator);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("local syscall pointer exports for module builds");
