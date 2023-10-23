/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_VARIANT_HPP
#define TGUI_VARIANT_HPP

#include <TGUI/Config.hpp>
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #if !TGUI_EXPERIMENTAL_USE_STD_MODULE
        #include <variant>
    #endif
#else
    #include <TGUI/Any.hpp>
    #include <TGUI/Exception.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
/*
#if TGUI_COMPILED_WITH_CPP_VER < 17
    namespace priv
    {
        template<typename... NoTypesLeft>
        struct IndexInEmulatedVariantHelper
        {
            static std::size_t findIndex(const Any&, std::size_t)
            {
                // We should never pass here, it means that the Any object didn't hold anything.
                throw Exception("tgui::Variant::index() called on uninitialized variant!");
            }

            static int getByIndex(const Any& any, std::size_t wantedIndex, std::size_t index)
            {
                throw Exception("tgui::Variant::get() called with too high index!");
            }
        };

        template<typename FirstType, typename... OtherTypes>
        struct IndexInEmulatedVariantHelper<FirstType, OtherTypes...>
        {
            static std::size_t findIndex(const Any& any, std::size_t index)
            {
                if (any.is<FirstType>())
                    return index;
                else
                    return IndexInEmulatedVariantHelper<OtherTypes...>::findIndex(any, index + 1);
            }

            static decltype(auto) getByIndex(const Any& any, std::size_t wantedIndex, std::size_t index)
            {
                if (index == wantedIndex)
                    return any.as<FirstType>();
                else
                    return IndexInEmulatedVariantHelper<OtherTypes...>::getByIndex(any, wantedIndex, index + 1);
            }
        };
    }
#endif
*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @internal
    /// @brief Wrapper around std::variant which will fall back to a custom Any class in c++14 mode
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    template <typename... Types>
#else
    template <typename FirstType, typename... OtherTypes>
#endif
    class Variant
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Variant() :
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            m_variant{}
#else
            m_any{FirstType{}}
#endif
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the variant with an initial value
        ///
        /// @param value  Value to store in the variant
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        Variant(T&& value) : // NOLINT(bugprone-forwarding-reference-overload)
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            m_variant{std::forward<T>(value)}
#else
            m_any{std::forward<T>(value)}
#endif
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieve the value in the variant
        ///
        /// @return Stored value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        TGUI_NODISCARD T& get()
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            return std::get<T>(m_variant);
#else
            return m_any.as<T>();
#endif
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieve the value in the variant
        ///
        /// @return Stored value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        TGUI_NODISCARD const T& get() const
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            return std::get<T>(m_variant);
#else
            return m_any.as<T>();
#endif
        }

/*
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieve the value in the variant
        ///
        /// @return Stored value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <std::size_t Index>
        auto& get()
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            return std::get<Index>(m_variant);
#else
            return priv::IndexInEmulatedVariantHelper<FirstType, OtherTypes...>::getByIndex(m_any, Index, 0);
#endif
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieve the value in the variant
        ///
        /// @return Stored value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <std::size_t Index>
        const auto& get() const
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            return std::get<Index>(m_variant);
#else
            return priv::IndexInEmulatedVariantHelper<FirstType, OtherTypes...>::getByIndex(m_any, Index, 0);
#endif
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the index
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t index() const
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            return m_variant.index();
#else
            return priv::IndexInEmulatedVariantHelper<FirstType, OtherTypes...>::findIndex(m_any, 0);
#endif
        }
*/

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::variant<Types...> m_variant;
#else
        Any m_any;
#endif
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_VARIANT_HPP

