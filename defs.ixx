module;

export module ZDefs;
import std;
import ZFlagSupport;
#include "defs.h"

export enum class direction
{
    NullExit,
    North,
    South,
    East,
    West,
    Launc,
    Land,
    Se,
    Sw,
    Ne,
    Nw,
    Up,
    Down,
    Enter,
    Exit,
    Leave,
    Out,
    Cross,
    NumDirs
};

export std::unique_ptr<std::ofstream> script_channel;

// Output stream, supporting scripting.
class TtyBuff : public std::basic_stringbuf<char, std::char_traits<char>>
{
public:
    TtyBuff() : gen(std::random_device{}()), dist(50, 200) {}

    bool IsTerminal() const { return term_sim; }
    void SetTerminal(bool on) { term_sim = on; }

protected:
    int sync() override
    {
        using namespace std::chrono_literals;
        // Delay between each character in terminal mode.
        auto delay = term_sim ? 10ms : 0ms;
        char c;
        if (term_sim)
        {
            // If in terminal mode, pause for a little bit to simulate memory/disk access, etc.
            std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
        }
        while ((c = this->sbumpc()) != (char)traits_type::eof())
        {
            std::cout << c;
            if (delay != 0ms)
            {
                std::cout.flush();
                std::this_thread::sleep_for(delay);
            }
            // If scripting, write this character to the script channel as well.
            if (script_channel)
            {
                (*script_channel) << c;
            }
        }
        if (script_channel)
            script_channel->flush();
        return 0;
    }

private:
    std::mt19937 gen;                               // random engine
    std::uniform_int_distribution<int> dist;        // distribution
    bool term_sim = false;
};

export TtyBuff tty_buf;
export std::ostream tty(&tty_buf);

// Hacky method of allowing an additional argument to be passed to 
// apply_random. This is only used in a couple of cases.
export enum class ApplyRandomArg
{
    read_out,
    read_in
};
// Defines a functor for an rapplic. The ApplyRandomArg argument is optional.
export using Rarg = std::optional<ApplyRandomArg>;
export using rapplic = std::function<bool(Rarg)>;


// For variant stuff.
export template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
export template<class... Ts> overload(Ts...) -> overload<Ts...>;

export std::string operator+(std::string_view s1, std::string_view s2)
{
    std::string ss1(s1);
    ss1 += s2;
    return ss1;
}

export enum class e_oactor
{
    player,
    master,
    robot,
    none
};

export enum class Bits
{
    ovison,
    readbit,
    takebit,
    doorbit,
    transbit,
    foodbit,
    ndescbit,
    drinkbit,
    contbit,
    lightbit,
    vicbit,
    burnbit,
    flamebit,
    toolbit,
    turnbit,
    vehbit,
    findmebit,
    sleepbit,
    searchbit,
    sacredbit,
    tiebit,
    climbbit,
    actorbit,
    weaponbit,
    fightbit,
    villain,
    staggered,
    trytakebit,
    no_check_bit,
    openbit,
    touchbit,
    onbit,
    bunchbit,
    oglobal,
    digbit,
    lastrealbit,
    // Special bits. Anything higher than lastrealbit refers to a GOBJECT
    housebit,
    rgwater,
    treebit,
    dwindow,
    birdbit,
    wellbit,
    wall_eswbit,
    wall_nbit,
    cpladder,
    cpwall,
    slidebit,
    ropebit,
    rosebit,
    chanbit,
    guardbit,
    mirrorbit,
    panelbit,
    masterbit,
    numbits
};
export constexpr size_t numbits = std::to_underlying(Bits::numbits);

export enum class RoomBit
{
    rseenbit,    // "visited?"
    rlightbit,   // "endogenous light source?"
    rlandbit,    // "on land"
    rwaterbit,   // "water room"
    rairbit,     // "mid-air room"
    rsacredbit,  // "thief not allowed"
    rfillbit,    // "can fill bottle here"
    rmungbit,    // "room has been munged"
    rbuckbit,    // "this room is a bucket"
    rhousebit,   // "This room is part of the house"
    rendgame,    // "This room is in the end game"
    rnwallbit,   // "This room doesn't have walls"
    rnumbits
};
export constexpr size_t rnumbits = std::to_underlying(RoomBit::rnumbits);
export using RoomBits = Flags<RoomBit, rnumbits>;

export using BitsList = std::list<Bits>;

export template <typename T0, typename... Ts>
bool is_empty(const std::variant<T0, Ts...>& v)
{
    return std::holds_alternative<std::monostate>(v);
}

export int no_tell = 0;
export int eg_score = 0;

export void prin1(int val)
{
    tty << val;
}

export bool apply_random(const rapplic& fcn)
{
    return fcn(Rarg());
}

export bool apply_random(rapplic fcn, ApplyRandomArg arg)
{
    return fcn(arg);
}
