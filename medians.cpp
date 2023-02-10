#include <array>

#include "medians.hpp"

using DM = DualMedian<int, 3, 5>;

int main() {
    auto ret = DM{};
    ret.insert(std::array{1,2,3,4,5,6});
    ret.insert(std::array{90,80,70});
    auto const [m0, m1] = ret.median();
    return std::max(m0, m1);
}
