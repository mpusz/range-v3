// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_DETAIL_COMPRESSED_PAIR_HPP
#define RANGES_V3_DETAIL_COMPRESSED_PAIR_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // BUGBUG add noexcept
            // BUGBUG && overloads should return by value, not by &&. BUT, then calling first()
            // twice on an rvalue compressed pair has UB. Hrm.
            template<typename First, typename Second, typename Enable /*= void*/>
            struct compressed_pair
            {
            private:
                First first_;
                Second second_;
            public:
                compressed_pair() = default;
                template<typename F, typename S>
                constexpr compressed_pair(F && f, S && s)
                  : first_(detail::forward<F>(f)), second_(detail::forward<S>(s))
                {}
                auto first() & -> First & { return first_; }
                auto second() & -> Second & { return second_; }
                constexpr auto first() const & -> First const & { return first_; }
                constexpr auto second() const & -> Second const & { return second_; }
                constexpr auto first() const && -> First &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)).first_; }
                constexpr auto second() const && -> Second &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)).second_; }
            };

            template<typename First, typename Second>
            struct compressed_pair<
                First
              , Second
              , typename std::enable_if<std::is_empty<First>::value &&
                                       !std::is_empty<Second>::value>::type
            >
              : private First
            {
            private:
                Second second_;
            public:
                compressed_pair() = default;
                template<typename F, typename S>
                constexpr compressed_pair(F && f, S && s)
                  : First(detail::forward<F>(f)), second_(detail::forward<S>(s))
                {}
                auto first() & -> First & { return *this; }
                auto second() & -> Second & { return second_; }
                constexpr auto first() const & -> First const & { return *this; }
                constexpr auto second() const & -> Second const & { return second_; }
                constexpr auto first() const && -> First &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)); }
                constexpr auto second() const && -> Second &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)).second_; }
            };

            template<typename First, typename Second>
            struct compressed_pair<
                First
              , Second
              , typename std::enable_if<!std::is_empty<First>::value &&
                                         std::is_empty<Second>::value>::type
            >
              : private Second
            {
            private:
                First first_;
            public:
                compressed_pair() = default;
                template<typename F, typename S>
                constexpr compressed_pair(F && f, S && s)
                  : Second(detail::forward<S>(s)), first_(detail::forward<F>(f))
                {}
                auto first() & -> First & { return first_; }
                auto second() & -> Second & { return *this; }
                constexpr auto first() const & -> First const & { return first_; }
                constexpr auto second() const & -> Second const & { return *this; }
                constexpr auto first() const && -> First &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)).first_; }
                constexpr auto second() const && -> Second &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)); }
            };

            template<typename First, typename Second>
            struct compressed_pair<
                First
              , Second
              , typename std::enable_if<std::is_empty<First>::value &&
                                        std::is_empty<Second>::value>::type
            >
              : private First, private Second
            {
                compressed_pair() = default;
                template<typename F, typename S>
                constexpr compressed_pair(F && f, S && s)
                  : First(detail::forward<F>(f)), Second(detail::forward<S>(s))
                {}
                auto first() & -> First & { return *this; }
                auto second() & -> Second & { return *this; }
                constexpr auto first() const & -> First const & { return *this; }
                constexpr auto second() const & -> Second const & { return *this; }
                constexpr auto first() const && -> First &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)); }
                constexpr auto second() const && -> Second &&
                    { return detail::move(const_cast<compressed_pair &&>(*this)); }
            };

            constexpr struct compressed_pair_maker
            {
                template<typename First, typename Second>
                constexpr auto operator()(First && f, Second && s) const ->
                    compressed_pair<First, Second>
                {
                    return {detail::forward<First>(f), detail::forward<Second>(s)};
                }
            } make_compressed_pair {};

            namespace
            {
                inline void test_compressed_pair()
                {
                    constexpr int i = compressed_pair<int, int>{4,5}.first();
                    compressed_pair<int, int> p{4,5};
                    int & j = p.first();
                    compressed_pair<int, int> const pc{4,5};
                    int const & k = pc.first();
                    int & l = make_compressed_pair(j, 42).first();
                    static_assert(std::is_same<decltype(make_compressed_pair(j, 42).second()), int &&>::value, "");
                }
            }
        }
    }
}

#endif

