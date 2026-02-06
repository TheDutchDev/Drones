#ifndef IPROPERTY_H
#define IPROPERTY_H

#include <cstdint>
#include <algorithm>
#include <memory>
#include <cmath>
#include <cstring>

#include "IPropertyBase.h"
#include "EDataType.h"
#include "ILogger.h"

template<typename T>
class IProperty : public IPropertyBase {
    public:
        const T MinValue;
        const T MaxValue;

        IProperty(T value, const T& minValue, const T& maxValue, EPropertyType propertyType) :
             MinValue(minValue)
             , MaxValue(maxValue)
             , _value(value)
             , _defaultValue(value) {
            PropertyType = propertyType;
            OnModified = PropertyModifiedDelegate::Make();
        }

        T Value() {
            return _value;
        }

        void SetValueFromByteArray(const uint8_t* data, size_t size) override {
            if(size > sizeof(T)) {
                LOG_ERROR("Can't set value from byte array, size mismatch on property type:", (int)PropertyType, ", size received:", size);
                return;
            }
            std::memcpy(&_value, data, size);
            Trigger();
        }

        [[nodiscard]] uint8_t* ValuePtr() override {
            return static_cast<uint8_t*>(static_cast<void*>(&_value));
        }

        void Reset() {
            Modify(_defaultValue);
        }

        /**
         * Will update the value of this property as long as the previous value isn't the same as the new value
         * @param newValue The new value
         */
        void Modify(T newValue) {
            // Can't do nan/fabs checks for strings/chars.
            if constexpr(!std::is_same<T, const char*>::value) {
                if(std::isnan(newValue))
                    return;

                newValue = std::clamp(newValue, MinValue, MaxValue);

                // If double/floats
                if constexpr(std::is_same<T, float>::value || std::is_same<T, double>::value) {
                    if(fabsf(_value - newValue) < 0.000001f)
                        return;
                }
                // Anything else (int, uint, ...)
                else if(_value == newValue)
                    return;
            }

            _value = newValue;
            Trigger();
        }

        /**
         * Will update the value of this property even when the previous value is the same
         * @param newValue The new value
         */
        void ForceModify(T newValue) {
            // Can't do nan/fabs checks for strings/chars.
            if constexpr(!std::is_same<T, const char*>::value) {
                if(std::isnan(newValue))
                    return;

                newValue = std::clamp(newValue, MinValue, MaxValue);
            }

            _value = newValue;
            Trigger();
        }

        IProperty& operator+=(const T& other) {
            Modify(_value + other);
            return *this;
        }

        void Trigger() {
            OnModified->Emit(this);
        }

        T operator*(const IProperty<T>& other) const {
            return _value * other._value;
        }

        T operator*(const T& other) const {
            return _value * other;
        }

        T operator+(const IProperty<T>& other) const {
            return _value + other._value;
        }

        T operator+(const T& other) const {
            return _value + other;
        }

        T operator-(const IProperty<T>& other) const {
            return _value - other._value;
        }

        T operator-(const T& other) const {
            return _value - other;
        }

        bool operator==(const T& other) const {
            return _value == other;
        }

        bool operator!=(const T& other) const {
            return _value != other;
        }

        bool operator>=(const T& other) const {
            return _value >= other;
        }

        bool operator>=(const IProperty<T>& other) const {
            return _value >= other._value;
        }

        bool operator<=(const T& other) const {
            return _value <= other;
        }

        bool operator<=(const IProperty<T>& other) const {
            return _value <= other._value;
        }

        bool operator>(const T& other) const {
            return _value > other;
        }

        bool operator>(const IProperty<T>& other) const {
            return _value > other._value;
        }

        bool operator<(const T& other) const {
            return _value < other;
        }

        bool operator<(const IProperty<T>& other) const {
            return _value < other._value;
        }

        bool operator!() const {
            return !_value;
        }

        void ToByteArray(uint8_t* data) const {
            ConvertToByteArray<T>(_value, data);
        }

        [[nodiscard]] EDataType GetGenericDataType() const {
            switch(DataType) {
                case EPropertyDataType::Float:
                    return EDataType::Float;
                case EPropertyDataType::Int16:
                case EPropertyDataType::Int32:
                    return EDataType::Int;
                case EPropertyDataType::Uint8:
                case EPropertyDataType::Uint16:
                case EPropertyDataType::Uint32:
                    return EDataType::UInt;
                case EPropertyDataType::Bool:
                    return EDataType::Bool;
                case EPropertyDataType::Char:
                    return EDataType::String;
                case EPropertyDataType::Unknown:
                default:
                    return EDataType::Unknown;
            }
        }

        [[nodiscard]] uint8_t GetDataSize() const {
            switch(DataType) {
                case EPropertyDataType::Int16:
                case EPropertyDataType::Uint16:
                    return 2;
                case EPropertyDataType::Bool:
                case EPropertyDataType::Char:
                case EPropertyDataType::Uint8:
                    return 1;
                case EPropertyDataType::Uint64:
                case EPropertyDataType::Int32:
                case EPropertyDataType::Float:
                case EPropertyDataType::Uint32:
                case EPropertyDataType::Unknown:
                default:
                    return 4;
            }
        }

    private:
        T _value;
        T _defaultValue;
};

#endif //IPROPERTY_H
