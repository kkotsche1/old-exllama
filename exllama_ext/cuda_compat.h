#ifndef _cuda_compat_h
#define _cuda_compat_h

// atomicAdd for half types, to support CC < 7.x

__device__ __forceinline__ void atomicAdd_half(half* address, half val)
{
    unsigned int * address_as_ui = (unsigned int *) ((char *)address - ((size_t)address & 2));
    unsigned int old = *address_as_ui;
    unsigned int assumed;

    do
    {
        assumed = old;
        __half_raw hsum;
        hsum.x = (size_t)address & 2 ? (old >> 16) : (old & 0xffff);
        half tmpres = __hadd(hsum, val);
        hsum = __half_raw(tmpres);
        old = (size_t)address & 2 ? (old & 0xffff) | (hsum.x << 16) : (old & 0xffff0000) | hsum.x;
        old = atomicCAS(address_as_ui, assumed, old);
    }
    while (assumed != old);
}

#ifdef __CUDA_ARCH__
#if __CUDA_ARCH__ < 700

__device__ __forceinline__ void atomicAdd(half* address, half val) { atomicAdd_half(address, val); }

#endif
#endif

#endif