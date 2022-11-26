#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/uidgid.h>
#include <linux/types.h>

static struct cstm_lsmod_module_info {
	char *name;
	enum module_state state;
	unsigned int size;
	void *ptr_to_struct;
	int references_count;
	struct cstm_lsmod_module_info *next;
};

static struct cstm_lsmod_module_info *
get_module_info_copy(struct module *module)
{
	struct cstm_lsmod_module_info *module_info_copy =
		kmalloc(sizeof(struct cstm_lsmod_module_info), GFP_USER);

	module_info_copy->name =
		kmalloc(sizeof(char) * MODULE_NAME_LEN, GFP_USER);
	strncpy(module_info_copy->name, module->name, MODULE_NAME_LEN);

	module_info_copy->state = module->state;
	module_info_copy->size = module->core_layout.size;
	module_info_copy->ptr_to_struct = (void *)module;
	module_info_copy->references_count = module_refcount(module);

	return module_info_copy;
}

SYSCALL_DEFINE2(cstm_lsmod, size_t __user *, modules_count,
		struct cstm_lsmod_module_info __user *, modules_ptr)
{
	struct module *list_mod = NULL;
	size_t count = 0;

	struct cstm_lsmod_module_info *start_mod_info = NULL;
	struct cstm_lsmod_module_info *cur_mod_info = NULL;
	struct cstm_lsmod_module_info *prev_mod_info = NULL;

	/* traverse the module list and find corresponding module*/
	list_for_each_entry (list_mod, THIS_MODULE->list.prev, list) {
		cur_mod_info = get_module_info_copy(list_mod);
		count++;

		if (prev_mod_info != NULL) {
			prev_mod_info->next = cur_mod_info;
		} else {
			start_mod_info = cur_mod_info;
		}

		prev_mod_info = cur_mod_info;
	}

	pr_alert("%ld", count);

	__copy_to_user(modules_count, count, sizeof(size_t));
	__copy_to_user(modules_ptr, start_mod_info,
		       sizeof(struct cstm_lsmod_module_info));

	return 0;
}