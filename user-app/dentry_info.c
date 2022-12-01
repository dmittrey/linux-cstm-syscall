#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define DNAME_INLINE_LEN 32

#define __NR_cstm_dentry_info 438

static struct time {
	signed long long tv_sec; /* seconds */
	long tv_nsec; /* nanoseconds */
};

static struct cstm_dentry_info {
	int is_cant_mount;
	bool is_mount_point;
	unsigned int inode_uid;
	unsigned int inode_gid;
	unsigned int dev_number;
	struct time access_time;
	struct time modify_time;
	struct time creation_time;
	unsigned char name[DNAME_INLINE_LEN];
};

static long __cstm_dentry_info(char *dntry_path, size_t dntry_path_len,
			       struct cstm_dentry_info *dntry_info)
{
	return syscall(__NR_cstm_dentry_info, dntry_path, dntry_path_len,
		       dntry_info);
}

static void print_dentry(struct cstm_dentry_info *dentry_info)
{
	printf("Dentry name: %s\n", dentry_info->name);

	printf("\tMount: ");
	if (dentry_info->is_cant_mount)
		printf("Unavailable\n");
	else
		printf("Available\n");

	printf("\tMount point: ");
	if (dentry_info->is_mount_point)
		printf("True\n");
	else
		printf("False\n");

	printf("\tInode UID: %u\n", dentry_info->inode_uid);
	printf("\tInode GID: %u\n", dentry_info->inode_gid);
	printf("\tDevice number: %u\n", dentry_info->dev_number);
	printf("\tAccess time: %llds\n", dentry_info->access_time.tv_sec);
	printf("\tModify time: %llds\n", dentry_info->modify_time.tv_sec);
	printf("\tCreation time: %llds\n", dentry_info->creation_time.tv_sec);
}

int main(int argc, char **argv)
{
	struct cstm_dentry_info *dntry_info =
		malloc(sizeof(struct cstm_dentry_info));

	char *path = malloc(sizeof(char) * 256);

	printf("Enter dir name: ");
	scanf("%s", path);

	long ret = __cstm_dentry_info(path, 256, dntry_info);

	if (ret < 0) {
		printf("System call __cstm_dentry_info error: %s\n",
		       strerror(-ret));
		free(dntry_info);
		free(path);
		return -1;
	}

	print_dentry(dntry_info);
	free(dntry_info);
	free(path);
	return 0;
}