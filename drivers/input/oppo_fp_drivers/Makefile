obj-$(CONFIG_OPPO_FINGERPRINT) += oppo_fp_common/
obj-$(CONFIG_OPPO_FINGERPRINT) += fpc/
obj-$(CONFIG_OPPO_FINGERPRINT) += goodix_optical_fp/

ifeq ($(CONFIG_OPPO_FINGERPRINT_MTK),y)
obj-$(CONFIG_OPPO_FINGERPRINT) += silead_optical_fp/
endif

ifeq ($(CONFIG_OPPO_FINGERPRINT_QCOM),y)
obj-$(CONFIG_OPPO_FINGERPRINT) += silead/
endif

