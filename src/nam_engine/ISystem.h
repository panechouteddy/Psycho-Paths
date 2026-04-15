#pragma once
namespace nam
{
    class Ecs;

    class ISystem {

    private:
        bool m_enabled = true;

    public:
        virtual ~ISystem() = default;
        virtual void Update(Ecs& ecs) = 0;
        virtual void Init();
        virtual void Destroy();
        bool IsEnabled() const;
        void SetEnabled(bool enabled);
    };
}