#include <malloc.h>
#include "types.h"
#include "result.h"
#include "arm/atomics.h"
#include "kernel/svc.h"
#include "services/nv.h"
#include "nvidia/ioctl.h"
#include "nvidia/buffer.h"
#include "nvidia/channel.h"
#include "nvidia/address_space.h"

Result nvAddressSpaceCreate(NvAddressSpace* a)
{
    Result rc;

    a->has_init = true;

    rc = nvOpen(&a->fd, "/dev/nvhost-as-gpu");

    if (R_FAILED(rc))
        a->fd = -1;

    if (R_SUCCEEDED(rc))
        rc = nvioctlNvhostAsGpu_InitializeEx(a->fd, 1, 0x10000);

    if (R_FAILED(rc))
        nvAddressSpaceClose(a);

    return rc;
}

void nvAddressSpaceClose(NvAddressSpace* a)
{
    if (!a->has_init)
        return;

    if (a->fd != -1)
        nvClose(a->fd);

    a->fd = -1;
}

Result nvAddressSpaceReserveAlign(
        NvAddressSpace* a, NvPageSize align, u32 pages, NvPageSize page_sz,
        iova_t* iova_out) {
    return nvioctlNvhostAsGpu_AllocSpace(a->fd, pages, page_sz, 0, align, iova_out);
}

Result nvAddressSpaceReserveAtFixedAddr(
        NvAddressSpace* a, iova_t addr, u32 pages, NvPageSize page_sz) {
    return nvioctlNvhostAsGpu_AllocSpace(a->fd, pages, page_sz, 1, addr, NULL);
}

Result nvAddressSpaceReserveFull(NvAddressSpace* a) {
    return nvAddressSpaceReserveAlign(a, NvPageSize_64K, 0x10000, NvPageSize_64K, NULL);
}

Result nvAddressSpaceMapBuffer(
        NvAddressSpace* a, u32 fd, u32 flags, NvKind kind,
        iova_t* iova_out) {
    return nvioctlNvhostAsGpu_MapBufferEx(
        a->fd, flags, kind, fd, 0x10000, 0, 0, 0, iova_out);
}

Result nvAddressSpaceUnmapBuffer(NvAddressSpace* a, iova_t iova) {
    return nvioctlNvhostAsGpu_UnmapBuffer(a->fd, iova);
}

Result nvAddressSpaceBindToChannel(NvAddressSpace* a, NvChannel* channel) {
    return nvioctlNvhostAsGpu_BindChannel(a->fd, channel->fd);
}
