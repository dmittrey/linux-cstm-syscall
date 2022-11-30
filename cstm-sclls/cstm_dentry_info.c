#include <linux/fs_struct.h>
#include <linux/dcache.h>

#include <linux/syscalls.h>

#define DEV_CLASS_MODE ((umode_t)(S_IRUGO | S_IWUGO))

struct cstm_dentry_info {
	int is_cant_mount;
	bool is_mount_point;
	unsigned char name[DNAME_INLINE_LEN];
	umode_t inode_mode;
	uid_t inode_uid;
	gid_t inode_gid;
	dev_t dev_number;
	unsigned long reval_time;
};

SYSCALL_DEFINE1(cstm_dentry_info, struct cstm_dentry_info *, dentry_info)
{
	struct dentry *my_dentry = current->fs->pwd.dentry;

	int is_cant_mount = cant_mount(my_dentry);
	bool is_mount_point = d_mountpoint(my_dentry);

	if (copy_to_user(&dentry_info->is_cant_mount, &is_cant_mount,
			 sizeof(dentry_info->is_cant_mount)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->is_mount_point, &is_mount_point,
			 sizeof(dentry_info->is_mount_point)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->inode_mode, &my_dentry->d_inode->i_mode,
			 sizeof(dentry_info->inode_mode)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->inode_uid,
			 &my_dentry->d_inode->i_uid.val,
			 sizeof(dentry_info->inode_uid)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->inode_gid,
			 &my_dentry->d_inode->i_gid.val,
			 sizeof(dentry_info->inode_gid)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->dev_number, &my_dentry->d_sb->s_dev,
			 sizeof(dentry_info->dev_number)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->reval_time, &my_dentry->d_time,
			 sizeof(dentry_info->reval_time)))
		return -EFAULT;

	if (copy_to_user(&dentry_info->name, my_dentry->d_iname,
			 sizeof(dentry_info->name)))
		return -EFAULT;

	return 0;
}
