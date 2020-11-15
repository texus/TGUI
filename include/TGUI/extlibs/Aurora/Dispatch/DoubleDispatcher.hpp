/////////////////////////////////////////////////////////////////////////////////
//
// Aurora C++ Library
// Copyright (c) 2012-2016 Jan Haller
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////

/// @file
/// @brief Class template aurora::DoubleDispatcher

#ifndef AURORA_DOUBLEDISPATCHER_HPP
#define AURORA_DOUBLEDISPATCHER_HPP

#include <TGUI/extlibs/Aurora/Dispatch/DispatchTraits.hpp>
#include <TGUI/extlibs/Aurora/Tools/NonCopyable.hpp>
#include <TGUI/extlibs/Aurora/Tools/Exceptions.hpp>
#include <TGUI/extlibs/Aurora/Tools/Hash.hpp>
#include <TGUI/extlibs/Aurora/Meta/Templates.hpp>
#include <TGUI/extlibs/Aurora/Config.hpp>

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cassert>


namespace aurora
{

/// @addtogroup Dispatch
/// @{

/// @brief Class that is able to perform dynamic dispatch on multiple functions with two parameters.
/// @details Sometimes you encounter the situation where you need to implement polymorphic behavior to dispatch
///  dynamically on more than one type. Like overloading functions with two parameters at compile time, this class
///  allows you to perform a dispatch on two arguments at runtime. At invocation time, all you need is the static
///  type of the base class, the %DoubleDispatcher figures out which dynamic types match which function.
/// @tparam Signature Function signature <b>R(B, B)</b>or <b>R(B, B, U)</b>, with the following types:
///  * <b>B</b>: Reference or pointer to polymorphic base class. This is the base class of every dispatched function's
///  parameter type. When it is a pointer, the arguments of the dispatched functions shall be pointers too (the
///  same applies to references).
///  the dispatched functions shall have arguments of type pointer or reference to const, too.
///  * <b>R</b>: Return type of the dispatched functions.
///  * <b>U</b>: Any parameter type that can be used to forward user arguments to the functions
/// @tparam Traits Traits class to customize the usage of the dispatcher. To define your own traits, you can (but don't have to)
///  inherit the class @ref aurora::DispatchTraits<K>, where K is your key. It predefines most members for convenience.
///  In general, the @c Traits class must contain the following members:
/// @code
/// struct Traits
/// {
///	    // The type that is used to differentiate objects. For RTTI class hierarchies, std::type_index is a good choice
///	    // -- but you're free to choose anything, such as an enum or a string. The requirements are that Key can be used
///	    // as a key in std::unordered_map, i.e. it must support a std::hash<Key> specialization and operator==.
///	    typedef K Key;
///
///	    // A function that returns the corresponding key (such as std::type_index) from a type identifier (such as aurora::Type<T>).
///	    // The type identifier is passed to bind() and can contain static type information, while the key is used by the map
///	    // storing the registered functions. Often, key and type identifier are the same.
///	    static Key keyFromId(Id id);
///
///	    // Given a function argument base, this static function extracts the key from it. B corresponds to the template parameter
///	    // specified at SingleDispatcher, that is, it is a reference or pointer.
///	    static Key keyFromBase(B base);
///
///	    // trampoline2() takes a function that is passed to DoubleDispatcher::bind() and modifies it in order to fit the common
///	    // R(B, B) signature. It therefore acts as a wrapper for user-defined functions which can link different signatures together.
///	    // For example, this is the place to insert downcasts.
///	    // The first two template parameters Id1 and Id2 are required, as they will be explicitly specified when trampoline2() is called.
///	    template <typename Id1, typename Id2, typename Fn>
///	    static std::function<R(B, B)> trampoline2(Fn f);
///
///	    // Optional function that returns a string representation of key for debugging.
///	    static const char* name(Key k);
/// };
/// @endcode
///
/// Usage example:
/// @code
/// // Example class hierarchy
/// class Base { public: virtual ~Base() {} };
/// class Derived1 : public Base {};
/// class Derived2 : public Base {};
///
/// // Free functions for the derived types
/// void func11(Derived1* lhs, Derived1* rhs);
/// void func12(Derived1* lhs, Derived2* rhs);
/// void func22(Derived2* lhs, Derived2* rhs);
///
/// // Create dispatcher and register functions
/// aurora::DoubleDispatcher<void(Base*,Base*)> dispatcher;
/// dispatcher.bind(aurora::Type<Derived1>(), aurora::Type<Derived1>(), &func11);
/// dispatcher.bind(aurora::Type<Derived1>(), aurora::Type<Derived2>(), &func12);
/// dispatcher.bind(aurora::Type<Derived2>(), aurora::Type<Derived2>(), &func22);
///
/// // Invoke functions on base class pointer
/// Base* ptr = new Derived1;
/// dispatcher.call(ptr, ptr); // Invokes void func11(Derived1* lhs, Derived1* rhs);
/// delete ptr;
/// @endcode
template <typename Signature, class Traits = RttiDispatchTraits<Signature, 2>>
class DoubleDispatcher : private NonCopyable
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public types
	public:
		/// @brief Function return type
		///
		typedef typename FunctionResult<Signature>::Type		Result;

		/// @brief Function parameter type denoting the object used for the dispatch
		///
		typedef typename FunctionParam<Signature, 0>::Type		Parameter;

		/// @brief Addition parameter for user data, only useful if @c Signature contains more than 2 parameters
		///
		typedef typename FunctionParam<Signature, 2>::Type		UserData;


	// ---------------------------------------------------------------------------------------------------------------------------
	// Static assertions

	// Make sure that B is either T* or T&
	static_assert(std::is_pointer<Parameter>::value || std::is_lvalue_reference<Parameter>::value,
		"Function parameter must be a pointer or reference.");

	// For signature R(X, Y), ensure that X == Y
	static_assert(std::is_same<typename FunctionParam<Signature, 0>::Type, typename FunctionParam<Signature, 1>::Type>::value,
		"The two function parameters must have the same type.");


	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Constructor
		/// @param symmetric Is true if the calls <b>fn(a,b)</b> and <b>fn(b,a)</b> are equivalent and it's enough
		///  to register one of both variants. Otherwise, both calls have to be registered separately and are resolved
		///  to different functions.
		explicit					DoubleDispatcher(bool symmetric = true);

		/// @brief Move constructor
									DoubleDispatcher(DoubleDispatcher&& source);

		/// @brief Move assignment operator
		DoubleDispatcher&			operator= (DoubleDispatcher&& source);

		/// @brief Destructor
									~DoubleDispatcher();

		/// @brief Registers a function bound to a specific key.
		/// @tparam Id1,Id2 %Types that identify the argument types. By default, these are aurora::Type<D>, where D is a
		///  derived class. Can be deduced from the argument.
		/// @tparam Fn %Type of the function. Can be deduced from the argument.
		/// @param identifier1,identifier2 Values that identify the object. The key, which is mapped to the function, is computed
		///  from each identifier through Traits::keyFromId(identifier).
		/// @param function Function to register and associate with the given identifier. Usually, the function has the signature
		///  <tt>Result(Parameter, Parameter)</tt>, but it's possible to deviate from it (e.g. using derived classes), see also the
		///  note about trampolines in the Traits classes. In case you specified a third parameter for the @c Signature template
		///  parameter, the function should have the signature <tt>Result(Parameter, Parameter, UserData)</tt>.
		template <typename Id1, typename Id2, typename Fn>
		void						bind(const Id1& identifier1, const Id2& identifier2, Fn function);

		/// @brief Dispatches the key of @c arg1 and @c arg2 and invokes the corresponding function.
		/// @details <tt>Traits::keyFromBase(arg)</tt> is invoked to determine the key of each passed argument. The function bound to
		///  the combination of both keys is then looked up in the map and invoked. If no match is found and a fallback function has
		///  been registered using fallback(), then the fallback function will be invoked.
		///  @n@n When the dispatcher is configured in symmetric mode (see constructor), then the arguments are forwarded to the
		///  correct parameters in the registered functions, even if the order is different. When necessary, they are swapped.
		///  In other words, symmetric dispatchers don't care about the order of the arguments at all.
		/// @param arg1,arg2 Function arguments as references or pointers.
		/// @return The return value of the dispatched function, if any.
		/// @throw FunctionCallException when no corresponding function is found and no fallback has been registered.
		Result						call(Parameter arg1, Parameter arg2) const;

		/// @brief Invokes the function depending on @c arg1 and @c arg2 and passes a user-defined argument @c data
		/// @details <tt>Traits::keyFromBase(arg)</tt> is invoked to determine the key of the passed argument. The function bound to
		///  that key is then looked up in the map and invoked. If no match is found and a fallback function has been registered
		///  using fallback(), then the fallback function will be invoked.
		///  @n@n When the dispatcher is configured in symmetric mode (see constructor), then the arguments are forwarded to the
		///  correct parameters in the registered functions, even if the order is different. When necessary, they are swapped.
		///  In other words, symmetric dispatchers don't care about the order of the first two arguments.
		///  @n@n This method is only enabled if the @c Signature template parameter contains 3 parameters.
		/// @param arg1,arg2 Function arguments as references or pointers.
		/// @param data An additional user argument that is forwarded to the function.
		/// @return The return value of the dispatched function, if any.
		/// @throw FunctionCallException when no corresponding function is found and no fallback has been registered.
		Result						call(Parameter arg1, Parameter arg2, UserData data) const;

		/// @brief Registers a fallback function.
		/// @details The passed function will be invoked when call() doesn't find a registered function. It can be used when
		///  not finding a match does not represent an exceptional situation, but a common case.
		/// @n@n If you want to perform no action, you can pass aurora::NoOp<R, 2>().
		/// @param function Function according to the specified signature.
		void						fallback(std::function<Signature> function);


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private types
	private:
		typedef typename Traits::Key					SingleKey;
		typedef std::function<Signature>				BaseFunction;

		struct Key
		{
											Key(const SingleKey& key1, const SingleKey& key2, bool swapped);
			bool							operator== (const Key& rhs) const;

			std::pair<SingleKey, SingleKey>	keyPair;
			bool							swapped;
		};

		struct Hasher
		{
			std::size_t operator() (const Key& k) const;
		};

		typedef std::unordered_map<Key, BaseFunction, Hasher>		FnMap;


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private member functions
	private:
		// Makes sure that the keys are sorted in case we use symmetric argument dispatching.
		Key							makeKey(SingleKey key1, SingleKey key2) const;


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private variables
	private:
		FnMap						mMap;
		BaseFunction				mFallback;
		bool						mSymmetric;
};

/// @}

} // namespace aurora

#include <TGUI/Aurora/Dispatch/Detail/DoubleDispatcher.inl>
#endif // AURORA_DOUBLEDISPATCHER_HPP
