#pragma once
namespace nam
{
    class ISparseSet {
    public:
        virtual ~ISparseSet() = default;
        virtual bool Remove(const u32 id) = 0;
        virtual bool Has(const u32 id) const = 0;
        virtual Vector<u32> GetEntities() const = 0;
        virtual const size Size() const = 0;
    };
}