#pragma once

#include "Core.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/UnorderedMap.hpp"

namespace ME::Core::Memory
{
    // Allocation without address, only offset
    struct OAllocation_S
    {
        SIZE_T Offset;
        SIZE_T Size;
    };

    typedef OAllocation_S* OAllocation;

    enum class BuddyAllocatorErrors : uint8
    {
        Success,
        // Specification errors
        IncorrectSpecification,
        // Allocation errors
        OutOfMemory, TooBigRequest, TooSmallRequest,
        // Free errors
        OutOfRange, InvalidArgument
    };

    struct BuddyAllocatorSpecification
    {
        SIZE_T MinBlockSize;
        SIZE_T TotalSize; // must be number powered by 2 (ex: 2, 4, 8, 16, 32, 64, 128 etc.) in bytes
    };

    class COREAPI BuddyAllocator
    {
    public:
        BuddyAllocator();
        BuddyAllocator(const BuddyAllocatorSpecification& specification);
        BuddyAllocator(const BuddyAllocator&) noexcept = delete;
        BuddyAllocator(BuddyAllocator&&) noexcept = delete;
        ~BuddyAllocator();
        uint8 CalculateMaxLevel(SIZE_T totalSize, SIZE_T minBlockSize) const;

    public:
        BuddyAllocatorErrors Init(const BuddyAllocatorSpecification& specification);
        void DumpFreeLists() const;

        SIZE_T GetSize() const { return m_TotalSize; }
        SIZE_T GetMinBlockSize() const { return m_MinBlockSize; }

    public:
        BuddyAllocatorErrors Allocate(OAllocation& allocation, SIZE_T size);
        BuddyAllocatorErrors Free(OAllocation& allocation);
        void Clear();
        bool Resize(SIZE_T size);

    private:
        // Helper functions
        uint8 GetLevel(SIZE_T size) const;
        inline SIZE_T GetBlockSize(uint8 level) const;
        SIZE_T GetBuddyOffset(SIZE_T offset, uint8 level);

        static bool ValidateSpecification(const BuddyAllocatorSpecification& specification);

    private:
        ME::Core::Array<ME::Core::Array<SIZE_T>> m_FreeLists;
        ME::Core::Array<OAllocation> m_Allocations;
        ME::Core::UnorderedMap<SIZE_T, uint8> m_Allocated;

    private:
        uint8 m_MaxLevel;
        SIZE_T m_MinBlockSize;
        SIZE_T m_TotalSize;
    };

}
