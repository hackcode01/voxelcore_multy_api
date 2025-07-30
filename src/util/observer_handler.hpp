#ifndef __OBSERVER_HANDLER_HPP__
#define __OBSERVER_HANDLER_HPP__

#include <functional>

class ObserverHandler {

public:
    ObserverHandler() = default;

    ObserverHandler(std::function<void()> destructor)
        : m_destructor(std::move(destructor)) {
    }

    ObserverHandler(const ObserverHandler&) = delete;

    ObserverHandler(ObserverHandler&& handler) noexcept
        : m_destructor(std::move(handler.m_destructor)) {
        handler.m_destructor = nullptr;
    }

    ~ObserverHandler() {
        if (m_destructor) {
            m_destructor();
        }
    }

    bool operator==(std::nullptr_t) const {
        return m_destructor == nullptr;
    }

    ObserverHandler& operator=(const ObserverHandler& handler) = delete;

    ObserverHandler& operator=(ObserverHandler&& handler) noexcept {
        if (m_destructor) {
            m_destructor();
        }

        m_destructor = std::move(handler.m_destructor);
        handler.m_destructor = nullptr;

        return *this;
    }

private:
    std::function<void()> m_destructor{};
};

#endif
