#include"qrtr_svc_lookup.h"
#include <linux/debugfs.h>
#include <linux/module.h>

#define HASH_SIZE  10
static struct list_head hash_array[HASH_SIZE];
static struct qrtr_svc qrtr_default = {.service = 0xffff, .key = 0xffff,};
static struct dentry *dent_parent = NULL;

static int get_hash_index(u64 key)
{
	return (key % HASH_SIZE);
}

static u64 get_key(u32 node, u32 port)
{
	return ((u64)node << 32 | port);
}

static struct qrtr_svc *qrtr_svc_search(u32 node, u32 port)
{
	struct list_head *pos;
	struct qrtr_svc *svc_id_loopup = NULL;
	u64 key = get_key(node, port);
	int index = get_hash_index(key);

	if(list_empty(&hash_array[index])){
		//pr_err("qrtr_svc_search: node:0x%x port:0x%x null\n", node, port);
		return NULL;
	}

	list_for_each(pos, &hash_array[index])
	{
		svc_id_loopup = list_entry(pos, struct qrtr_svc, lst);
		if(svc_id_loopup->key == key){
			//pr_err("qrtr_svc_search: node:0x%x port:0x%x svc_id:0x%x key:0x%lu\n", node, port, svc_id_loopup->service, svc_id_loopup->key);
			return svc_id_loopup;
		}
	}
	return NULL;
}

struct qrtr_svc *qrtr_get_svc_id(u32 node, u32 port)
{
	static struct qrtr_svc qrtr_svc_last = {.service = 0xffff, .key = 0xffff,};
	struct qrtr_svc *svc_id_loopup = NULL;

	if(qrtr_svc_last.key == get_key(node, port))
		return &qrtr_svc_last;

	svc_id_loopup = qrtr_svc_search(node, port);
	if(svc_id_loopup != NULL){
		qrtr_svc_last.key = svc_id_loopup->key;
		qrtr_svc_last.service = svc_id_loopup->service;
	}else{
		qrtr_svc_last.key = qrtr_default.key;
		qrtr_svc_last.service = qrtr_default.service;
	}
	return svc_id_loopup;
}

int qrtr_svc_new(u32 node, u32 port, u32 svc_id)
{
	struct qrtr_svc *svc_id_new = qrtr_svc_search(node, port);
	u64 key = -1;
	int index = 0;

	if(svc_id_new == NULL)
	{
		svc_id_new = kmalloc(sizeof(struct qrtr_svc), GFP_ATOMIC);
		if(svc_id_new != NULL){
			key = get_key(node, port);
			index = get_hash_index(key);
			svc_id_new->service = svc_id;
			svc_id_new->key = get_key(node, port);
			svc_id_new->node = node;
			svc_id_new->port = port;
			list_add_tail(&svc_id_new->lst,&hash_array[index]);
			//pr_err("qrtr_svc_new: node:0x%x port:0x%x svc_id:0x%x key:0x%lu\n", node, port, svc_id_new->service, svc_id_new->key);
		}
		return 1;
	}
	return 0;
}

void qrtr_svc_del(u32 node, u32 port)
{
	struct qrtr_svc *svc_id_loopup = qrtr_svc_search(node, port);

	if(svc_id_loopup != NULL){
		list_del(&svc_id_loopup->lst);
		//pr_err("qrtr_svc_del: node:0x%x port:0x%x svc_id:0x%x key:0x%lu\n", node, port, svc_id_loopup->service, svc_id_loopup->key);
		kfree(svc_id_loopup);
	}
	return;
}
struct qrtr_svc *qrtr_get_svc_default(void){
	return &qrtr_default;
}

static int qrtr_svc_debugfs_open(struct inode *inode, struct file *file)
{
	char *buf = kmalloc(sizeof(char) * PAGE_SIZE, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	file->private_data = buf;
	return 0;
}

static ssize_t  qrtr_svc_debugfs_read(struct file *file, char __user *user_buf,
				      size_t nbytes, loff_t *ppos)
{
	char *buf = file->private_data;
	struct list_head *pos;
	struct qrtr_svc *svc_id_loopup = NULL;
	int count = 0;

	count += snprintf(buf+count, PAGE_SIZE-count, "\n\tnode    port    svc_id    key\n");
	for(int index = 0; index < HASH_SIZE; index++){
		list_for_each(pos, &hash_array[index])
		{
			svc_id_loopup = list_entry(pos, struct qrtr_svc, lst);
			count += snprintf(buf+count, PAGE_SIZE-count, "\t0x%04x  0x%04x  0x%06x  0x%lu\n",
			svc_id_loopup->node, svc_id_loopup->port, svc_id_loopup->service, svc_id_loopup->key);
		}
	}
	return simple_read_from_buffer(user_buf, nbytes, ppos, buf, count);
}

static int  qrtr_svc_debugfs_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	file->private_data = NULL;

	return 0;
}

static const struct file_operations qrtr_svc_debugfs_fops = {
	.owner = THIS_MODULE,
	.open = qrtr_svc_debugfs_open,
	.llseek = no_llseek,
	.read = qrtr_svc_debugfs_read,
	.release = qrtr_svc_debugfs_release,
};

static void  qrtr_svc_debugfs_init(void){

	struct dentry *dent = NULL;
	dent_parent = debugfs_create_dir("qrtr-lookup", NULL);
	if (IS_ERR_OR_NULL(dent_parent)) {
		pr_err("Unable to create qrtr svc debugfs\n");
		return;
	}
	dent = debugfs_create_file("qrtr_svcs", 0644, dent_parent, NULL, &qrtr_svc_debugfs_fops);
	if (IS_ERR_OR_NULL(dent)) {
		pr_err("Unable to create debugfs qrtr_svcs file\n");
		debugfs_remove_recursive(dent_parent);
		return;
	}
}

void qrtr_svc_init(void)
{
	for(int i = 0; i < HASH_SIZE; i++)
		INIT_LIST_HEAD(&hash_array[i]);
	qrtr_svc_debugfs_init();
}

void qrtr_svc_deinit(void)
{
	struct list_head *pos;
	struct qrtr_svc *svc_id_loopup = NULL;
	for(int index = 0; index < HASH_SIZE; index++){
		list_for_each(pos, &hash_array[index]){
			svc_id_loopup = list_entry(pos, struct qrtr_svc, lst);
			if(svc_id_loopup != NULL){
				list_del(&svc_id_loopup->lst);
				kfree(svc_id_loopup);
			}
		}
	}
	if(dent_parent != NULL)
		debugfs_remove_recursive(dent_parent);
}

