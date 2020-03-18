#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <memory>
#include <random>
#include "ungod/base/Logger.h"

namespace ungod
{
    constexpr float PI = 3.14159265359f;

    template<typename DERIV, typename BASE>
    std::unique_ptr<DERIV>
    inline static_unique_ptr_cast( std::unique_ptr<BASE>&& ptr )
    {
        auto d = static_cast<DERIV *>(ptr.release());
        return std::unique_ptr<DERIV>(d);
    }

    template<typename DERIV, typename BASE>
    std::weak_ptr<DERIV>
    inline static_weak_ptr_cast( std::weak_ptr<BASE>& ptr )
    {
        return std::static_pointer_cast<DERIV>( ptr.lock() );
    }

    inline std::vector<std::string> convertToVector(const std::string& str)
    {
        std::stringstream ss(str);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> result(begin, end);
        return result;
    }

    template <typename T> struct AlwaysFalse : std::false_type {};

    struct NumberGenerator
    {
    private:
        static std::mt19937 gen;

    public:
        static int getRandBetw(const int a, const int b);
        static float getFloatRandBetw(const float a, const float b);
        static float getNormRand(const float mu, const float rho);
        static bool getRandBool();
        static bool getRandBool(const float true_prob);
    };



    /** \brief Utility struct to allow usage of make_shared with protected constructors. */
    template<typename T>
    struct MakeSharedEnabler : public T
    {
        template <typename... ARGS>
        MakeSharedEnabler(ARGS &&... args) : T(std::forward<ARGS>(args)...) {}
    };

    /** \brief Make_shared callable for classes with protected constructors. */
    template<typename T, typename ... ARGS>
    std::shared_ptr<T> protected_make_shared(ARGS &&... args)
    {
        return std::make_shared< MakeSharedEnabler<T> >(std::forward<ARGS>(args)...);
    }


    /** \brief A macro for polymorphic cloning. */
    #define CLONE_BASE_UP(B) \
        virtual std::unique_ptr<B> clone() const = 0;

    /** \brief A macro for polymorphic cloning. */
    #define CLONE_DERIVED_UP(D,B) \
        virtual std::unique_ptr<B> clone() const override { \
        return static_unique_ptr_cast<B>(std::make_unique<D>(*this)); }

    ///compile time convert number to string
     #define NUM2STR(x) #x

    ///compile string concatenation
     #define STRCAT(x,y) x y


    //returns a vector of all filenames in a given directory
    std::vector<std::string> enumerateDirectory(const std::string& pathToDir);
}

#endif // UTILITY_H
