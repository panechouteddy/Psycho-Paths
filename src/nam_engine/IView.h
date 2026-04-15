#pragma once
namespace nam
{
    class Ecs;

    class IView
    {
    public:
        virtual ~IView() = default;
        virtual void ActiveNeedRefresh() = 0;
        virtual void Refresh(Ecs& ecs) = 0;
        virtual bool HasComponent(const u32 componentId) const = 0;
    };
}