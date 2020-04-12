#ifndef __QRTR_SVC_H_
#define __QRTR_SVC_H_
#include <linux/slab.h>
#include <linux/list.h>

struct qrtr_svc{
	struct list_head lst;
	u32 service;
	u32 node;
	u32 port;
	u64 key;
};

void qrtr_svc_init(void);
int qrtr_svc_new(u32 node, u32 port, u32 svc_id);
struct qrtr_svc* qrtr_get_svc_id(u32 node, u32 port);
void qrtr_svc_del(u32 node, u32 port);
struct qrtr_svc* qrtr_get_svc_default(void);
void qrtr_svc_deinit(void);
#endif

