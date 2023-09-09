
#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <iostream>

namespace v0
{

template<typename T, typename F, typename R = decltype(std::declval<F>()(std::declval<const T&>()))>
std::vector<R> map(const std::vector<T>& vec, F&& fun)
{
  std::vector<R> res;
  res.reserve(vec.size());
  std::transform(vec.begin(), vec.end(), std::back_inserter(res),
    [&fun](const T& val) { return fun(val); });
  return res;
}

template<typename T>
std::vector<T*> rawpointers(const std::vector<std::unique_ptr<T>>& ptrs)
{
  std::vector<Lane*> res;
  res.reserve(ptrs.size());
  std::transform(ptrs.begin(), ptrs.end(), std::back_inserter(res),
    [](const std::unique_ptr<T>& ptr) -> T* { return ptr.get(); });
  return res;
}

} // namespace v0

inline namespace v1
{

/**
 * \brief a map() function for C++
 * \tparam T  input element type
 * \tparam F  functor-like type
 * \tparam R  output element type
 * \param vec  input vector of elements
 * \param fun  function to apply to each element
 * 
 * Transforms a vector of elements by applying a function to each element;
 * similar to Javascript Array.map() function.
 * Returns the vector of transformed elements.
 * 
 * \note the template parameters are deduced by template argument deduction
 */
template<typename T, typename F, typename R = typename std::invoke_result<F, T>::type>
std::vector<R> map(const std::vector<T>& vec, F&& fun)
{
  std::vector<R> res;
  res.reserve(vec.size());
  std::transform(vec.begin(), vec.end(), std::back_inserter(res),
    [&fun](const T& val) { return fun(val); });
  return res;
}

/**
 * \brief converts a vector of shared pointer to raw pointers
 * \tparam T  type of the element that is pointed to
 * \param ptrs  a vector of unique pointers
 * \return a vector of raw pointers
 * 
 * \note template parameters are deduced by template argument deduction
 */
template<typename T>
std::vector<T*> rawpointers(const std::vector<std::unique_ptr<T>>& ptrs)
{
  return map(ptrs, [](const std::unique_ptr<T>& ptr) -> T* { return ptr.get(); });
}

} // namespace v1

struct Lane { };

class Road
{
public:

  const std::vector<std::unique_ptr<Lane>>& lanes() const
  {
    return m_lanes;
  }

  std::vector<Lane*> getLanes() const
  {
    return rawpointers(m_lanes);
  }

private:
  std::vector<std::unique_ptr<Lane>> m_lanes;
};

int main()
{
  std::cout << "Hello World!" << std::endl;

  {
    Road road;
    //auto lanes = road.lanes(); // Won't compile, cannot copy!
    auto& lanesref = road.lanes(); // Warning: actually a const reference 🤯
    std::vector<Lane*> a = v0::rawpointers(road.lanes());
    std::vector<Lane*> b = road.getLanes();
  }

  std::vector<int> integers{1, 2, 3, 4, 5};

  // Transform the integers into their square.
  auto squares = map(integers, [](int n) { return n * n; });

  for(int i : squares)
  {
    std::cout << i << " ";
  }

  std::cout << std::endl;

  // Convert to string
  std::vector<std::string> strs = map(squares, [](int n) { return "'" + std::to_string(n) + "'"; });

  for(const std::string& s : strs)
  {
    std::cout << s << " ";
  }

  std::cout << std::endl;

  return 0;
}
