#include "final_tuple.h"

int main() {
    tuple<int, double, float> t;
    get<0>(t);
    ++get<0>(t);

    std::cout << get<0>(t) << '\n' << get<1>(t) << '\n' << get<2>(t) << '\n';
    const tuple<int, double, int, float> e(1, 2.12, 3, 2.1221);

    std::cout << get<0>(e) << '\n' << get<1>(e) << '\n' << get<2>(e) << '\n' << get<3>(e) << '\n';
    return 0;
}
