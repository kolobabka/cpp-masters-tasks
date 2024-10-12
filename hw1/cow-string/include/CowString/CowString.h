#ifndef _COW_STRING__H__
#define _COW_STRING__H__

#include <iostream>

namespace bicycle {

template <typename T> void destroy(T *Ptr) {
    Ptr->~T();
}

template <typename FwdIter> void destroy(FwdIter Begin, FwdIter End) {
    while (Begin != End)
        destroy(&*Begin++);
}

template<typename CharT,
         typename Traits = std::char_traits<CharT>>
class StringBuffer {
protected:
    static constexpr auto DefaultCap = 32ull; 

    CharT *Data = nullptr;
    size_t Size = 0;
    size_t Capacity = DefaultCap;
    int Refcount = 0;
protected:
    StringBuffer(size_t Sz) 
        : Data((Sz == 0) ? nullptr 
        : static_cast<CharT *>(::operator new(sizeof(CharT) * Sz * 2))),
        Size(0), Capacity(Sz * 2), Refcount(1) {} // Refcount 1?

    StringBuffer(const StringBuffer &Rhs) = delete;
    StringBuffer &operator=(const StringBuffer &) = delete;
    
    StringBuffer(StringBuffer &&Rhs) noexcept 
        : Data(Rhs.Data), Size(Rhs.Size), 
          Capacity(Rhs.Size), Refcount(Rhs.Refcount) {
        Rhs.Data = nullptr;
        Rhs.Size = 0;
        Rhs.Capacity = 0;
        Rhs.Refcount = 1;
    } 

    StringBuffer &operator=(StringBuffer &&Rhs) noexcept {
        if (&Rhs == this)
            return *this;
        std::swap(Data, Rhs.Data);
        std::swap(Size, Rhs.Size);
        std::swap(Capacity, Rhs.Capacity);
        std::swap(Refcount, Rhs.Refcount);

        return *this;
    }

    ~StringBuffer() {
        --Refcount;
        if (!Refcount) {
            destroy(Data, Data + Capacity);
            ::operator delete(Data);
        }
    }
};

template<typename CharT = char,
         typename Traits = std::char_traits<CharT>>
class CowString final : private StringBuffer<CharT, Traits> {
    using StringBuffer<CharT, Traits>::Data;
    using StringBuffer<CharT, Traits>::Size;
    using StringBuffer<CharT, Traits>::Capacity;
    using StringBuffer<CharT, Traits>::Refcount;
public:
    CowString(size_t Sz) : StringBuffer<CharT, Traits>(Sz) {}
};
} // namespace bicycle

#endif // _COW_STRING__H__
