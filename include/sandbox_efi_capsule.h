// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2023, Linaro Limited
 */

#if !defined(_SANDBOX_EFI_CAPSULE_H_)
#define _SANDBOX_EFI_CAPSULE_H_

#define SANDBOX_UBOOT_IMAGE_GUID	"50980990-5af9-5522-86e2-8f05f4d7313c"
#define SANDBOX_UBOOT_ENV_IMAGE_GUID	"3554b655-b9f0-5240-ace2-6f34c2f7fcca"
#define SANDBOX_FIT_IMAGE_GUID		"8b38adc7-df0c-5769-8b89-c090ca3d07a7"
#define SANDBOX_INCORRECT_GUID		"058b7d83-50d5-4c47-a195-60d86ad341c4"

#define UBOOT_FIT_IMAGE			"u-boot_bin_env.itb"

#define CAPSULE_PRIV_KEY		"capsule_priv_key_good.key"
#define CAPSULE_PUB_KEY			"capsule_pub_key_good.crt"
#define CAPSULE_INVAL_KEY		"capsule_priv_key_bad.key"
#define CAPSULE_INVAL_PUB_KEY		"capsule_pub_key_bad.crt"

#endif /* _SANDBOX_EFI_CAPSULE_H_ */
