//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  @ingroup system
//!  Various system utilities.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsCerrReport.h"
#include "tsUString.h"

#if defined(TS_UNIX)
#include "tsBeforeStandardHeaders.h"
#include <cerrno>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sched.h>
#include "tsAfterStandardHeaders.h"
#endif

namespace ts
{
	//!
	//! Get the error code of the last operating system call.
	//! The validity of the returned value may depends on specific conditions.
	//!
	//! Portability of error code representation: On UNIX, error codes are just @c int. On Windows, error
	//! codes are @c DWORD, which is compatible with @c int. In C++11, @c std::error_core uses @c int for
	//! error codes. Therefore, because of this new C++11 feature, we just use @c int.
	//!
	//! Windows note: According to Windows documentation, socket functions should call WSAGetLastError()
	//! instead of GetLastError() to retrieve the error code. This is an oddity from the old 16-bit
	//! Windows API. On Win32, various sources confirm that WSAGetLastError() just call GetLastError().
	//! Thus, in this application, we do not make the difference.
	//!
	//! @return The error code of the last operating system call.
	//!
	TSDUCKDLL inline int LastSysErrorCode()
	{
		#if defined(TS_WINDOWS)
		return ::GetLastError();
		#else
		return errno;
		#endif
	}

	//!
	//! Format a system error code into a string.
	//! @param [in] code An error code from the operating system.
	//! Typically a result from @c errno (Unix) or @c GetLastError() (Windows).
	//! @param [in] category Error category, system by default.
	//! @return A string describing the error.
	//!
	TSDUCKDLL inline std::string SysErrorCodeMessage(int code = LastSysErrorCode(), const std::error_category &category = std::system_category())
	{
		return std::error_code(code, category).message();
	}

	//!
	//! Portable type for ioctl() request parameter.
	//!
	#if defined(DOXYGEN)
	typedef platform - dependent ioctl_request_t;
	#elif defined(TS_WINDOWS)
		// Second parameter of ::DeviceIoControl().
	typedef ::DWORD ioctl_request_t;
	#else
		// Extract the type of the second parameter of ::ioctl().
		// It is "unsigned long" on most Linux systems but "int" on Alpine Linux.
	template<typename T>
	T request_param_type(int (*ioctl_syscall)(int, T, ...));
	typedef decltype(request_param_type(&::ioctl)) ioctl_request_t;
	#endif

	//!
	//! Check if the current user is privileged (root on UNIX, an administrator on Windows).
	//! @return True if the current user is privileged.
	//!
	TSDUCKDLL bool IsPrivilegedUser();

	//!
	//! Ensure that writing to a broken pipe does not kill the current process.
	//!
	//! On UNIX systems, writing to a <i>broken pipe</i>, i.e. a pipe with
	//! no process reading from it, kills the current process. This may not
	//! be what you want. This functions prevents this.
	//!
	//! <strong>UNIX Systems:</strong> This function ignores SIGPIPE.
	//! Writing to a broken pipe will now return an error instead of killing
	//! the process.
	//!
	//! <strong>Windows systems:</strong> This function does nothing (because
	//! there is no need to do anything).
	//!
	TSDUCKDLL void IgnorePipeSignal();

	//!
	//! Check if the standard input is a terminal.
	//! @return True if the standard input is a terminal.
	//!
	TSDUCKDLL bool StdInIsTerminal();

	//!
	//! Check if the standard output is a terminal.
	//! @return True if the standard output is a terminal.
	//!
	TSDUCKDLL bool StdOutIsTerminal();

	//!
	//! Check if the standard error is a terminal.
	//! @return True if the standard error is a terminal.
	//!
	TSDUCKDLL bool StdErrIsTerminal();

	//!
	//! Get the name of a class from the @c type_info of an object.
	//! The result may be not portable.
	//! @param [in] info The @c type_info of an object.
	//! @return An implementation-specific name of the object class.
	//!
	TSDUCKDLL UString ClassName(const std::type_info &info);
}
