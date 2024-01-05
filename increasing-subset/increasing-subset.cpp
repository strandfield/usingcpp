
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

// problem: given a list of integers, we want to extract a sublist that is:
// - strictly increasing;
// - of maximum length.
// note: elements of the sublist need not be consecutive elements of the
// original list, but the order must be preserved.
//
// example:
// with the list [1, 3, 7, 5]
// we can extract many sublists, including:
// - [1, 7, 5], but it is not strictly increasing;
// - [1, 5], but it is not of maximal length;
// - [1, 3, 5] and [1, 3, 7].
// the maximum length of the sublist satisfying the requirements is 3.

// recursively compute the length of the longest increasing subset that
// can be constructed from the numbers in [begin, end) by 'constructing'
// on-the-fly all such subsets.
// 'maxval' is used for the recursion to carry the max value of the subset
// currently being tested.
size_t compute_length_of_longest_increasing_subset(
    std::vector<int>::const_iterator begin,
    std::vector<int>::const_iterator end,
    int maxval = std::numeric_limits<int>::min()) {
    if (begin == end) return 0;

    int val = *begin;

    if (val <= maxval) // 'val' cannot be part of the current subset
    {
        return compute_length_of_longest_increasing_subset(std::next(begin),
                                                           end, maxval);
    } else {
        // 'val' CAN be part of an increasing subset, but that doesn't mean it
        // SHOULD. we need to test both the subset with 'val' and the one
        // without it.
        size_t without_val = compute_length_of_longest_increasing_subset(
            std::next(begin), end, maxval);
        size_t with_val = compute_length_of_longest_increasing_subset(
            std::next(begin), end, val);
        return std::max(1 + with_val, without_val);
    }
}

// returns the length of the longest increasing subset that can be constructed
// with the given 'numbers'
size_t
compute_length_of_longest_increasing_subset(const std::vector<int>& numbers) {
    return compute_length_of_longest_increasing_subset(numbers.begin(),
                                                       numbers.end());
}

// returns the max value in a vector with increasing elements.
// this effectively returns the last element; unless the vector
// is empty in which case it returns INT_MIN, a value that will
// compare less than all other integers.
inline int subset_max_value(const std::vector<int>& increasing_values) {
    return increasing_values.empty() ? std::numeric_limits<int>::min()
                                     : increasing_values.back();
}

// builds all increasing subsets of a range of integers.
// this function aims at being exhaustive and is therefore both
// slow and memory consuming.
std::vector<std::vector<int>>
build_all_increasing_subsets(std::vector<int>::const_iterator begin,
                             std::vector<int>::const_iterator end) {
    if (begin == end) return {};

    std::vector<int>::const_iterator it = std::prev(end);
    int value = *it;

    std::vector<std::vector<int>> subsets =
        build_all_increasing_subsets(begin, it);

    // size_t nb_subsets = subsets.size();

    for (auto subset_iterator = subsets.begin();
         subset_iterator != subsets.end();) {
        if (subset_max_value(*subset_iterator) >= value) {
            ++subset_iterator;
        } else {
            std::vector<int> new_subset = *subset_iterator;
            new_subset.push_back(value);
            subset_iterator = subsets.insert(std::next(subset_iterator),
                                             std::move(new_subset));
        }
    }

    // if (nb_subsets == subsets.size()) { // if no new subset was created
    // // we create one with a single element
    subsets.push_back(std::vector<int>{value});
    // }

    return subsets;
}

std::vector<std::vector<int>>
build_all_increasing_subsets(const std::vector<int>& values) {
    return build_all_increasing_subsets(values.begin(), values.end());
}

// recursively builds a set of candidates for the award of
// "longest increasing subset" of a set of integers.
//
// the function processes efficiently by removing candidates
// along the way when they can no longer be part of the
// longest subset.
//
// two rules of simplication are used:
// - at most one subset of a given length is kept
//   --> the one with the smallest maximum value
// - no two subsets can end with the same value
//   --> we keep the longest one
std::vector<std::vector<int>>
build_increasing_subsets_candidates(std::vector<int>::const_iterator begin,
                                    std::vector<int>::const_iterator end) {
    if (begin == end) return {};

    std::vector<int>::const_iterator it = std::prev(end);

    // computes a set of candidates for the beginning of the range.
    // 'subsets' is sorted by increasing maximum value
    std::vector<std::vector<int>> subsets =
        build_increasing_subsets_candidates(begin, it);

    // we will use 'value' to construct a new subset that we will insert in the
    // list of all candidates.
    int value = *it;

    // find the place in 'subsets' where a new subset, ending with 'value' will
    // be inserted.
    auto insert_it =
        std::lower_bound(subsets.begin(), subsets.end(), value,
                         [](const std::vector<int>& subset, int v) {
                             return subset_max_value(subset) < v;
                         });

    size_t target_length; // length of the new subset we will insert in the list
                          // of candidates

    if (insert_it == subsets.begin()) {
        // insert a new subset of length 1 at the beginning of the candidates
        insert_it = subsets.insert(subsets.begin(), std::vector<int>{value});
        target_length = 1;
    } else {
        // find the longest subset among those that will be placed before
        // the new subset in the list of candidates.
        // this longest subset will be used as a base for constructing
        // the new subset.
        auto longest_subset_it = std::max_element(
            subsets.begin(), insert_it,
            [](const std::vector<int>& a, const std::vector<int>& b) {
                return a.size() < b.size();
            });

        // compute the length of the new subset
        size_t current_max_length = longest_subset_it->size();
        target_length = 1 + current_max_length;

        // build the new subset
        std::vector<int> newsubset = *longest_subset_it;
        newsubset.push_back(value);

        // insert the new subset at the right location
        insert_it = subsets.insert(insert_it, std::move(newsubset));
    }

    // remove all subsets that can no longer be used to construct the longest
    // subset.
    // this means the subset that have a maximum value greater than the one
    // we just created and are shorter.
    auto remove_iterator =
        std::remove_if(std::next(insert_it), subsets.end(),
                       [target_length, value](const std::vector<int>& subset) {
                           return subset_max_value(subset) >= value &&
                                  subset.size() <= target_length;
                       });

    subsets.erase(remove_iterator, subsets.end());

    return subsets;
}

class IncreasingSubsetExtractor {
  private:
    std::vector<int> m_numbers; // the input numbers
    std::vector<std::vector<int>> m_subsets;

  public:
    // appends a new value to the list of input numbers and
    // updates the increasing subsets.
    void feed(int n) {
        m_numbers.push_back(n);

        // code is a copy-pasta of the above algorithm

        auto insert_it =
            std::lower_bound(m_subsets.begin(), m_subsets.end(), n,
                             [](const std::vector<int>& subset, int v) {
                                 return subset_max_value(subset) < v;
                             });

        size_t target_length;

        if (insert_it == m_subsets.begin()) {
            insert_it =
                m_subsets.insert(m_subsets.begin(), std::vector<int>{n});
            target_length = 1;
        } else {
            auto longest_subset_it = std::max_element(
                m_subsets.begin(), insert_it,
                [](const std::vector<int>& a, const std::vector<int>& b) {
                    return a.size() < b.size();
                });

            size_t current_max_length = longest_subset_it->size();
            target_length = 1 + current_max_length;

            std::vector<int> newsubset = *longest_subset_it;
            newsubset.push_back(n);

            insert_it = m_subsets.insert(insert_it, std::move(newsubset));
        }

        auto remove_iterator =
            std::remove_if(std::next(insert_it), m_subsets.end(),
                           [target_length, n](const std::vector<int>& subset) {
                               return subset_max_value(subset) >= n &&
                                      subset.size() <= target_length;
                           });

        m_subsets.erase(remove_iterator, m_subsets.end());
    }

    template <typename It> void feed(It begin, It end) {
        while (begin != end) {
            feed(*(begin++));
        }
    }

    void feed(const std::vector<int>& numbers) {
        feed(numbers.begin(), numbers.end());
    }

    const std::vector<int>& numbers() const { return m_numbers; }
    const std::vector<std::vector<int>>& subsets() const { return m_subsets; }
    std::vector<int> longest_increasing_subset() const {
        return m_subsets.empty() ? std::vector<int>() : m_subsets.back();
    }
};

namespace v1 {
std::vector<std::vector<int>>
build_lis_candidates(std::vector<int>::const_iterator begin,
                     std::vector<int>::const_iterator end) {
    return build_all_increasing_subsets(begin, end);
}

std::vector<std::vector<int>>
build_lis_candidates_from_vec(const std::vector<int>& numbers) {
    return build_all_increasing_subsets(numbers);
}
} // namespace v1

inline namespace v2 {
std::vector<std::vector<int>>
build_lis_candidates(const std::vector<int>& numbers) {
    return build_increasing_subsets_candidates(numbers.begin(), numbers.end());
}

std::vector<std::vector<int>>
build_lis_candidates_from_vec(const std::vector<int>& numbers) {
    return build_lis_candidates(numbers);
}
} // namespace v2

using CandidatesBuilderFunction =
    std::function<std::vector<std::vector<int>>(const std::vector<int>&)>;

std::vector<int>
longest_increasing_subset(const std::vector<int>& numbers,
                          CandidatesBuilderFunction build_candidates =
                              &v2::build_lis_candidates_from_vec) {
    if (numbers.empty()) return {};

    std::vector<std::vector<int>> increasing_subsets =
        build_candidates(numbers);

    auto it = std::max_element(
        increasing_subsets.begin(), increasing_subsets.end(),
        [](const std::vector<int>& lhs, const std::vector<int>& rhs) {
            return lhs.size() < rhs.size();
        });

    return *it;
}

int longest_increasing_subset_length(
    const std::vector<int>& numbers,
    CandidatesBuilderFunction build_candidates =
        &v2::build_lis_candidates_from_vec) {
    return longest_increasing_subset(numbers, build_candidates).size();
}

void print(const std::vector<int>& numbers) {
    std::cout << "[";

    for (size_t i(0); i < numbers.size(); ++i) {
        std::cout << numbers.at(i);
        if (i != numbers.size() - 1) std::cout << ", ";
    }

    std::cout << "]" << std::endl;
}

void print(const std::vector<std::vector<int>>& lists) {
    for (const auto& l : lists)
        print(l);
}

static const std::vector<int> sixty_four = {
    357, 412, 321, 441, 332, 255, 249, 154, 273, 277, 263, 548, 362,
    397, 403, 238, 325, 302, 337, 357, 285, 273, 268, 267, 257, 395,
    231, 282, 224, 215, 248, 316, 339, 262, 291, 256, 159, 346, 227,
    330, 365, 288, 172, 218, 402, 283, 289, 331, 212, 226, 215, 253,
    231, 186, 128, 213, 240, 289, 138, 95,  129, 94,  145, 302};

static const std::vector<int> three_sixty_five = {
    335, 414, 263, 360, 446, 185, 361, 262, 371, 336, 274, 379, 496, 415, 442,
    234, 254, 331, 257, 281, 221, 241, 315, 398, 292, 399, 346, 392, 194, 518,
    378, 252, 348, 348, 403, 162, 323, 287, 468, 378, 393, 344, 150, 263, 279,
    357, 401, 228, 350, 245, 326, 350, 271, 369, 410, 242, 312, 399, 236, 319,
    354, 293, 298, 274, 229, 459, 425, 377, 251, 332, 332, 164, 343, 288, 282,
    485, 380, 372, 387, 320, 414, 294, 284, 314, 479, 232, 249, 334, 288, 241,
    467, 237, 406, 271, 123, 259, 264, 305, 366, 226, 523, 441, 232, 370, 356,
    199, 385, 451, 351, 149, 327, 300, 379, 393, 289, 161, 276, 191, 265, 180,
    172, 301, 226, 296, 185, 232, 160, 287, 154, 161, 371, 182, 281, 278, 204,
    203, 282, 385, 241, 316, 365, 276, 204, 374, 353, 185, 311, 289, 399, 394,
    224, 283, 309, 95,  336, 341, 233, 348, 239, 181, 292, 472, 322, 216, 284,
    370, 118, 322, 321, 366, 210, 271, 400, 330, 429, 388, 384, 367, 173, 221,
    171, 280, 424, 291, 201, 328, 205, 364, 155, 238, 200, 285, 271, 182, 240,
    202, 174, 136, 372, 198, 284, 184, 392, 255, 186, 311, 240, 328, 150, 253,
    228, 165, 232, 169, 184, 208, 217, 138, 228, 257, 86,  203, 189, 233, 273,
    234, 417, 326, 256, 274, 136, 264, 191, 260, 310, 255, 143, 321, 192, 220,
    203, 268, 176, 116, 231, 161, 212, 161, 140, 313, 134, 108, 209, 225, 211,
    302, 204, 118, 153, 266, 119, 254, 169, 241, 293, 187, 234, 334, 308, 208,
    323, 306, 78,  115, 287, 311, 271, 269, 168, 274, 280, 276, 279, 243, 382,
    282, 339, 189, 217, 219, 161, 272, 187, 303, 28,  205, 168, 245, 208, 158,
    103, 260, 300, 201, 130, 294, 197, 243, 143, 360, 380, 186, 64,  156, 223,
    34,  238, 213, 151, 200, 231, 132, 204, 9,   107, 209, 187, 270, 139, 5,
    145, 158, 157, 181, 131, 151, 128, 133, 220, 153, 291, 334, 91,  114, 240,
    192, 76,  224, 131, 150, 132, 227, 263, 164, 227, 194, 136, 85,  171, 60,
    253, 198, 118, 133, 258};

int main(int argc, char** argv) {

    {
        std::cout
            << "Computing just the length the of the longest increasing subset"
            << std::endl;

        std::cout << compute_length_of_longest_increasing_subset({})
                  << std::endl;
        std::cout << compute_length_of_longest_increasing_subset({1})
                  << std::endl;
        std::cout << compute_length_of_longest_increasing_subset({1, 2})
                  << std::endl;
        std::cout << compute_length_of_longest_increasing_subset({2, 1})
                  << std::endl;
        // // Things start to get noticeably slow around twenty elements.
        std::cout << compute_length_of_longest_increasing_subset(
                         {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                          11, 12, 13, 14, 15, 16, 17, 18, 19, 20})
                  << std::endl;
    }

    {
        std::cout << "---\n\nComputing all increasing subsets" << std::endl;

        std::vector<std::vector<int>> lists{
            {1, 0, 2, 1, 3}, {1, 0, 2, 1, 3, 7}, {1, 0, 2, 1, 3, 7, 5}};

        for (const std::vector<int>& numbers : lists) {
            std::cout << "List is:";
            print(numbers);
            auto sublists = build_all_increasing_subsets(numbers);
            std::cout << "Increasing subsets are:" << std::endl;
            print(sublists);
            std::cout << std::endl;
        }
    }

    {
        std::cout
            << "---\n\nCompute limited list of candidates for the longest "
               "increasing subset"
            << std::endl;

        std::vector<std::vector<int>> lists{{1, 3, 7, 5},
                                            {1, 2, -1, 3, 0, 7, 5, 6},
                                            {1, 2, -1, 3, 0, 7, 5, 6, 4},
                                            {1,  2,  3,  4,  5,  6,  7,
                                             8,  9,  10, 11, 12, 13, 14,
                                             15, 16, 17, 18, 19, 20}};

        for (const std::vector<int>& numbers : lists) {
            std::cout << "List is:";
            print(numbers);
            auto sublists = v2::build_lis_candidates(numbers);
            std::cout << "Longest increasing subset candidates are:"
                      << std::endl;
            print(sublists);
            std::cout << std::endl;
        }
    }

    {
        std::cout << "---\n\nVerify that the various algorithms agree"
                  << std::endl;

        std::vector<std::vector<int>> lists{
            {1, 3, 7, 5},
            {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
            {9, 0, 8, 1, 7, 2, 6, 3, 5, 4, 5, 6}};

        for (const std::vector<int>& numbers : lists) {
            size_t l1 = compute_length_of_longest_increasing_subset(numbers);
            size_t l2 = longest_increasing_subset_length(
                numbers, &v1::build_lis_candidates_from_vec);
            size_t l3 = longest_increasing_subset_length(
                numbers, &v2::build_lis_candidates_from_vec);
            print(numbers);
            if (l1 == l2 && l2 == l3) {
                std::cout << "--> Ok: " << l1 << std::endl;
            } else {
                std::cout << "--> NOT ok :( " << std::endl;
            }
        }
    }

    {
        std::cout << "---\n\nCompute one such subset efficiently";

        std::vector<std::vector<int>> lists{
            {1, 3, 7, 5},
            {9, 0, 8, 1, 7, 2, 6, 3, 5, 4, 5, 6},
            sixty_four,
            three_sixty_five};

        for (const std::vector<int>& numbers : lists) {
            std::cout << "List is: ";
            print(numbers);
            auto lis = longest_increasing_subset(
                numbers, &v2::build_lis_candidates_from_vec);
            std::cout << "Longest increasing subset (length=" << lis.size()
                      << ") is: ";
            print(lis);
            std::cout << std::endl;
        }
    }

    {
        std::cout << "---\n\nCompute increasing subsets iteratively";

        IncreasingSubsetExtractor builder;

        std::vector<int> numbers{1, 3, 0, 7, 2, 5, 6};

        for (int n : numbers) {
            std::cout << "Feeding " << n << std::endl;
            builder.feed(n);
            std::cout << "List is: ";
            print(builder.numbers());
            std::cout << "Subsets are:\n";
            print(builder.subsets());
            std::cout << std::endl;
        }
    }
}
