#include <type_traits>
#include "SFML/Graphics/Text.hpp"

#pragma once

namespace utils
{
    template<
        typename T, //real type
        typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    >
    inline T map(T value, T istart, T istop, T ostart, T ostop )
    {
        return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
    }

    inline float GetSFMLTextMaxHeight(const sf::Text& l_text)
    {
        auto charSize = l_text.getCharacterSize();
        auto font = l_text.getFont();
        auto string = l_text.getString().toAnsiString();
        bool bold = (l_text.getStyle() & sf::Text::Bold);
        float max = 0.f;
        for (size_t i = 0; i < string.length(); ++i)
        {
            sf::Uint32 character = string[i];
            auto glyph = font->getGlyph(character, charSize, bold);
            auto height = glyph.bounds.height;
            if (height <= max)
            {
                continue;
            }
            max = height;
        }
        return max;
    }

    inline void CenterSFMLText(sf::Text& l_text)
    {
        sf::FloatRect rect = l_text.getLocalBounds();
        auto maxHeight = GetSFMLTextMaxHeight(l_text);
        l_text.setOrigin( rect.left + (rect.width * 0.5f),
                          rect.top + ((maxHeight >= rect.height ? maxHeight * 0.5f : rect.height * 0.5f)));
    }

    inline void CenterSFMLTextForX(sf::Text& l_text)
    {
        sf::FloatRect rect = l_text.getLocalBounds();
        l_text.setOrigin( rect.left + (rect.width * 0.5f), 0);

    }
}


