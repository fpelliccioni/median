#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
#include <ranges>

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
requires (odd(WindowSize1) && odd(WindowSize2))
struct MovingMedian {

    constexpr static auto MinWindowSize = std::min(WindowSize1, WindowSize2);
    constexpr static auto MaxWindowSize = std::max(WindowSize1, WindowSize2);
    constexpr static auto DiffSizes = MaxWindowSize - MinWindowSize;

    constexpr
    void adjust_sorted(auto& sorted, T to_remove, T value) {
        // precondition: sorted.contains(to_remove)
        auto it = std::find(sorted.begin(), sorted.end(), to_remove);
        // std::cout << "found in sorted at index: " << it - sorted.begin() << std::endl;

        // print("sorted: ", sorted);

        *it = value;
        // print("sorted: ", sorted);
    }

    constexpr
    auto insert(T value) {
        using std::swap;

        if (size < MinWindowSize) {
            // std::cout << "----------------------------------\n";
            // std::cout << "size < MinWindowSize" << std::endl;
            data_[size] = value;
            sorted_max_[size] = value;
            sorted_min_[size] = value;
            // print("sorted_min: ", sorted_min_);
            // print("sorted_max: ", sorted_max_);
            // print("data:       ", data_);
            ++size;
            if (size < MinWindowSize) return *this;;

            std::ranges::nth_element(sorted_min_, sorted_min_.begin() + MinWindowSize / 2);
            // print("sorted_min: ", sorted_min_);
        } else if (size < MaxWindowSize) {
            // std::cout << "----------------------------------\n";
            // std::cout << "size < MaxWindowSize" << std::endl;

            data_[size] = value;
            sorted_max_[size] = value;

            // print("sorted_min: ", sorted_min_);
            // print("sorted_max: ", sorted_max_);
            // print("data:       ", data_);

            auto to_remove = data_[size - MinWindowSize];
            // std::cout << "to_remove index: " << (size - MinWindowSize) << std::endl;
            // std::cout << "to_remove elem:  " << to_remove << std::endl;
            adjust_sorted(sorted_min_, to_remove, value);
            // print("sorted_min: ", sorted_min_);
            std::ranges::nth_element(sorted_min_, sorted_min_.begin() + MinWindowSize / 2);
            // print("sorted_min: ", sorted_min_);

            ++size;
            if (size < MaxWindowSize) return *this;;

            std::ranges::nth_element(sorted_max_, sorted_max_.begin() + MaxWindowSize / 2);
            // print("sorted_max: ", sorted_max_);
        } else {
            // std::cout << "----------------------------------\n";
            // std::cout << "size == MaxWindowSize" << std::endl;

            auto to_remove = data_[0];
            shift_data();
            data_[MaxWindowSize - 1] = value;
            // print("data:       ", data_);

            adjust_sorted(sorted_max_, to_remove, value);
        }

        return *this;
    }

    constexpr
    std::pair<T, T> median() const {
        return {sorted_min_[MinWindowSize / 2], sorted_max_[MaxWindowSize / 2]};
    }

private:

    constexpr
    void shift_data() {
        // std::rotate(data_.begin(), data_.begin() + 1, data_.end());

        for (size_t i = 0; i < MaxWindowSize - 1; ++i) {
            data_[i] = data_[i + 1];
        }

        // loop<size_t, WindowSize - 1>([this] (auto i) {
        //     data_[i] = data_[i + 1];
        // });
    }


    std::array<T, MaxWindowSize> data_ = {};
    std::array<T, MinWindowSize> sorted_min_ = {};
    std::array<T, MaxWindowSize> sorted_max_ = {};
    size_t size = 0;
};




int main() {
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

    constexpr auto m1 = MovingMedian<int, 3, 5>{};
    static_assert(m1.median().first == 0);
    static_assert(m1.median().second == 0);

    constexpr auto m2 = MovingMedian<int, 3, 5>{}.insert(10).insert(5);
    static_assert(m2.median().first == 0);
    static_assert(m2.median().second == 0);

    // median.insert(10);
    // median.insert(5);
    // median.insert(100);
    // median.insert(4);
    // median.insert(102);
    // median.insert(2);


}


