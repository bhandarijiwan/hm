//
// Created by Jiwan on 1/18/21.
//

#ifndef HM_MACROS_H
#define HM_MACROS_H
#define CAT(x, y) x##_##y

#define INS_BEGIN(x) auto CAT(x, _begin) = std::chrono::steady_clock::now();
#define INS_END(x) auto CAT(x, _end) = std::chrono::steady_clock::now();
#define INS_PRINT_NANO(x, z)                                                   \
  fmt::print("{} = {}\n", z,                                                   \
             std::chrono::duration_cast<std::chrono::nanoseconds>(             \
                 CAT(x, _end) - CAT(x, _begin))                                \
                 .count());

#endif // HM_MACROS_H
