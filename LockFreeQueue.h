#ifndef _LOCK_FREE_QUEUE_H_
#define _LOCK_FREE_QUEUE_H_

/*
kfifo ring buffer
内存屏障
smp_mb
smp_rmb
smp_wmb
*/

struct kfifo
{
	unsigned char *buffer; /* the buffer holding the data */
	unsigned int size;	   /* the size of the allocated buffer */
	unsigned int in;	   /* data is added at offset (in % size) */
	unsigned int out;	   /* data is extracted from off. (out % size) */
	spinlock_t *lock;	   /* protects concurrent modifications */
};

struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
{
	unsigned char *buffer;
	struct kfifo *ret;

	/*
     * round up to the next power of 2, since our 'let the indices
     * wrap' tachnique works only in this case.
     */
	if (size & (size - 1))
	{
		BUG_ON(size > 0x80000000);
		size = roundup_pow_of_two(size);
	}

	buffer = kmalloc(size, gfp_mask);
	if (!buffer)
		return ERR_PTR(-ENOMEM);

	ret = kfifo_init(buffer, size, gfp_mask, lock);

	if (IS_ERR(ret))
		kfree(buffer);

	return ret;
}

unsigned int __kfifo_put(struct kfifo *fifo,
						 unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	len = min(len, fifo->size - fifo->in + fifo->out);

	/*
     * Ensure that we sample the fifo->out index -before- we
     * start putting bytes into the kfifo.
     */

	smp_mb();

	/* first put the data starting from fifo->in to buffer end */
	l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
	memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, buffer + l, len - l);

	/*
     * Ensure that we add the bytes to the kfifo -before-
     * we update the fifo->in index.
     */

	smp_wmb();

	fifo->in += len;

	return len;
}

unsigned int __kfifo_get(struct kfifo *fifo,
						 unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	len = min(len, fifo->in - fifo->out);

	/*
     * Ensure that we sample the fifo->in index -before- we
     * start removing bytes from the kfifo.
     */

	smp_rmb();

	/* first get the data from fifo->out until the end of the buffer */
	l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
	memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(buffer + l, fifo->buffer, len - l);

	/*
     * Ensure that we remove the bytes from the kfifo -before-
     * we update the fifo->out index.
     */

	smp_mb();

	fifo->out += len;

	return len;
}

#endif