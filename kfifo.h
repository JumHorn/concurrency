#ifndef _KFIFO_H_
#define _KFIFO_H_

/*
kfifo ring buffer
内存屏障
smp_mb
smp_rmb
smp_wmb

Memory Barriers

The Linux kernel has a variety of memory barriers:

barrier(), which can be modeled as an atomic_signal_fence(memory_order_acq_rel) or an atomic_signal_fence(memory_order_seq_cst).
smp_mb(), which does not have a direct C11 or C++11 counterpart. On an ARM, PowerPC, or x86 system, it can be modeled as a full memory-barrier instruction (dmb, sync, and mfence, respectively). On an Itanium system, it can be modeled as an mf instruction, but this relies on gcc emitting an ld.acq for an ACCESS_ONCE() load and an st.rel for an ACCESS_ONCE() store. (Peter Zijlstra of Intel notes that although IA64's reference manual claims instructions with acquire and release semantics, the actual hardware implements only full barriers. See commit e4f9bfb3feae (“ia64: Fix up smp_mb__{before,after}_clear_bit()”) for Linux-kernel changes based on this situation. Tony Luck and Fenghua Yu are the IA64 maintainers for the Linux kernel.)
smp_rmb(), which can be modeled (overly conservatively) as an atomic_thread_fence(memory_order_acq_rel). One difference is that smp_rmb() need not order prior loads against later stores, or prior stores against later stores. Another difference is that smp_rmb() need not provide any sort of transitivity, having (lack of) transitivity properties similar to ARM's or PowerPC's address/control/data dependencies.
smp_wmb(), which can be modeled (again overly conservatively) as an atomic_thread_fence(memory_order_acq_rel). One difference is that smp_wmb() need not order prior loads against later stores, nor prior loads against later loads. Similar to smp_rmb(), smp_wmb() need not provide any sort of transitivity.
smp_read_barrier_depends(), which is a no-op on all architectures other than Alpha. On Alpha, smp_read_barrier_depends() may be modeled as a atomic_thread_fence(memory_order_acq_rel) or as a atomic_thread_fence(memory_order_seq_cst).
smp_mb__before_atomic(), which provides a full memory barrier before the immediately following non-value-returning atomic operation.
smp_mb__after_atomic(), which provides a full memory barrier after the immediately preceding non-value-returning atomic operation. Both smp_mb__before_atomic() and smp_mb__after_atomic() are described in more detail in the later section on atomic operations.
smp_mb__after_unlock_lock(), which provides a full memory barrier after the immediately preceding lock operation, but only when paired with a preceding unlock operation by this same thread or a preceding unlock operation on the same lock variable. The use of smp_mb__after_unlock_lock() is described in more detail in the section on locking.
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