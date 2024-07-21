#include "Private.h"

DescriptorManager::DescriptorManager(RenderDevice* device, u32 dynamicHeapCount)
{
    _Device = device;
    _CPUSRVHeap = _Device->CreateDescriptorHeap(DescriptorHeap::Config{
        .Type = DescriptorType::CBV_SRV_UAV,
        .Number = cCPUSRVDescriptorNum,
        .GPUVisible = false
        });

    _CPUSamplerHeap = _Device->CreateDescriptorHeap(DescriptorHeap::Config{
        .Type = DescriptorType::Sampler,
        .Number = cCPUSamplerDescriptorNum,
        .GPUVisible = false
        });

    _GPURTVHeap = _Device->CreateDescriptorHeap(DescriptorHeap::Config{
        .Type = DescriptorType::RTV,
        .Number = cGPURTVDSVDescriptorNum,
        .GPUVisible = false
        });

    _GPUDSVHeap = _Device->CreateDescriptorHeap(DescriptorHeap::Config{
        .Type = DescriptorType::DSV,
        .Number = cGPURTVDSVDescriptorNum,
        .GPUVisible = false
      });

    for (u32 i = 0; i < dynamicHeapCount; i++)
    {
        _SRVDynamicHeap.push_back(_Device->CreateDynamicDescriptorHeap(cDynamicSRVDescriptorNum, DescriptorType::CBV_SRV_UAV));
        _SamplerDynamicHeap.push_back(_Device->CreateDynamicDescriptorHeap(cDynamicSamplerDescriptorNum, DescriptorType::Sampler));
    }
}

DescriptorManager::~DescriptorManager()
{
    if (_CPUSRVHeap)
        delete _CPUSRVHeap;

    if (_CPUSamplerHeap)
        delete _CPUSamplerHeap;

    if (_GPURTVHeap)
        delete _GPURTVHeap;

    if (_GPUDSVHeap)
        delete _GPUDSVHeap;

    for (auto s : _SRVDynamicHeap)
    {
        delete s;
    }

    for( auto s : _SamplerDynamicHeap)
    {
        delete s;
    }
}

DescriptorHeap::DescriptorHeap()
{
    _DescriptorBitSet.reset();
    _DescriptorAllocatedNum = 0;
}

DescriptorAllocation DescriptorHeap::AllocateBlock(u32 count)
{
    check(_Config.GPUVisible);
    for (auto iter = _FreeBlocks.begin(); iter != _FreeBlocks.end(); iter++)
    {
        if (iter->Count >= count)
        {
            DescriptorAllocation alloc = {
                .Offset = (i32)iter->Offset,
                .Heap = this
            };

            Block n = {
                .Offset = iter->Offset,
                .Count = count
            };

            Block remain = {
                .Offset = iter->Offset + count,
                .Count = iter->Count - count
            };

            _AllocBlocks.push_back(n);
            _FreeBlocks.insert(iter, remain);
            _FreeBlocks.erase(iter);
            return alloc;
        }
    }

    check(0);
    return DescriptorAllocation{};

}

DescriptorAllocation DescriptorHeap::Allocate()
{
    check(!_Config.GPUVisible); // must be cpu heap or rvt, dsv.
    for (u32 i = 0; i < _Config.Number; i++)
    {
        if (!_DescriptorBitSet.test(i))
        {
            _DescriptorBitSet.set(i);
            _DescriptorAllocatedNum++;
            return DescriptorAllocation{
                .Offset = (i32)i,
                .Heap = this
            };
        }
    }
    
    check(0);
    return DescriptorAllocation{
        .Offset = -1,
        .Heap = nullptr
    };
}

void DescriptorHeap::Free(const DescriptorAllocation& pos)
{
    check(!_Config.GPUVisible); // must be cpu heap or rvt, dsv.
    check(_DescriptorBitSet.test(pos.Offset));
    _DescriptorBitSet.set(pos.Offset, false);
    _DescriptorAllocatedNum--;    
}

void DescriptorHeap::FreeBlock(const DescriptorAllocation& pos)
{
    check(_Config.GPUVisible);
    for (auto iter = _AllocBlocks.begin(); iter != _AllocBlocks.end(); iter++)
    {
        if (iter->Offset == pos.Offset)
        {
            Block b = {
                .Offset = iter->Offset,
                .Count = iter->Count
            };

            _AllocBlocks.erase(iter);
            // merge to free blocks.
            auto iter_next = _FreeBlocks.begin();
            while (iter_next != _FreeBlocks.end())
            {
                if (iter_next->Offset > b.Offset + b.Count) // insert to.
                {
                    _FreeBlocks.insert(iter_next, b);
                    break;
                }
                else if (iter_next->Offset == b.Offset + b.Count) // merge.
                {
                    iter_next->Offset = b.Offset;
                    iter_next->Count += b.Count;
                    break;
                }

                auto iter_this = iter_next++;
                if (iter_next == _FreeBlocks.end())
                    break;

                if (iter_this->Offset + iter_this->Count < b.Offset && b.Offset + b.Count < iter_next->Offset) // middle
                {
                    _FreeBlocks.insert(iter_next, b);
                    break;
                }
                else if (iter_this->Offset + iter_this->Count == b.Offset && b.Offset + b.Count < iter_next->Offset) // merge to front.
                {
                    iter_this->Count += b.Count;
                    break;
                }
                else if (iter_this->Offset + iter_this->Count < b.Offset && b.Offset + b.Count == iter_next->Offset) // merge to next.
                {
                    iter_next->Offset = b.Offset;
                    iter_next->Count += b.Count;
                    break;
                }
                else if (iter_this->Offset + iter_this->Count == b.Offset && b.Offset + b.Count == iter_next->Offset) // merge it all
                {
                    iter_this->Count += b.Count;
                    iter_this->Count += iter_next->Count;
                    _FreeBlocks.erase(iter_next);
                    break;
                }
            }

            return;
        }
    }

    check(0);
}


u32 DynamicDescriptorHeap::Allocate()
{
    check(_Current < _Size - 1);
    
    u32 alloc = _Current++;
    return alloc;
}

void DynamicDescriptorHeap::Reset()
{
    _Current = 0;
}