#pragma once
#include <boost/serialization/serialization.hpp>

template <typename FlagType, size_t sz>
class Flags
{
public:
    typedef std::bitset<sz> FlagBits;

    bool operator[](FlagType flag) const { return f[static_cast<int>(flag)]; }
    typename FlagBits::reference operator[](FlagType flag) { return f[static_cast<int>(flag)]; }
    bool test(FlagType flag) const { return f.test(static_cast<int>(flag)); }
    bool none() const { return f.none(); }
    bool any() const { return f.any(); }
    Flags<FlagType, sz>& set() { f.set(); return *this; }

    Flags& flip(FlagType flag)
    {
        f.flip(static_cast<int>(flag));
        return *this;
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive& ar, const unsigned int version)
    {
        ar& f;
    }

private:
    FlagBits f;

    template <typename FT, size_t Sz>
    friend Flags<FT, Sz> operator&(const Flags<FT, Sz>& lhs, const Flags<FT, Sz>& rhs);
};

template <typename FlagType, size_t sz>
Flags<FlagType, sz> operator&(const Flags<FlagType, sz>& lhs, const Flags<FlagType, sz>& rhs)
{
    Flags<FlagType, sz> rv;
    rv.f = lhs.f & rhs.f;
    return rv;
}
