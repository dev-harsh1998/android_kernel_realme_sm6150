#yangmingjin@BSP.POWER.Basic 2019/07/10 modify for RM_TAG_POWER_DEBUG
##ifdef VENDOR_EDIT
obj-$(CONFIG_QRTR) := qrtr.o qrtr_svc_lookup.o
##else
#obj-$(CONFIG_QRTR) := qrtr.o
##endif

obj-$(CONFIG_QRTR_SMD) += qrtr-smd.o
qrtr-smd-y	:= smd.o

obj-$(CONFIG_QRTR_MHI) += qrtr-mhi.o
qrtr-mhi-y	:= mhi.o

obj-$(CONFIG_QRTR_FIFO) += qrtr-fifo.o
qrtr-fifo-y	:= fifo.o
