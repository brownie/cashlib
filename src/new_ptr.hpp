/*
 * new_ptr: allows you to do:
 *     new_ptr<T>(args)
 * where you would usually do:
 *     shared_ptr<T>(new T(args))
 *
 * from http://www.boostcookbook.com/Recipe:/1234950
 */

#ifndef __NEW_PTR_HPP__
#define __NEW_PTR_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>

#ifndef NEW_PTR_MAX_PARAMS
    #define NEW_PTR_MAX_PARAMS 16
#endif // NEW_PTR_MAX_PARAMS

#define NEW_PTR_PARAM_DECL(z, n, _) BOOST_PP_COMMA_IF(n) A ##  n const & a ## n

class new_ptr_access {
    private:
        template <class T>
            static void destroy(T* pT) {
                typedef char typeMustBeCompleteType[sizeof(T) ? 1 : -1];
                (void)sizeof(typeMustBeCompleteType);
                delete pT;
            }
   
        #define NEW_PTR_ACCESS_CREATE_FUNCTION(z, n, _) \
            template< class T BOOST_PP_ENUM_TRAILING_PARAMS(n, class A) > \
                static boost::shared_ptr<T> create(BOOST_PP_REPEAT(n, NEW_PTR_PARAM_DECL, _)) { \
                    return boost::shared_ptr<T>(new T(BOOST_PP_ENUM_PARAMS(n, a)), destroy<T>); \
                } \
   
   
        BOOST_PP_REPEAT(BOOST_PP_INC(NEW_PTR_MAX_PARAMS), NEW_PTR_ACCESS_CREATE_FUNCTION, _)

        #undef NEW_PTR_ACCESS_CREATE_FUNCTION

        #define NEW_PTR_ACCESS_FRIEND_DECL(z, n, _) \
            template<class T BOOST_PP_ENUM_TRAILING_PARAMS(n, class A)> \
                friend boost::shared_ptr<T> new_ptr(BOOST_PP_REPEAT(n, NEW_PTR_PARAM_DECL, _)); \


        BOOST_PP_REPEAT(BOOST_PP_INC(NEW_PTR_MAX_PARAMS), NEW_PTR_ACCESS_FRIEND_DECL, _)

        #undef NEW_PTR_ACCESS_FRIEND_DECL

    private:
        new_ptr_access();
};

#define NEW_PTR_FUNCTION_DECL(z, n, _) \
    template< class T BOOST_PP_ENUM_TRAILING_PARAMS(n, class A)> \
        boost::shared_ptr<T> new_ptr(BOOST_PP_REPEAT(n, NEW_PTR_PARAM_DECL, _)) { \
            return new_ptr_access::create<T>(BOOST_PP_ENUM_PARAMS(n, a)); \
        } \


BOOST_PP_REPEAT(BOOST_PP_INC(NEW_PTR_MAX_PARAMS), NEW_PTR_FUNCTION_DECL, _)
#undef NEW_PTR_FUNCTION_DECL
#undef NEW_PTR_PARAM_DECL
#undef NEW_PTR_DECL

/*
// Example:
class foo {
    private:
        foo()
            : name_("My Name")
        { }

        foo(const std::string name)
            : name_(name)
        { }

        foo(int i, int j)
            : name_("I & J")
        { }

        ~foo() { }

        friend class new_ptr_access;

    public:
        const std::string& name() const { return name_; }

    private:
        std::string name_;
};

shared_ptr<foo> bar1 = new_ptr<foo>();
shared_ptr<foo> bar2 = new_ptr<foo>("example");
shared_ptr<foo> bar3 = new_ptr<foo>(1, 2);

delete bar1.get(); // won't compile

foo stackInstance("example"); // won't compile
foo* heapInstance = new foo("example") // won't compile

*/
#endif
