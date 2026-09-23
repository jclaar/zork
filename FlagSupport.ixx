module;
#include <boost/serialization/serialization.hpp>

export module ZFlagSupport;
import std;

export template <typename FlagType, size_t sz>
class Flags : private std::bitset<sz>
{
    typedef std::bitset<sz> Base;
    Flags(const Base& b) : Base(b)
    {}
public:
    using Base::none;
    using Base::any;
    using typename Base::reference;

    Flags() {}

    bool operator[](FlagType flag) const { return Base::operator[](std::to_underlying(flag)); }
    Base::reference operator[](FlagType flag) { return Base::operator[](std::to_underlying(flag)); }
    bool test(FlagType flag) const { return Base::test(std::to_underlying(flag)); }
    Flags<FlagType, sz>& set() { Base::set(); return *this; }

    Flags& flip(FlagType flag)
    {
        Base::flip(std::to_underlying(flag));
        return *this;
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive& ar, const unsigned int version)
    {
        ar& static_cast<Base&>(*this);
    }

    template <typename FT, size_t siz>
    friend Flags<FT, siz> operator&(const Flags<FT, siz>& lhs, const Flags<FT, siz>& rhs);

private:
};

export template <typename FlagType, size_t sz>
Flags<FlagType, sz> operator&(const Flags<FlagType, sz>& lhs, const Flags<FlagType, sz>& rhs)
{
    auto& fl = static_cast<const Flags<FlagType, sz>::Base&>(lhs);
    auto& fr = static_cast<const Flags<FlagType, sz>::Base&>(rhs);
    Flags<FlagType, sz> rv(fl & fr);

    return rv;
}
