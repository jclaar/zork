#pragma once
#include <boost/serialization/serialization.hpp>

template <typename FlagType, size_t sz>
class Flags : private std::bitset<sz>
{
    typedef std::bitset<sz> Base;
    Flags(const Base& b) : Base(b)
    {
    }
public:
    using Base::none;
    using Base::any;
    using Base::reference;

    Flags() {}

    bool operator[](FlagType flag) const { return Base::operator[](static_cast<int>(flag)); }
    Base::reference operator[](FlagType flag) { return Base::operator[](static_cast<int>(flag)); }
    bool test(FlagType flag) const { return Base::test(static_cast<int>(flag)); }
    Flags<FlagType, sz>& set() { Base::set(); return *this; }

    Flags& flip(FlagType flag)
    {
        Base::flip(static_cast<int>(flag));
        return *this;
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive& ar, const unsigned int version)
    {
        ar & static_cast<Base&>(*this);
    }

private:

    template <typename FT, size_t Sz>
    friend Flags<FT, Sz> operator&(const Flags<FT, Sz>& lhs, const Flags<FT, Sz>& rhs);
};

template <typename FlagType, size_t sz>
Flags<FlagType, sz> operator&(const Flags<FlagType, sz>& lhs, const Flags<FlagType, sz>& rhs)
{
    auto& fl = static_cast<const Flags<FlagType, sz>::Base&>(lhs);
    auto& fr = static_cast<const Flags<FlagType, sz>::Base&>(rhs);
    Flags<FlagType, sz> rv(fl & fr);
    
    return rv;
}
