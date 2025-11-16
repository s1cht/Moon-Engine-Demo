#include "BuddyAllocator.hpp"
#include "Core/Algorithm.hpp"

namespace ME::Core::Memory
{
    BuddyAllocator::BuddyAllocator()
        : m_MaxLevel(0), m_MinBlockSize(0), m_TotalSize(0)
    {
    }

    BuddyAllocator::BuddyAllocator(const BuddyAllocatorSpecification& specification)
    {
        Init(specification);
    }

    BuddyAllocator::~BuddyAllocator()
    {
        Clear();
    }

    uint8 BuddyAllocator::CalculateMaxLevel(SIZE_T totalSize, SIZE_T minBlockSize) const
    {
        ME_CORE_ASSERT(minBlockSize > 0, ME_LOGGER_TEXT("MinBlockSize must be > 0"));
        uint8 level = 0;
        SIZE_T block = minBlockSize;
        while (block < totalSize)
        {
            block <<= 1;
            ++level;
        }
        return level;
    }

    BuddyAllocatorErrors BuddyAllocator::Init(const BuddyAllocatorSpecification& specification)
    {
        if (!ValidateSpecification(specification))
        {
            ME_CORE_ASSERT(false, ME_LOGGER_TEXT("Buddy allocator can't be initialized with given specification!"));
            return BuddyAllocatorErrors::IncorrectSpecification;
        }

        m_TotalSize = specification.TotalSize;
        m_MinBlockSize = specification.MinBlockSize;
        m_MaxLevel = CalculateMaxLevel(m_TotalSize, m_MinBlockSize);
        m_FreeLists = Array<Array<SIZE_T>>(m_MaxLevel + 1);
        for (uint8 i = 0; i <= m_MaxLevel; ++i) m_FreeLists[i].Clear();
        m_FreeLists[m_MaxLevel].EmplaceBack(0);

        m_Allocated.Clear();
        m_Allocations.Clear();

        return BuddyAllocatorErrors::Success;
    }

    void BuddyAllocator::DumpFreeLists() const
    {
        for (uint8 i = 0; i <= m_MaxLevel; ++i)
            ME_INFO("Level {0}: {1} blocks\n", i, m_FreeLists[i].Size());
    }

    BuddyAllocatorErrors BuddyAllocator::Allocate(OAllocation& allocation, SIZE_T size)
    {
        if (m_TotalSize == 0 || m_MinBlockSize == 0)
        {
            ME_CORE_ASSERT(false, ME_LOGGER_TEXT("Buddy allocator isn't initialized!"));
            return BuddyAllocatorErrors::IncorrectSpecification;
        }

        if (size == 0) return BuddyAllocatorErrors::TooSmallRequest;
        if (size > m_TotalSize) return BuddyAllocatorErrors::TooBigRequest;

        allocation = nullptr;

        uint8 level = GetLevel(size);

        for (uint8 i = level; i <= m_MaxLevel; ++i)
        {
            if (m_FreeLists[i].Empty())
                continue;

            SIZE_T offset = m_FreeLists[i].Back();
            m_FreeLists[i].PopBack();

            uint8 cur = i;
            while (cur > level)
            {
                cur--;
                SIZE_T buddyOffset = offset + GetBlockSize(cur);
                m_FreeLists[cur].EmplaceBack(buddyOffset);
            }

            m_Allocated.Insert(offset, level);

            auto allocPtr = new OAllocation_S{ offset, size };
            m_Allocations.EmplaceBack(allocPtr);
            allocation = allocPtr;
            return BuddyAllocatorErrors::Success;
        }

        return BuddyAllocatorErrors::OutOfMemory;
    }

    BuddyAllocatorErrors BuddyAllocator::Free(OAllocation& allocation)
    {
        if (m_TotalSize == 0 || m_MinBlockSize == 0)
        {
            ME_CORE_ASSERT(false, ME_LOGGER_TEXT("Buddy allocator isn't initialized!"));
            return BuddyAllocatorErrors::IncorrectSpecification;
        }

        if (!allocation) return BuddyAllocatorErrors::InvalidArgument;
        if (allocation->Size > m_TotalSize) return BuddyAllocatorErrors::OutOfRange;
        if (allocation->Offset + allocation->Size > m_TotalSize) return BuddyAllocatorErrors::OutOfRange;

        auto it = m_Allocated.Find(allocation->Offset);
        if (it == m_Allocated.end()) return BuddyAllocatorErrors::OutOfRange;

        SIZE_T level = it->Value2;
        m_Allocated.Erase(it);

        SIZE_T curOffset = allocation->Offset;
        while (level < m_MaxLevel)
        {
            SIZE_T buddyOffset = GetBuddyOffset(curOffset, static_cast<uint8>(level));
            auto& list = m_FreeLists[level];
            auto buddy = ME::Core::Algorithm::Find(list.begin(), list.end(), buddyOffset);
            if (buddy == list.end())
                break;

            list.Erase(buddy);
            curOffset = ME::Core::Algorithm::Min(curOffset, buddyOffset);
            level++;
        }

        m_FreeLists[level].EmplaceBack(curOffset);

        for (SIZE_T i = 0; i < m_Allocations.Size(); ++i)
        {
            if (m_Allocations[i]->Offset == allocation->Offset)
            {
                delete m_Allocations[i];
                m_Allocations.EraseAt(i);
                break;
            }
        }

        allocation = nullptr;
        return BuddyAllocatorErrors::Success;
    }

    void BuddyAllocator::Clear()
    {
        for (auto& alloc : m_Allocations)
            delete alloc;
        m_Allocations.Clear();

        for (auto& list : m_FreeLists)
            list.Clear();

        if (m_MaxLevel > 0)
            m_FreeLists[m_MaxLevel].EmplaceBack(0);
        else if (m_FreeLists.Size() > 0)
            m_FreeLists[0].EmplaceBack(0);

        m_Allocated.Clear();
    }

    bool BuddyAllocator::Resize(SIZE_T size)
    {
        if (size <= m_TotalSize)
            return true;

        if (!IS_POWER_OF_2(size))
        {
            ME_CORE_ERROR("BuddyAllocator::Resize: Size must be a power of 2");
            return false;
        }

        SIZE_T oldTotalSize = m_TotalSize;
        uint8 oldMaxLevel = m_MaxLevel;

        m_TotalSize = size;
        m_MaxLevel = CalculateMaxLevel(m_TotalSize, m_MinBlockSize);

        if (m_FreeLists.Size() < m_MaxLevel + 1)
            if (!m_FreeLists.Reserve(m_MaxLevel + 1))
            {
                m_TotalSize = oldTotalSize;
                m_MaxLevel = oldMaxLevel;
                ME_CORE_ERROR("BuddyAllocator::Resize: Failed to resize free lists");
                return false;
            }
        m_FreeLists.Resize(m_MaxLevel + 1);

        return true;
    }
    uint8 BuddyAllocator::GetLevel(SIZE_T size) const
    {
        ME_CORE_ASSERT(m_MinBlockSize > 0, ME_LOGGER_TEXT("Buddy allocator isn't initialized!"));
        SIZE_T block = m_MinBlockSize;
        uint8 level = 0;
        while (block < size)
        {
            block <<= 1;
            ++level;
        }
        return level;
    }

    inline SIZE_T BuddyAllocator::GetBlockSize(uint8 level) const
    {
        ME_CORE_ASSERT(m_MinBlockSize > 0, ME_LOGGER_TEXT("Buddy allocator isn't initialized!"));
        return m_MinBlockSize << level;
    }

    SIZE_T BuddyAllocator::GetBuddyOffset(SIZE_T offset, uint8 level)
    {
        SIZE_T size = GetBlockSize(level);
        ME_CORE_ASSERT((offset % size) == 0, ME_LOGGER_TEXT("Offset misaligned for buddy computation!"));
        return offset ^ size;
    }

    bool BuddyAllocator::ValidateSpecification(const BuddyAllocatorSpecification& specification)
    {
        if (specification.TotalSize <= specification.MinBlockSize) return false;
        if (!IS_POWER_OF_2(specification.MinBlockSize)) return false;
        if (!IS_POWER_OF_2(specification.TotalSize)) return false;
        return true;
    }
}
