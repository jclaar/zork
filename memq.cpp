export module ZMemq;
import std;

// Simulates MEMQ returns. If a value is returned, it is an
// iterator to the item in the container. Otherwise there is no value
// returned.
export template <typename Container>
using MemqRet = std::optional<typename Container::const_iterator>;

export template <typename T, typename Container>
MemqRet<Container> memq(const T& i, const Container& c)
{
    typename Container::const_iterator iter = std::find(std::begin(c), std::end(c), i);
    return iter == std::end(c) ? std::nullopt : MemqRet<Container>(iter);
}


