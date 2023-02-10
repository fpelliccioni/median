#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

constexpr
bool odd(std::integral auto value) {
    return (value & 1) == 1;
}

constexpr
bool even(std::integral auto value) {
    return (value & 1) == 0;
}

template <std::integral T, size_t WindowSize1, size_t WindowSize2>
requires (odd(WindowSize1) &&
          odd(WindowSize2) &&
          WindowSize1 != WindowSize2)
struct DualMedian {
    constexpr static auto MinWindowSize = std::min(WindowSize1, WindowSize2);
    constexpr static auto MaxWindowSize = std::max(WindowSize1, WindowSize2);

    constexpr
    auto insert(T value) noexcept {
        if (size < MaxWindowSize) {
            data_[size] = value;
            ++size;
        } else {
            shift_data(1);
            data_[MaxWindowSize - 1] = value;
        }

        calculate_medians();
        return *this;
    }

    constexpr
    auto insert(std::ranges::input_range auto const& rng) noexcept {
        auto s = std::size(rng);
        if (s == 0) return *this;

        if (s >= MaxWindowSize) {
            std::copy_n(std::begin(rng) + (s - MaxWindowSize), MaxWindowSize, data_.begin());
            size = MaxWindowSize;
        } else {
            if (size + s > MaxWindowSize) {
                auto to_shift = size + s - MaxWindowSize;
                shift_data(to_shift);
                std::copy_n(std::begin(rng), s, data_.begin() + (size - to_shift));
                size += s - to_shift;
            } else {
                std::copy_n(std::begin(rng), s, data_.begin() + size);
                size += s;
            }
        }

        calculate_medians();
        return *this;
    }

    constexpr
    std::pair<T, T> median() const {
        if constexpr(WindowSize1 < WindowSize2) {
            return {median0_, median1_};
        } else {
            return {median1_, median0_};
        }
    }

// private:
    constexpr
    void shift_data(size_t n) noexcept {
        for (size_t i = 0; i < MaxWindowSize - n; ++i) {
            data_[i] = data_[i + n];
        }
    }

    constexpr
    auto calculate_medians() noexcept {
        if (size == MaxWindowSize) {
            auto tmp = data_;
            std::nth_element(tmp.begin(), tmp.begin() + MaxWindowSize / 2, tmp.end());
            median1_ = tmp[MaxWindowSize / 2];
            auto first = MaxWindowSize - MinWindowSize;
            std::nth_element(tmp.begin() + first, tmp.begin() + first + MinWindowSize / 2, tmp.begin() + first + MinWindowSize);
            median0_ = tmp[first + MinWindowSize / 2];
            return;
        }

        if (size >= MinWindowSize) {
            auto tmp = data_;
            auto first = size - MinWindowSize;
            std::nth_element(tmp.begin() + first, tmp.begin() + first + MinWindowSize / 2, tmp.begin() + first + MinWindowSize);
            median0_ = tmp[first + MinWindowSize / 2];
        }
    }

    std::array<T, MaxWindowSize> data_ = {};
    T median0_ = {};
    T median1_ = {};
    size_t size = 0;
};


void static_tests() {
    using DM = DualMedian<int, 3, 5>;

    // constexpr MovingMedian<int, 3, 5> median;

    // // median.insert(1);
    // // median.insert(2);
    // // median.insert(3);
    // // std::cout << median.median() << std::endl;
    // // median.insert(4);
    // // std::cout << median.median() << std::endl;

    // median.insert(10);
    // median.insert(5);
    // median.insert(100);
    // median.insert(4);
    // median.insert(102);
    // median.insert(2);


    constexpr auto m0 = DM{};
    static_assert(m0.median().first == 0);
    static_assert(m0.median().second == 0);

    constexpr auto m1 = DM{}.insert(10);
    static_assert(m1.median().first == 0);
    static_assert(m1.median().second == 0);

    constexpr auto m2 = DM{}.insert(10).insert(5);
    static_assert(m2.median().first == 0);
    static_assert(m2.median().second == 0);

    constexpr auto m3 = DM{}.insert(10).insert(5).insert(100);
    static_assert(m3.size == 3);
    static_assert(m3.data_ == std::array<int, 5>{10,5,100,0,0});
    static_assert(m3.median().first == 10);
    static_assert(m3.median().second == 0);

    constexpr auto m4 = DM{}.insert(10).insert(5).insert(100).insert(4);
    static_assert(m4.size == 4);
    static_assert(m4.data_ == std::array<int, 5>{10,5,100,4,0});
    static_assert(m4.median().first == 5);
    static_assert(m4.median().second == 0);

    constexpr auto m4b = DM{}.insert(10).insert(5).insert(100).insert(-1);
    static_assert(m4b.size == 4);
    static_assert(m4b.data_ == std::array<int, 5>{10,5,100,-1,0});
    static_assert(m4b.median().first == 5);
    static_assert(m4b.median().second == 0);


    constexpr auto m5 = DM{}.insert(10).insert(5).insert(100).insert(4).insert(102);
    static_assert(m5.size == 5);
    static_assert(m5.data_ == std::array<int, 5>{10,5,100,4,102});
    static_assert(m5.median().first == 100);
    static_assert(m5.median().second == 10);

    constexpr auto m5b = DM{}.insert(std::vector{10, 5, 100, 4, 102});
    static_assert(m5b.size == 5);
    static_assert(m5b.data_ == std::array<int, 5>{10,5,100,4,102});
    static_assert(m5b.median().first == 100);
    static_assert(m5b.median().second == 10);


    constexpr auto m6 = DM{}.insert(std::vector{10, 5, 100, 4, 102, 6});
    static_assert(m6.size == 5);
    static_assert(m6.data_ == std::array<int, 5>{5,100,4,102,6});
    static_assert(m6.median().first == 100);
    static_assert(m6.median().second == 6);


    constexpr auto m7 = DM{}.insert(std::vector{10, 5, 100, 4, 102, 6, 123});
    static_assert(m7.size == 5);
    static_assert(m7.data_ == std::array<int, 5>{100,4,102,6,123});
    static_assert(m7.median().first == 102);
    static_assert(m7.median().second == 100);

    constexpr auto m8 = DM{}.insert(std::vector{1,2,3,4,5}).insert(std::vector{10, 5, 100, 4, 102, 6, 123});
    static_assert(m8.size == 5);
    static_assert(m8.data_ == std::array<int, 5>{100,4,102,6,123});
    static_assert(m8.median().first == 102);
    static_assert(m8.median().second == 100);

    constexpr auto m9 = DM{}.insert(1).insert(std::vector{10, 5, 100, 4, 102, 6, 123});
    static_assert(m9.size == 5);
    static_assert(m9.data_ == std::array<int, 5>{100,4,102,6,123});
    static_assert(m9.median().first == 102);
    static_assert(m9.median().second == 100);

    constexpr auto m10 = DM{}.insert(1).insert(std::vector{10, 5, 100, 4});
    static_assert(m10.size == 5);
    static_assert(m10.data_ == std::array<int, 5>{1,10,5,100,4});
    static_assert(m10.median().first == 10);
    static_assert(m10.median().second == 5);

    constexpr auto m11 = DM{}.insert(1).insert(2).insert(std::vector{90,80,70,60});
    static_assert(m11.size == 5);
    static_assert(m11.data_ == std::array<int, 5>{2,90,80,70,60});
    static_assert(m11.median().first == 80);
    static_assert(m11.median().second == 70);

    constexpr auto m12 = DM{}.insert(std::vector{1,2,3}).insert(std::vector{90,80,70,60});
    static_assert(m12.size == 5);
    static_assert(m12.data_ == std::array<int, 5>{3,90,80,70,60});
    static_assert(m12.median().first == 80);
    static_assert(m12.median().second == 70);

    constexpr auto m13 = DM{}.insert(std::vector{1,2,3,4}).insert(std::vector{90,80,70,60});
    static_assert(m13.size == 5);
    static_assert(m13.data_ == std::array<int, 5>{4,90,80,70,60});
    static_assert(m13.median().first == 80);
    static_assert(m13.median().second == 70);

    {
    constexpr auto m14 = DM{}.insert(std::vector{1,2,3,4,5}).insert(std::vector{90,80,70,60});
    static_assert(m14.size == 5);
    static_assert(m14.data_ == std::array<int, 5>{5,90,80,70,60});
    static_assert(m14.median().first == 80);
    static_assert(m14.median().second == 70);
    }

    {
    constexpr auto m = DM{}.insert(std::vector{1,2,3,4,5,6}).insert(std::vector{90,80,70,60});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{6,90,80,70,60});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = DM{}.insert(std::vector<int>{}).insert(std::vector{90,80,70});
    static_assert(m.size == 3);
    static_assert(m.data_ == std::array<int, 5>{90,80,70,0,0});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 0);
    }
    {
    constexpr auto m = DM{}.insert(std::vector{1}).insert(std::vector{90,80,70});
    static_assert(m.size == 4);
    static_assert(m.data_ == std::array<int, 5>{1,90,80,70,0});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 0);
    }

    {
    constexpr auto m = DM{}.insert(std::vector{1,2}).insert(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{1,2,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = DM{}.insert(std::vector{1,2,3}).insert(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{2,3,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = DM{}.insert(std::vector{1,2,3,4}).insert(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{3,4,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = DM{}.insert(std::vector{1,2,3,4,5}).insert(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{4,5,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = DM{}.insert(std::vector{1,2,3,4,5,6}).insert(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{5,6,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }
}
