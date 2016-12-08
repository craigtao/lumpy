#pragma once

#include <lumpy/core.h>

namespace lumpy
{

template<class _Type, class _Size=uint, _Size TrunkSize=4*1024/sizeof(_Type)>
class ArrayList: INocopyable
{
public:
    using Type  = _Type;
    using Size  = _Size;
    using Index = _Size;

    ArrayList()  = default;
    ~ArrayList() = default;

public:
    constexpr static Size trunk_size()        noexcept { return TrunkSize;              }
    constexpr        Size trunk_count() const noexcept { return Size(trunks_.size());   }
    constexpr        Size size()        const noexcept { return trunks_.empty() ?  0 : Size(trunks_.size()*trunk_size() - (end_ - next_));  }

    Size size(Index i) const noexcept {
        if (i >= trunk_size())    return 0;
        if (i == trunk_size()-1)  return trunk_size()-(end_-next_);
        return trunk_size();
    }

    Type*       data(Index i)       noexcept { return trunks_[i]; }
    const Type* data(Index i) const noexcept { return trunks_[i]; }


    void push_back(const Type& value) {
        if (next_ != end_) {
            *next_++ = value;
        }
        else {
            next_ = static_cast<Type*>(::malloc(sizeof(Type) * trunk_size()));
            end_  = next_ + trunk_size();
            trunks_.push_back(next_);
            push_back(value);
        }
    }

protected:
    std::vector<Type*>  trunks_;           // blocks
    Type*               next_   = nullptr; // current block
    Type*               end_    = nullptr; // current end
};

}
