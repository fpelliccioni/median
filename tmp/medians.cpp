#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
// #include <ranges>

// #include <iostream>

constexpr
bool odd(std::integral auto value) {
    return (value & 1) == 1;
}

constexpr
bool even(std::integral auto value) {
    return (value & 1) == 0;
}

namespace detail {

template <typename T, T... inds, typename F>
constexpr
void loop(std::integer_sequence<T, inds...>, F&& f) {
    (f(std::integral_constant<T, inds>{}), ...);
}

} // namespace detail

template <typename T, T count, typename F>
constexpr
void loop(F&& f) {
    detail::loop(std::make_integer_sequence<T, count>{}, std::forward<F>(f));
}

// void print(std::string_view rem, std::ranges::input_range auto const& a) {
//     for (std::cout << rem; const auto e : a)
//         std::cout << e << ' ';
//     std::cout << "\n";
// }

template <std::integral T, size_t WindowSize1, size_t WindowSize2>
requires (odd(WindowSize1) && odd(WindowSize2) && WindowSize1 != WindowSize2)
struct MovingMedian {

    constexpr static auto MinWindowSize = std::min(WindowSize1, WindowSize2);
    constexpr static auto MaxWindowSize = std::max(WindowSize1, WindowSize2);

    constexpr
    auto insert(T value) {

        if (size < MaxWindowSize) {
            data_[size] = value;
            ++size;
        } else {
            shift_data_1();
            data_[MaxWindowSize - 1] = value;
        }

        if (size == MaxWindowSize) {
            auto tmp = data_;
            // std::ranges::nth_element(tmp, tmp.begin() + MaxWindowSize / 2);
            std::nth_element(tmp.begin(), tmp.begin() + MaxWindowSize / 2, tmp.end());
            median1_ = tmp[MaxWindowSize / 2];
            auto first = MaxWindowSize - MinWindowSize;

            // std::ranges::nth_element(tmp | std::views::drop(first) | std::ranges::views::take(MinWindowSize), tmp.begin() + first + MinWindowSize / 2);
            std::nth_element(tmp.begin() + first, tmp.begin() + first + MinWindowSize / 2, tmp.begin() + first + MinWindowSize);

            median0_ = tmp[first + MinWindowSize / 2];
            return *this;
        }

        if (size >= MinWindowSize) {
            auto tmp = data_;
            auto first = size - MinWindowSize;

            // std::ranges::nth_element(tmp | std::views::drop(first) | std::ranges::views::take(MinWindowSize), tmp.begin() + first + MinWindowSize / 2);
            std::nth_element(tmp.begin() + first, tmp.begin() + first + MinWindowSize / 2, tmp.begin() + first + MinWindowSize);

            median0_ = tmp[first + MinWindowSize / 2];
        }

        return *this;
    }

    constexpr inline
    auto insert_range(std::ranges::input_range auto const& rng) {
        auto s = std::size(rng);
        if (s == 0) return *this;

        if (s >= MaxWindowSize) {
            // std::ranges::copy_n(std::begin(rng | std::views::drop(s - MaxWindowSize)), MaxWindowSize, data_.begin());
            std::copy_n(std::begin(rng) + (s - MaxWindowSize), MaxWindowSize, data_.begin());

            size = MaxWindowSize;
        } else {
            if (size + s > MaxWindowSize) {
                auto to_shift = size + s - MaxWindowSize;
                shift_data_n(to_shift);

                // std::ranges::copy_n(std::begin(rng), s, data_.begin() + (size - to_shift));
                std::copy_n(std::begin(rng), s, data_.begin() + (size - to_shift));

                size += s - to_shift;
            } else {
                // std::ranges::copy_n(std::begin(rng), s, data_.begin() + size);
                std::copy_n(std::begin(rng), s, data_.begin() + size);
                size += s;
            }
        }

        if (size == MaxWindowSize) {
            auto tmp = data_;

            // std::ranges::nth_element(tmp, tmp.begin() + MaxWindowSize / 2);
            std::nth_element(tmp.begin(), tmp.begin() + MaxWindowSize / 2, tmp.end());

            median1_ = tmp[MaxWindowSize / 2];
            auto first = MaxWindowSize - MinWindowSize;

            // std::ranges::nth_element(tmp | std::views::drop(first) | std::ranges::views::take(MinWindowSize), tmp.begin() + first + MinWindowSize / 2);
            std::nth_element(tmp.begin() + first, tmp.begin() + first + MinWindowSize / 2, tmp.begin() + first + MinWindowSize);

            median0_ = tmp[first + MinWindowSize / 2];
            return *this;
        }

        if (size >= MinWindowSize) {
            auto tmp = data_;
            auto first = size - MinWindowSize;

            // std::ranges::nth_element(tmp | std::views::drop(first) | std::ranges::views::take(MinWindowSize), tmp.begin() + first + MinWindowSize / 2);
            std::nth_element(tmp.begin() + first, tmp.begin() + first + MinWindowSize / 2, tmp.begin() + first + MinWindowSize);

            median0_ = tmp[first + MinWindowSize / 2];
        }

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
    void shift_data_1() {
        // std::rotate(data_.begin(), data_.begin() + 1, data_.end());

        for (size_t i = 0; i < MaxWindowSize - 1; ++i) {
            data_[i] = data_[i + 1];
        }

        // loop<size_t, MaxWindowSize - 1>([this] (auto i) {
        //     data_[i] = data_[i + 1];
        // });
    }

    constexpr
    void shift_data_n(size_t n) {
        // std::rotate(data_.begin(), data_.begin() + 1, data_.end());

        for (size_t i = 0; i < MaxWindowSize - n; ++i) {
            data_[i] = data_[i + n];
        }

        // loop<size_t, MaxWindowSize - n>([this] (auto i) {
        //     data_[i] = data_[i + n];
        // });
    }

    std::array<T, MaxWindowSize> data_ = {};
    T median0_ = {};
    T median1_ = {};
    size_t size = 0;
};

using MM = MovingMedian<int, 3, 5>;

void static_tests() {
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


    constexpr auto m0 = MM{};
    static_assert(m0.median().first == 0);
    static_assert(m0.median().second == 0);

    constexpr auto m1 = MM{}.insert(10);
    static_assert(m1.median().first == 0);
    static_assert(m1.median().second == 0);

    constexpr auto m2 = MM{}.insert(10).insert(5);
    static_assert(m2.median().first == 0);
    static_assert(m2.median().second == 0);

    constexpr auto m3 = MM{}.insert(10).insert(5).insert(100);
    static_assert(m3.size == 3);
    static_assert(m3.data_ == std::array<int, 5>{10,5,100,0,0});
    static_assert(m3.median().first == 10);
    static_assert(m3.median().second == 0);

    constexpr auto m4 = MM{}.insert(10).insert(5).insert(100).insert(4);
    static_assert(m4.size == 4);
    static_assert(m4.data_ == std::array<int, 5>{10,5,100,4,0});
    static_assert(m4.median().first == 5);
    static_assert(m4.median().second == 0);

    constexpr auto m4b = MM{}.insert(10).insert(5).insert(100).insert(-1);
    static_assert(m4b.size == 4);
    static_assert(m4b.data_ == std::array<int, 5>{10,5,100,-1,0});
    static_assert(m4b.median().first == 5);
    static_assert(m4b.median().second == 0);


    constexpr auto m5 = MM{}.insert(10).insert(5).insert(100).insert(4).insert(102);
    static_assert(m5.size == 5);
    static_assert(m5.data_ == std::array<int, 5>{10,5,100,4,102});
    static_assert(m5.median().first == 100);
    static_assert(m5.median().second == 10);

    constexpr auto m5b = MM{}.insert_range(std::vector{10, 5, 100, 4, 102});
    static_assert(m5b.size == 5);
    static_assert(m5b.data_ == std::array<int, 5>{10,5,100,4,102});
    static_assert(m5b.median().first == 100);
    static_assert(m5b.median().second == 10);


    constexpr auto m6 = MM{}.insert_range(std::vector{10, 5, 100, 4, 102, 6});
    static_assert(m6.size == 5);
    static_assert(m6.data_ == std::array<int, 5>{5,100,4,102,6});
    static_assert(m6.median().first == 100);
    static_assert(m6.median().second == 6);


    constexpr auto m7 = MM{}.insert_range(std::vector{10, 5, 100, 4, 102, 6, 123});
    static_assert(m7.size == 5);
    static_assert(m7.data_ == std::array<int, 5>{100,4,102,6,123});
    static_assert(m7.median().first == 102);
    static_assert(m7.median().second == 100);

    constexpr auto m8 = MM{}.insert_range(std::vector{1,2,3,4,5}).insert_range(std::vector{10, 5, 100, 4, 102, 6, 123});
    static_assert(m8.size == 5);
    static_assert(m8.data_ == std::array<int, 5>{100,4,102,6,123});
    static_assert(m8.median().first == 102);
    static_assert(m8.median().second == 100);

    constexpr auto m9 = MM{}.insert(1).insert_range(std::vector{10, 5, 100, 4, 102, 6, 123});
    static_assert(m9.size == 5);
    static_assert(m9.data_ == std::array<int, 5>{100,4,102,6,123});
    static_assert(m9.median().first == 102);
    static_assert(m9.median().second == 100);

    constexpr auto m10 = MM{}.insert(1).insert_range(std::vector{10, 5, 100, 4});
    static_assert(m10.size == 5);
    static_assert(m10.data_ == std::array<int, 5>{1,10,5,100,4});
    static_assert(m10.median().first == 10);
    static_assert(m10.median().second == 5);

    constexpr auto m11 = MM{}.insert(1).insert(2).insert_range(std::vector{90,80,70,60});
    static_assert(m11.size == 5);
    static_assert(m11.data_ == std::array<int, 5>{2,90,80,70,60});
    static_assert(m11.median().first == 80);
    static_assert(m11.median().second == 70);

    constexpr auto m12 = MM{}.insert_range(std::vector{1,2,3}).insert_range(std::vector{90,80,70,60});
    static_assert(m12.size == 5);
    static_assert(m12.data_ == std::array<int, 5>{3,90,80,70,60});
    static_assert(m12.median().first == 80);
    static_assert(m12.median().second == 70);

    constexpr auto m13 = MM{}.insert_range(std::vector{1,2,3,4}).insert_range(std::vector{90,80,70,60});
    static_assert(m13.size == 5);
    static_assert(m13.data_ == std::array<int, 5>{4,90,80,70,60});
    static_assert(m13.median().first == 80);
    static_assert(m13.median().second == 70);

    {
    constexpr auto m14 = MM{}.insert_range(std::vector{1,2,3,4,5}).insert_range(std::vector{90,80,70,60});
    static_assert(m14.size == 5);
    static_assert(m14.data_ == std::array<int, 5>{5,90,80,70,60});
    static_assert(m14.median().first == 80);
    static_assert(m14.median().second == 70);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector{1,2,3,4,5,6}).insert_range(std::vector{90,80,70,60});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{6,90,80,70,60});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector<int>{}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 3);
    static_assert(m.data_ == std::array<int, 5>{90,80,70,0,0});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 0);
    }
    {
    constexpr auto m = MM{}.insert_range(std::vector{1}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 4);
    static_assert(m.data_ == std::array<int, 5>{1,90,80,70,0});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 0);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector{1,2}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{1,2,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector{1,2,3}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{2,3,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector{1,2,3,4}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{3,4,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector{1,2,3,4,5}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{4,5,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }

    {
    constexpr auto m = MM{}.insert_range(std::vector{1,2,3,4,5,6}).insert_range(std::vector{90,80,70});
    static_assert(m.size == 5);
    static_assert(m.data_ == std::array<int, 5>{5,6,90,80,70});
    static_assert(m.median().first == 80);
    static_assert(m.median().second == 70);
    }
}

int main() {
    static_tests();

    auto ret = MM{};
    ret.insert_range(std::array{1,2,3,4,5,6});
    ret.insert_range(std::array{90,80,70});
    auto const [m0, m1] = ret.median();
    return std::max(m0, m1);




    // auto tmp = m5.data_;

    // print("sorted: ", tmp);
    // std::ranges::nth_element(tmp, tmp.begin() + 5 / 2);
    // print("sorted: ", tmp);


    // auto drop = 5 - 3;
    // std::ranges::nth_element(tmp | std::views::drop(drop) | std::ranges::views::take(3), tmp.begin() + drop + 3 / 2);
    // auto median0_ = tmp[drop + 3 / 2];
    // print("sorted: ", tmp);

    // median.insert(10);
    // median.insert(5);
    // median.insert(100);
    // median.insert(4);
    // median.insert(102);
    // median.insert(2);


}


