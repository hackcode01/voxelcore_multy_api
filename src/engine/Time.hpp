#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <stdint.h>

class Time {
    uint64_t m_frame = 0;
    double m_lastTime = 0.0;
    double m_delta = 0.0;

public:
    Time() {}

    void update(double currentTime) {
        ++m_frame;
        m_delta = currentTime - m_lastTime;
        m_lastTime = currentTime;
    }

    void step(double delta) {
        ++m_frame;
        m_lastTime += delta;
        this->m_delta = delta;
    }

    void set(double currentTime) {
        m_lastTime = currentTime;
    }

    double getDelta() const {
        return m_delta;
    }

    double getTime() const {
        return m_lastTime;
    }
};

#endif
