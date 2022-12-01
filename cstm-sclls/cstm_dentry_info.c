#include <linux/fs_struct.h>
#include <linux/dcache.h>

#include <linux/syscalls.h>

#include <linux/namei.h>
#include <linux/fs.h>

#define COPY_TO_USER(dest_struct, src_struct)                                  \
	if (copy_to_user(&dest_struct, &src_struct, sizeof(dest_struct)))      \
		return -EFAULT;

#define COPY_FROM_USER(dest_struct, src_struct, size)                          \
	if (copy_from_user(dest_struct, src_struct, size))                     \
		return -EFAULT;

struct cstm_dentry_info {
	int is_cant_mount;
	bool is_mount_point;
	uid_t inode_uid;
	gid_t inode_gid;
	dev_t dev_number;
	struct timespec64 access_time;
	struct timespec64 modify_time;
	struct timespec64 creation_time;
	unsigned char name[DNAME_INLINE_LEN];
};

SYSCALL_DEFINE3(cstm_dentry_info, char *, dentry_path, size_t, dentry_path_len,
		struct cstm_dentry_info *, dentry_info)
{
	struct dentry *my_dentry;
	struct path path;

	int error;

	int is_cant_mount;
	bool is_mount_point;

	char *dentry_path_from_user =
		kmalloc(sizeof(char) * dentry_path_len, GFP_ATOMIC);

	COPY_FROM_USER(dentry_path_from_user, dentry_path,
		       sizeof(char) * dentry_path_len);

	error = kern_path(dentry_path_from_user, LOOKUP_FOLLOW, &path);
	if (error)
		return error;

	my_dentry = path.dentry;

	is_cant_mount = cant_mount(my_dentry);
	is_mount_point = d_mountpoint(my_dentry);

	COPY_TO_USER(dentry_info->is_cant_mount, is_cant_mount);

	COPY_TO_USER(dentry_info->is_mount_point, is_mount_point);

	COPY_TO_USER(dentry_info->inode_uid, my_dentry->d_inode->i_uid.val);

	COPY_TO_USER(dentry_info->inode_gid, my_dentry->d_inode->i_gid.val);

	COPY_TO_USER(dentry_info->dev_number, my_dentry->d_sb->s_dev);

	COPY_TO_USER(dentry_info->access_time, my_dentry->d_inode->i_atime);

	COPY_TO_USER(dentry_info->modify_time, my_dentry->d_inode->i_mtime);

	COPY_TO_USER(dentry_info->creation_time, my_dentry->d_inode->i_ctime);

	COPY_TO_USER(dentry_info->name, (*my_dentry->d_iname));

	kfree(dentry_path_from_user);

	return 0;
}
