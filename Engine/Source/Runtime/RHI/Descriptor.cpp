#include "Private.h"

DescriptorManager::DescriptorManager(RenderDevice* device)
{
    _Device = device;

    for (u32 i = 0; i < (u32)DescriptorType::Max; i++)
    {
        DescriptorType t = (DescriptorType)i;
        bool gpuVisible = true;
        if (t == DescriptorType::RVT || t == DescriptorType::DSV)
            gpuVisible = false;

        _Heaps[i] = _Device->CreateDescriptorHeap(t, gpuVisible);
    }
}

DescriptorManager::~DescriptorManager()
{
    for (u32 i = 0; i < (u32)DescriptorType::Max; i++)
    {
        delete _Heaps[i];
    }
}

DescriptorHeap::DescriptorHeap()
{
    _SingleDescriptorBitSet.reset();
    _FreeList.push_back(DescriptorAllocation{
        .Offset = cSingleDescriptorNum,
        .Count = cHeapDescriptorMax - cSingleDescriptorNum
        });

}

DescriptorAllocation DescriptorHeap::Allocate(u32 num)
{
    check(num > 0);
    if (num == 1)
    {
        for (u32 i = 0; i < _SingleDescriptorBitSet.size(); i++)
        {
            if (!_SingleDescriptorBitSet.test(i))
            {
                _SingleDescriptorBitSet.set(i);
                _SingleDescriptorAllocatedNum++;
                return DescriptorAllocation{
                    .Offset = i,
                    .Count = 1
                };
            }
        }
        check(0);
        return DescriptorAllocation{};
    }
    else
    {
        for (auto iter = _FreeList.begin(); iter != _FreeList.end(); iter++)
        {
            if (iter->Count >= num)
            {
                DescriptorAllocation alloc = {
                    .Offset = iter->Offset,
                    .Count = num
                };

                DescriptorAllocation remain = {
                    .Offset = iter->Offset + num,
                    .Count = iter->Count - num
                };

                _AllocList.push_back(alloc);
                _FreeList.insert(iter, remain);
                _FreeList.erase(iter);
                _MultipleDescriptorAllocatedNum += num;
                return alloc;
            }
        }

        check(0);
        return DescriptorAllocation{};
    }
}

void DescriptorHeap::Free(DescriptorAllocation& pos)
{
    if (pos.Count == 1)
    {
        check(_SingleDescriptorBitSet.test(pos.Offset));
        _SingleDescriptorBitSet.set(pos.Offset, false);
        _SingleDescriptorAllocatedNum--;
    }
    else
    {
        for (auto iter = _AllocList.begin(); iter != _AllocList.end(); iter++)
        {
            if (iter->Offset == pos.Offset && iter->Count == pos.Count)
            {
                _FreeList.push_back(pos);
                _AllocList.erase(iter);  // TODO, don't consider memory fragment.
                _MultipleDescriptorAllocatedNum -= pos.Count;
                return;
            }
        }

        // don't find.
        check(0);
    }
}

u32 DescriptorHeap::GetAvailableNum()
{
    return cHeapDescriptorMax - _SingleDescriptorAllocatedNum - _MultipleDescriptorAllocatedNum;
}