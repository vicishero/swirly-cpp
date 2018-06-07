/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_FIN_LEVEL_HPP
#define SWIRLY_FIN_LEVEL_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/app/MemAlloc.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {
inline namespace fin {

using LevelKey = Ticks::ValueType;

namespace detail {

/**
 * Synthetic level key.
 */
constexpr LevelKey compose_key(Side side, Ticks ticks) noexcept
{
    return (side == Side::Buy ? -ticks : ticks).count();
}

} // namespace detail

/**
 * Price level.
 *
 * A price level is an aggregation of orders by price. I.e. the sum of all orders in the market at
 * the same price.
 */
class SWIRLY_API Level
: public Comparable<Level>
, public MemAlloc {
  public:
    explicit Level(const Order& first_order) noexcept;

    ~Level();

    // Copy.
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;

    // Move.
    Level(Level&&);
    Level& operator=(Level&&) = delete;

    int compare(const Level& rhs) const noexcept { return swirly::compare(key_, rhs.key_); }
    const Order& first_order() const noexcept { return *first_order_; }
    LevelKey key() const noexcept { return key_; }
    Ticks ticks() const noexcept { return ticks_; }
    Lots lots() const noexcept { return lots_; }
    int count() const noexcept { return count_; }
    void set_first_order(const Order& first_order) noexcept { first_order_ = &first_order; }
    void reduce(Lots delta) noexcept { lots_ -= delta; }
    void add_order(const Order& order) noexcept;

    void sub_order(const Order& order) noexcept;

    boost::intrusive::set_member_hook<> key_hook;

  private:
    const Order* first_order_;
    const LevelKey key_;
    const Ticks ticks_;
    /**
     * Must be greater than zero.
     */
    Lots lots_;
    /**
     * Must be greater than zero.
     */
    int count_;
};

class SWIRLY_API LevelSet {
    struct ValueCompare {
        bool operator()(const Level& lhs, const Level& rhs) const noexcept
        {
            return lhs.key() < rhs.key();
        }
    };
    struct KeyValueCompare {
        bool operator()(LevelKey lhs, const Level& rhs) const noexcept { return lhs < rhs.key(); }
        bool operator()(const Level& lhs, LevelKey rhs) const noexcept { return lhs.key() < rhs; }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<Level, decltype(Level::key_hook), &Level::key_hook>;
    using Set
        = boost::intrusive::set<Level, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = std::unique_ptr<Level>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    LevelSet() = default;
    ~LevelSet();

    // Copy.
    LevelSet(const LevelSet&) = delete;
    LevelSet& operator=(const LevelSet&) = delete;

    // Move.
    LevelSet(LevelSet&&);
    LevelSet& operator=(LevelSet&&);

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(Side side, Ticks ticks) const noexcept
    {
        return set_.find(detail::compose_key(side, ticks), KeyValueCompare());
    }
    Iterator find(Side side, Ticks ticks) noexcept
    {
        return set_.find(detail::compose_key(side, ticks), KeyValueCompare());
    }
    std::pair<ConstIterator, bool> find_hint(Side side, Ticks ticks) const noexcept
    {
        const auto key = detail::compose_key(side, ticks);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    std::pair<Iterator, bool> find_hint(Side side, Ticks ticks) noexcept
    {
        const auto key = detail::compose_key(side, ticks);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    Iterator insert(ValuePtr value) noexcept;

    Iterator insert_hint(ConstIterator hint, ValuePtr value) noexcept;

    Iterator insert_or_replace(ValuePtr value) noexcept;

    void remove(const Level& level) noexcept;

    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(std::make_unique<Level>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_hint(ConstIterator hint, ArgsT&&... args)
    {
        return insert_hint(hint, std::make_unique<Level>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_or_replace(ArgsT&&... args)
    {
        return insert_or_replace(std::make_unique<Level>(std::forward<ArgsT>(args)...));
    }

  private:
    Set set_;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_LEVEL_HPP
