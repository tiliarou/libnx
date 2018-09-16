#pragma once

typedef struct NvGpu NvGpu;

typedef struct {
    NvBuffer buffer;
    size_t   offset;
    size_t   num_cmds;
    size_t   max_cmds;
    NvGpu*   parent;
} NvCmdList;

Result nvCmdListCreate(NvCmdList* c, NvGpu* parent, size_t max_cmds);
void   nvCmdListClose(NvCmdList* c);

iova_t nvCmdListGetGpuAddr(NvCmdList* c);
u64    nvCmdListGetListSize(NvCmdList* c);

void   nvCmdListReset(NvCmdList* c);
u32*   nvCmdListInsert(NvCmdList* c, size_t num_cmds);
