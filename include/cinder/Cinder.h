/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <boost/cstdint.hpp>
#include <boost/version.hpp>
#include <boost/chrono.hpp>
#include <boost/config.hpp>

#if BOOST_VERSION < 104800
	#error "Cinder requires Boost version 1.48 or later"
#endif

namespace cinder {
using boost::int8_t;
using boost::uint8_t;
using boost::int16_t;
using boost::uint16_t;
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;

#define CINDER_CINDER

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	#define CINDER_MSW
#elif defined(linux) || defined(__linux) || defined(__linux__)
	#define CINDER_LINUX
	#if defined(ANDROID)
		#define CINDER_ANDROID
	#endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
	#define CINDER_COCOA
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE
		#define CINDER_COCOA_TOUCH
	#else
		#define CINDER_MAC
	#endif
	// This is defined to prevent the inclusion of some unfortunate macros in <AssertMacros.h>
	#define __ASSERTMACROS__
#else
	#error "cinder compile error: Unknown platform"
#endif

#define CINDER_LITTLE_ENDIAN

} // namespace cinder


#if defined( _MSC_VER ) && ( _MSC_VER >= 1600 ) || defined( _LIBCPP_VERSION )
	#include <memory>
#elif defined( CINDER_ANDROID )
    #include <memory>
#elif defined( CINDER_COCOA )
	#include <tr1/memory>
	namespace std {
		using std::tr1::shared_ptr;
		using std::tr1::weak_ptr;		
		using std::tr1::static_pointer_cast;
		using std::tr1::dynamic_pointer_cast;
		using std::tr1::const_pointer_cast;
		using std::tr1::enable_shared_from_this;
	}
#else
	#include <boost/shared_ptr.hpp>
	#include <boost/enable_shared_from_this.hpp>
	namespace std {
		using boost::shared_ptr; // future-proof shared_ptr by putting it into std::
		using boost::weak_ptr;
		using boost::static_pointer_cast;
		using boost::dynamic_pointer_cast;
		using boost::const_pointer_cast;
		using boost::enable_shared_from_this;		
	}
#endif

#include <boost/checked_delete.hpp> // necessary for checked_array_deleter
using boost::checked_array_deleter;

// if compiler supports r-value references, #define CINDER_RVALUE_REFERENCES
#if defined( _MSC_VER ) && ( _MSC_VER >= 1600 )
	#define CINDER_RVALUE_REFERENCES
#elif defined( __clang__ )
	#if __has_feature(cxx_rvalue_references)
		#define CINDER_RVALUE_REFERENCES
	#endif
#endif

// Create a namepace alias as shorthand for cinder::
#if ! defined( CINDER_NO_NS_ALIAS )
	namespace ci = cinder;
#endif // ! defined( CINDER_NO_NS_ALIAS )

#if defined( CINDER_ANDROID )
#include <android/log.h>
#define CI_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define CI_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define CI_LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "cinder", __VA_ARGS__))
#define CI_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "cinder", __VA_ARGS__))
#define CI_LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, "cinder", __VA_ARGS__))

//  wstring compatibility
#include "cinder/android/wtypes.h"
#endif
