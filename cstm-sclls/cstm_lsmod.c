#include <linux/module.h>

#include <linux/syscalls.h>

struct cstm_lsmod_module_info {
	char name[MODULE_NAME_LEN];
	enum module_state state;
	unsigned int size;
	int references_count;
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

	if (copy_to_user(modules_count, &count, sizeof(modules_count)))
		return -EFAULT;

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

		pr_alert("Module: %s", cur_module->name);

		// Check for unformed modules
		if (cur_module != NULL) {
			if (copy_to_user(cur_modules_info->name,
					 cur_module->name,
					 sizeof(cur_modules_info->name)))
				return -EFAULT;

			if (copy_to_user(&cur_modules_info->state,
					 &cur_module->state,
					 sizeof(cur_modules_info->state)))
				return -EFAULT;

			if (copy_to_user(&cur_modules_info->size,
					 &cur_module->core_layout.size,
					 sizeof(cur_modules_info->size)))
				return -EFAULT;

			cur_module_refcnt = module_refcount(cur_module);

			if (copy_to_user(
				    &cur_modules_info->references_count,
				    &cur_module_refcnt,
				    sizeof(cur_modules_info->references_count)))
				return -EFAULT;

			cur_modules_info += 1;
		}
	}

	return 0;
}