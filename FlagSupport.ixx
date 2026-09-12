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

	Flags<FlagType, sz>& operator&(const Flags<FlagType, sz>& rhs)
	{
		Base::operator&=(static_cast<const Base&>(rhs));
		return *this;
	}

private:
};

