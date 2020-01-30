#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <linux/xarray.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/pagemap.h>


MODULE_AUTHOR("Nathan Maurice & Smail Aider & Clement Therry");
MODULE_DESCRIPTION("Detect holes between dirty pages in the radix tree");
MODULE_LICENSE("GPL");


#define MAX_PATH_LENGTH 128

struct hole_stats {
	size_t dirty;
	size_t clean;
	size_t holes;
};

static char* pathname = NULL;
static int hole = 1;

struct xarray* get_xarray(void)
{
	unsigned int flags = O_RDONLY;
	struct path path;
	struct inode* inode;
	struct address_space* as;
	struct xarray* xa;
	int res;

	if (pathname == NULL)
		return NULL;
	res = kern_path(pathname, flags, &path);
	
	inode = d_inode(path.dentry);

	pr_info("inode %lu, %lu\n", inode->i_ino, inode->i_blocks);
	as = &inode->i_data;

        xa = &as->i_pages;

	pr_info("nrpages %lu\n", as->nrpages);
	
	return xa;
}

int count_holes(struct hole_stats* hstats)
{
	struct xarray* xa;
	void* entry;

	unsigned long index = 0, prev = 0, lastDirty = 0;
        bool hasMark;
	
	xa = get_xarray();
	xa_for_each(xa, index, entry) {
		hasMark = xa_get_mark(xa, index, PAGECACHE_TAG_DIRTY);
		if (hasMark) {
			hstats->dirty++;
			if (prev - lastDirty <= hole
			    && prev - lastDirty >= 1)
				hstats->holes++;
			lastDirty = index;
		} else {
			hstats->clean++;
		}
		prev = index;
	}
       	
	return 0;
}

ssize_t detection_show(
		       struct kobject *kobj,
		       struct kobj_attribute *attr,
		       char* buf)
{
	ssize_t len;
	struct hole_stats hstats;
	hstats.dirty = 0;
	hstats.clean = 0;
	hstats.holes = 0;
	
	count_holes(&hstats);
	len = snprintf(buf, PAGE_SIZE, "dirty %lu, clean %lu, holes %lu\n",
		       hstats.dirty, hstats.clean, hstats.holes);
	return len;
}

static struct kobj_attribute kattr = __ATTR_RO(detection);

static int detection_init(void)
{
	int status;
	status = sysfs_create_file(kernel_kobj, &kattr.attr);
	if (status < 0)
		pr_warn("Couldn't create sysfs file");
        
	return 0;
}

static void detection_exit(void)
{
        sysfs_remove_file(kernel_kobj, &kattr.attr);
}

module_init(detection_init);
module_exit(detection_exit);

module_param(pathname, charp, 0660);
MODULE_PARM_DESC(pathname, "Path of the file for which hole should be checked");

module_param(hole, int, 0660);
MODULE_PARM_DESC(hole, "Max size of hole that should be detected");
