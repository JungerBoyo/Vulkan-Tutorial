#ifndef VULKANTUT2_CONSTEXPRMAP_H
#define VULKANTUT2_CONSTEXPRMAP_H

#include <array>

template<typename KeyType, typename T, size_t size>
struct ConstexprMap
{
    constexpr ConstexprMap(const std::array<std::pair<KeyType, T>, size>& values)
        : _values(values) {}

    constexpr const auto& operator[](const KeyType& key) const
    {
        for(const auto& value : _values)
            if(value.first == key)
                return value.second;

        return _values[0].second;
    }

    private:
        std::array<std::pair<KeyType, T>, size> _values;
};

#endif