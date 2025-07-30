#ifndef __SETTINGS_DATA_HPP__
#define __SETTINGS_DATA_HPP__

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "delegates.hpp"
#include "typedefs.hpp"
#include "util/observer_handler.hpp"

enum class SettingFormats { Simple, Percent };

class Settings {

public:
    Settings(SettingFormats format) : format(format) {}

    virtual ~Settings() {}

    virtual void resetToDefault() = 0;

    virtual SettingFormats getFormat() const {
        return format;
    }

    virtual std::string toString() const = 0;

protected:
    SettingFormats format;
};

template <class T>
class ObservableSetting : public Settings {
    
public:
    ObservableSetting(T value, SettingFormats format)
    : Settings(format), initial(value), value(value) {
    }
    
    ObserverHandler observe(consumer_t<T> callback, bool callOnStart = false) {
        const int_fast32_t id = ++m_nextid;
        
        m_observers.emplace(id, callback);
        
        if (callOnStart) {
            callback(value);
        }
        
        return ObserverHandler([this, id]() {
            m_observers.erase(id);
        });
    }
    
    const T& get() const {
        return value;
    }
    
    const T& getDefault() const {
        return initial;
    }
    
    T& operator*() {
        return value;
    }
    
    void notify(T value) {
        for (auto& entry : m_observers) {
            entry.second(value);
        }
    }
    
    void set(T value) {
        if (value == this->value) {
            return;
        }

        this->value = value;
        notify(value);
    }
    
    virtual void resetToDefault() override {
        set(initial);
    }

protected:
    T initial;
    T value;

private:
    int_fast32_t m_nextid = 1;
    std::unordered_map<int_fast32_t, consumer_t<T>> m_observers{};
};

class NumberSetting : public ObservableSetting<number_t> {
protected:
    number_t min;
    number_t max;
public:
    NumberSetting(
        number_t value,
        number_t min = std::numeric_limits<number_t>::min(),
        number_t max = std::numeric_limits<number_t>::max(),
        SettingFormats format = SettingFormats::Simple
    )
        : ObservableSetting(value, format), min(min), max(max) {
    }

    number_t& operator*() {
        return value;
    }

    number_t get() const {
        return value;
    }

    number_t getMin() const {
        return min;
    }

    number_t getMax() const {
        return max;
    }

    number_t getT() const {
        return (value - min) / (max - min);
    }

    virtual std::string toString() const override;

    static inline NumberSetting createPercent(number_t def) {
        return NumberSetting(def, 0.0, 1.0, SettingFormats::Percent);
    }
};

class IntegerSetting : public ObservableSetting<integer_t> {
protected:
    integer_t min;
    integer_t max;
public:
    IntegerSetting(
        integer_t value,
        integer_t min = std::numeric_limits<integer_t>::min(),
        integer_t max = std::numeric_limits<integer_t>::max(),
        SettingFormats format = SettingFormats::Simple
    )
        : ObservableSetting(value, format), min(min), max(max) {
    }

    integer_t getMin() const {
        return min;
    }

    integer_t getMax() const {
        return max;
    }

    integer_t getT() const {
        return (value - min) / (max - min);
    }

    virtual std::string toString() const override;
};

class FlagSetting : public ObservableSetting<bool> {
public:
    FlagSetting(bool value, SettingFormats format = SettingFormats::Simple)
        : ObservableSetting(value, format) {
    }

    void toggle() {
        set(!get());
    }

    virtual std::string toString() const override;
};

class StringSetting : public ObservableSetting<std::string> {
public:
    StringSetting(
        std::string value, SettingFormats format = SettingFormats::Simple
    )
        : ObservableSetting(value, format) {
    }

    virtual std::string toString() const override;
};

#endif
