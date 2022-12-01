#include <linux/module.h>

#include <linux/syscalls.h>

#define COPY_TO_USER_S(dest, src, size)                                        \
	if (copy_to_user(dest, src, size))                                     \
		return -EFAULT;

#define COPY_TO_USER(dest, src) COPY_TO_USER_S(dest, src, sizeof(dest))

struct cstm_lsmod_module_info {
	enum module_state state;
	unsigned int size;
	int references_count;
	char name[MODULE_NAME_LEN];
};

SYSCALL_DEFINE1(cstm_lsmod_count, long *, modules_count)
{
	struct kobject *k;
	long count = 0;

	list_for_each_entry (k, &module_kset->list, entry) {
		struct module_kobject *cur_module_kobject =
			container_of(k, struct module_kobject, kobj);

		// Check for unformed modules
		if (cur_module_kobject->mod != NULL) {
			count++;
		}
	}

	COPY_TO_USER(modules_count, &count);

	return 0;
}

SYSCALL_DEFINE1(cstm_lsmod, struct cstm_lsmod_module_info *, modules_info)
{
	struct kobject *k;

	struct cstm_lsmod_module_info *cur_modules_info = modules_info;

	struct module *cur_module;
	int cur_module_refcnt;

	list_for_each_entry (k, &module_kset->list, entry) {
		cur_module = container_of(k, struct module_kobject, kobj)->mod;

		// Check for unformed modules
		if (cur_module != NULL) {
			cur_module_refcnt = module_refcount(cur_module);

			COPY_TO_USER(cur_modules_info->name, cur_module->name);

			COPY_TO_USER_S(&cur_modules_info->state,
				       &cur_module->state,
				       sizeof(cur_modules_info->state));

			COPY_TO_USER_S(&cur_modules_info->size,
				       &cur_module->core_layout.size,
				       sizeof(cur_modules_info->size));

			COPY_TO_USER_S(
				&cur_modules_info->references_count,
				&cur_module_refcnt,
				sizeof(cur_modules_info->references_count));

			cur_modules_info++;
		}
	}

	return 0;
}