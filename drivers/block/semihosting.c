// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Vincent Stehlé <vincent.stehle@arm.com>
 * Based on sandbox.c and ide.c
 * Pieces copied from arch/arm/lib/semihosting.c :(
 */

#include <common.h>
#include <blk.h>
#include <dm.h>
#include <ata.h>
#include <os.h>

#define SYSOPEN		0x01
#define SYSCLOSE	0x02
#define SYSWRITE	0x05
#define SYSREAD		0x06
#define SYSSEEK		0x0A
#define SYSFLEN		0x0C

#define MODE_READ	0x0
#define MODE_READBIN	0x1
#define MODE_RWBIN	0x3

#define SHB_FILENAME "shb.img"

struct shb_priv {
	long fd;
	long len;
};

struct smh_open_s {
	const char *fname;
	unsigned long mode;
	size_t len;
};

struct smh_seek_s {
	long fd;
	unsigned long pos;
};

struct smh_read_s {
	long fd;
	void *memp;
	size_t len;
};

struct smh_write_s {
	long fd;
	const void *memp;
	size_t len;
};

DECLARE_GLOBAL_DATA_PTR;

/*
 * Call the handler
 */
static noinline long smh_trap(unsigned int sysnum, void *addr)
{
#ifndef CONFIG_SANDBOX
	register long result asm("r0");
#if defined(CONFIG_ARM64)
	asm volatile ("hlt #0xf000" : "=r" (result) : "0"(sysnum), "r"(addr));
#elif defined(CONFIG_CPU_V7M)
	asm volatile ("bkpt #0xAB" : "=r" (result) : "0"(sysnum), "r"(addr));
#else
	/* Note - untested placeholder */
	asm volatile ("svc #0x123456" : "=r" (result) : "0"(sysnum), "r"(addr));
#endif
#else // CONFIG_SANDBOX
	long result = -1;

	switch (sysnum) {
	case SYSOPEN: {
		const struct smh_open_s *s = addr;
		int flags;

		switch (s->mode) {
		case MODE_RWBIN:
			flags = OS_O_RDWR;
			break;
		default:
			assert(!"bad mode");
		}

		debug("os_open %s\n", s->fname);
		result = os_open(s->fname, flags);
		break;
	}

	case SYSFLEN: {
		int fd = *(long *)addr;

		debug("os_fdsize %d\n", fd);
		result = os_fdsize(fd);
		break;
	}

	case SYSSEEK: {
		const struct smh_seek_s *s = addr;
		off_t ret;

		debug("os_lseek %ld %ld\n", s->fd, s->pos);
		ret = os_lseek(s->fd, s->pos, OS_SEEK_SET);
		result = (ret == s->pos) ? 0 : -1;
		break;
	}

	case SYSREAD: {
		const struct smh_read_s *s = addr;
		ssize_t ret;

		debug("os_read %ld %p %ld\n", s->fd, s->memp, s->len);
		ret = os_read(s->fd, s->memp, s->len);
		result = (ret == s->len) ? 0 : -1;
		break;
	}

	case SYSWRITE: {
		const struct smh_write_s *s = addr;
		ssize_t ret;

		debug("os_write %ld %p %ld\n", s->fd, s->memp, s->len);
		ret = os_write(s->fd, s->memp, s->len);
		result = (ret == s->len) ? 0 : -1;
		break;
	}


	default:
		assert(!"bad sysnum");
	}
#endif // CONFIG_SANDBOX
	return result;
}

/*
 * Open a file on the host. Mode is "r" or "rb" currently. Returns a file
 * descriptor or -1 on error.
 */
static long smh_open(const char *fname, char *modestr)
{
	long fd;
	unsigned long mode;
	struct smh_open_s open;

	debug("%s: file \'%s\', mode \'%s\'\n", __func__, fname, modestr);

	/* Check the file mode */
	if (!(strcmp(modestr, "r"))) {
		mode = MODE_READ;
	} else if (!(strcmp(modestr, "rb"))) {
		mode = MODE_READBIN;
	} else if (!(strcmp(modestr, "r+b"))) {
		mode = MODE_RWBIN;
	} else {
		printf("%s: ERROR mode \'%s\' not supported\n", __func__,
		       modestr);
		return -1;
	}

	open.fname = fname;
	open.len = strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = smh_trap(SYSOPEN, &open);
	if (fd == -1)
		printf("%s: ERROR fd %ld for file \'%s\'\n", __func__, fd,
		       fname);

	return fd;
}

/*
 * Seek. Returns 0 on success, else failure
 */
static long smh_seek(long fd, off_t pos)
{
	long ret;
	struct smh_seek_s seek;

	debug("%s: fd %ld, pos %lu\n", __func__, fd, pos);

	seek.fd = fd;
	seek.pos = pos;

	ret = smh_trap(SYSSEEK, &seek);
	if (ret < 0) {
		printf("%s: ERROR ret %ld, fd %ld, pos %lu\n",
		       __func__, ret, fd, pos);
		return -1;
	}

	return 0;
}

/*
 * Read 'len' bytes of file into 'memp'. Returns 0 on success, else failure
 */
static long smh_read(long fd, void *memp, size_t len)
{
	long ret;
	struct smh_read_s read;

	debug("%s: fd %ld, memp %p, len %zu\n", __func__, fd, memp, len);

	read.fd = fd;
	read.memp = memp;
	read.len = len;

	ret = smh_trap(SYSREAD, &read);
	if (ret < 0) {
		/*
		 * The ARM handler allows for returning partial lengths,
		 * but in practice this never happens so rather than create
		 * hard to maintain partial read loops and such, just fail
		 * with an error message.
		 */
		printf("%s: ERROR ret %ld, fd %ld, len %zu memp %p\n",
		       __func__, ret, fd, len, memp);
		return -1;
	}

	return 0;
}

/*
 * Write 'len' bytes to file from 'memp'. Returns 0 on success, else failure
 */
static long smh_write(long fd, const void *memp, size_t len)
{
	long ret;
	struct smh_write_s write;

	debug("%s: fd %ld, memp %p, len %zu\n", __func__, fd, memp, len);

	write.fd = fd;
	write.memp = memp;
	write.len = len;

	ret = smh_trap(SYSWRITE, &write);
	if (ret < 0) {
		printf("%s: ERROR ret %ld, fd %ld, len %zu memp %p\n",
		       __func__, ret, fd, len, memp);
		return -1;
	}

	return 0;
}

/*
 * Close the file using the file descriptor
 */
static long smh_close(long fd)
{
	long ret;

	debug("%s: fd %ld\n", __func__, fd);

	ret = smh_trap(SYSCLOSE, &fd);
	if (ret == -1)
		printf("%s: ERROR fd %ld\n", __func__, fd);

	return ret;
}

/*
 * Get the file length from the file descriptor
 */
static long smh_len_fd(long fd)
{
	long ret;

	debug("%s: fd %ld\n", __func__, fd);

	ret = smh_trap(SYSFLEN, &fd);
	if (ret == -1)
		printf("%s: ERROR ret %ld, fd %ld\n", __func__, ret, fd);

	return ret;
}

static unsigned long shb_read(struct udevice *dev, unsigned long start,
				lbaint_t blkcnt, void *buffer)
{
	struct shb_priv *priv = dev_get_priv(dev);
	struct blk_desc *desc = dev_get_uclass_platdata(dev);
	long ret;

	debug("%s: %ld %ld %p\n", __func__, start, blkcnt, buffer);

	if (start + blkcnt > desc->lba) {
		debug("%s: ERROR! bad block\n", __func__);
		return -1;
	}

	ret = smh_seek(priv->fd, start * ATA_BLOCKSIZE);
	if (ret) {
		debug("%s: ERROR! smh_seek %ld\n", __func__, ret);
		return -1;
	}

	ret = smh_read(priv->fd, buffer, blkcnt * ATA_BLOCKSIZE);
	if (ret) {
		debug("%s: ERROR! smh_read %ld\n", __func__, ret);
		return -1;
	}

	return blkcnt;
}

static unsigned long shb_write(struct udevice *dev, unsigned long start,
				lbaint_t blkcnt, const void *buffer)
{
	struct shb_priv *priv = dev_get_priv(dev);
	struct blk_desc *desc = dev_get_uclass_platdata(dev);
	long ret;

	debug("%s: %ld %ld %p\n", __func__, start, blkcnt, buffer);

	if (start + blkcnt > desc->lba) {
		debug("%s: ERROR! bad block\n", __func__);
		return -1;
	}

	ret = smh_seek(priv->fd, start * ATA_BLOCKSIZE);
	if (ret) {
		debug("%s: ERROR! smh_seek %ld\n", __func__, ret);
		return -1;
	}

	ret = smh_write(priv->fd, buffer, blkcnt * ATA_BLOCKSIZE);
	if (ret) {
		debug("%s: ERROR! smh_write %ld\n", __func__, ret);
		return -1;
	}

	return blkcnt;
}

static int shb_probe(struct udevice *udev)
{
	struct shb_priv *priv = dev_get_priv(udev);
	struct blk_desc *desc = dev_get_uclass_platdata(udev);

	debug("%s %s\n", __func__, udev->name);
	strncpy(desc->vendor, "Arm", BLK_VEN_SIZE);
	desc->vendor[BLK_VEN_SIZE] = '\0';
	strncpy(desc->product, "Semihosting", BLK_PRD_SIZE);
	desc->product[BLK_PRD_SIZE] = '\0';
	strncpy(desc->revision, "0.1", BLK_REV_SIZE);
	desc->revision[BLK_REV_SIZE] = '\0';
	desc->if_type = IF_TYPE_SHB;
	desc->removable = 0;

	priv->fd = smh_open(SHB_FILENAME, "r+b");
	if (priv->fd < 0) {
		printf("%s: ERROR! smh_open %ld\n", __func__, priv->fd);
		return priv->fd;
	}

	priv->len = smh_len_fd(priv->fd);
	if (priv->len < 0) {
		printf("%s: ERROR! smh_len_fd %ld\n", __func__, priv->len);
		smh_close(priv->fd);
		return priv->len;
	}

	desc->blksz = ATA_BLOCKSIZE;
	desc->log2blksz = LOG2(desc->blksz);
	desc->lba = priv->len / desc->blksz;
	desc->type = DEV_TYPE_HARDDISK;
	desc->lun = 0;
	desc->bdev = udev;
	debug("%s: %s %ld B, %ld LBA\n", __func__, SHB_FILENAME, priv->len,
			desc->lba);
	return 0;
}

static const struct blk_ops shb_ops = {
	.read	= shb_read,
	.write	= shb_write,
};

U_BOOT_DRIVER(shb_driver) = {
	.name			= "semihosting_blk",
	.id			= UCLASS_BLK,
	.ops			= &shb_ops,
	.probe			= shb_probe,
	.priv_auto_alloc_size	= sizeof(struct shb_priv),
};

U_BOOT_DEVICE(shb_device) = {
	.name = "semihosting_blk",
};
