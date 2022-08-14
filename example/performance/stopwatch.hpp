
#include <sstream>
#include <chrono>

template <typename TClock = std::chrono::steady_clock>
class Stopwatch
{
    typename TClock::time_point _last;

public:
    Stopwatch()
        : _last(TClock::now())
    {}

    void reset()
    {
        _last = TClock::now();
    }

    typename TClock::duration elapsed() const
    {
        return TClock::now() - _last;
    }

    std::string elapsed_str() const
    {
        std::stringstream ss;
        ss << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count() << "ms";
        return ss.str();
    }
};
