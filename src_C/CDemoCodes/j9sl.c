/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 * 
 * (c) Copyright IBM Corp. 1991, 2008 All Rights Reserved
 * 
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

/**
 * @file
 * @ingroup Port
 * @brief shared library
 */


/* 
 *	Standard unix shared libraries
 *	(AIX: 4.2 and higher only)
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ldr.h>
#include <load.h>
#include <dlfcn.h>
#include "ut_j9prt.h"

/* Start copy from j9filetext.c */
/* __STDC_ISO_10646__ indicates that the platform wchar_t encoding is Unicode */
/* but older versions of libc fail to set the flag, even though they are Unicode */
#if defined(__STDC_ISO_10646__)
#define J9VM_USE_MBTOWC
#else
#include "j9iconvhelpers.h"
#endif


#if defined(J9VM_USE_MBTOWC) || defined(J9VM_USE_ICONV)
#include <nl_types.h>
#include <langinfo.h>

/* Some older platforms (Netwinder) don't declare CODESET */
#ifndef CODESET
#define CODESET _NL_CTYPE_CODESET_NAME
#endif
#endif

/* End copy */

#include "j9port.h"
#include "j9lib.h"
#include "portnls.h"

#if defined(J9OS_I5)
#include "Xj9I5OSInterface.H"
#endif

#define PLATFORM_DLL_EXTENSION ".so"

#if (defined(J9VM_USE_MBTOWC)) /* priv. proto (autogen) */

static void convertWithMBTOWC(struct J9PortLibrary *portLibrary, char *error, char *errBuf, UDATA bufLen);
#endif /* J9VM_USE_MBTOWC (autogen) */



#if (defined(J9VM_USE_ICONV)) /* priv. proto (autogen) */

static void convertWithIConv(struct J9PortLibrary *portLibrary, const char *error, char *errBuf, UDATA bufLen);
#endif /* J9VM_USE_ICONV (autogen) */



static void getDLError(struct J9PortLibrary *portLibrary, char *errBuf, UDATA bufLen);
static UDATA getDirectoryOfLibrary(struct J9PortLibrary *portLibrary, char *buf, UDATA bufLen);


/**
 * Close a shared library.
 *
 * @param[in] portLibrary The port library.
 * @param[in] descriptor Shared library handle to close.
 *
 * @return 0 on success, any other value on failure.
 */
UDATA VMCALL j9sl_close_shared_library(struct J9PortLibrary *portLibrary, UDATA descriptor)
{
	UDATA result = 0;

	Trc_PRT_sl_close_shared_library_Entry(descriptor);
#if defined(J9OS_I5)
	result = (UDATA) Xj9dlclose(descriptor);
#else
	result = (UDATA) dlclose((void *)descriptor);
#endif

	Trc_PRT_sl_close_shared_library_Exit(result);
	return result;
}
/** 
 * Opens a shared library .
 *
 * @param[in] portLibrary The port library.
 * @param[in] name path Null-terminated string containing the shared library.
 * @param[out] descriptor Pointer to memory which is filled in with shared-library handle on success.
 * @param[out] errBuf Buffer to contain an error message on failure.
 * @param[in] bufLen Size of errBuf.
 * @param[in] flags bitfield comprised of J9PORT_SLOPEN_* constants
 *
 * @return 0 on success, any other value on failure.
 *
 * @note contents of descriptor are undefined on failure.
 */
UDATA VMCALL
j9sl_open_shared_library(struct J9PortLibrary *portLibrary, char *name, UDATA *descriptor, UDATA flags)
{
	void *handle;
	char *openName = name;
	char *fileName;
	char mangledName[1024];
	char errBuf[512];
	UDATA result;
	int lazyOrNow = (flags & J9PORT_SLOPEN_LAZY) ? RTLD_LAZY : RTLD_NOW;
	UDATA decorate = flags & J9PORT_SLOPEN_DECORATE;

	Trc_PRT_sl_open_shared_library_Entry(name, flags);

	fileName = strrchr(name, '/');

	if (decorate) {
		if ( fileName ) {
			/* the names specifies a path */
			portLibrary->str_printf(portLibrary, mangledName, 1024, "%.*slib%s" PLATFORM_DLL_EXTENSION, (UDATA)fileName+1-(UDATA)name, name, fileName+1);
		} else {
			portLibrary->str_printf(portLibrary, mangledName, 1024, "lib%s" PLATFORM_DLL_EXTENSION, name);
		}
		openName = mangledName;
	}

	Trc_PRT_sl_open_shared_library_Event1(openName);

	/* CMVC 137341: 
	 * dlopen() searches for libraries using the LIBPATH envvar as it was when the process
	 * was launched.  This causes multiple issues such as:
	 *  - finding 32 bit binaries for libjsig.so instead of the 64 bit binary needed and vice versa
	 *  - finding compressed reference binaries instead of non-compressed ref binaries
	 * 
	 * calling loadAndInit(libname, 0 -> no flags, NULL -> use the currently defined LIBPATH) allows
	 * us to load the library with the current libpath instead of the one at process creation
	 * time. We can then call dlopen() as per normal and the just loaded library will be found. 
	 * */
	loadAndInit(openName, L_RTLD_LOCAL, NULL);
	handle = dlopen(openName, lazyOrNow);
	if (handle == NULL) {
		char portLibDir[1024];
		/* get the initial error message, which may be more
		accurate than the errors from subsequent attempts */
		getDLError(portLibrary, errBuf, sizeof(errBuf));
		if (portLibrary->file_attr(portLibrary, openName) == EsIsFile) {
			result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
		} else {
			result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_NOT_FOUND, errBuf);
		}
		
		/* last ditch, try dir port lib DLL is in */
		if (getDirectoryOfLibrary(portLibrary, portLibDir, 1024-strlen(openName)-2)) {
			strcat(portLibDir, "/");
			strcat(portLibDir, openName);
			loadAndInit(portLibDir, L_RTLD_LOCAL, NULL);
			handle = dlopen(portLibDir, lazyOrNow);
			if ( handle == NULL ) {
				/* update the error string and return code only if a file was actually found */
				if (portLibrary->file_attr(portLibrary, portLibDir) == EsIsFile) {
					getDLError(portLibrary, errBuf, sizeof(errBuf));
					result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
				}
			}
		}
	}
	if ( handle == NULL ) {
#if defined(J9OS_I5)
		/* now check to see if it is an iSeries library. */
		handle = Xj9LoadIleLibrary(openName,errBuf,sizeof(errBuf));
		if (handle == NULL) {
		    /* was an error message return?  */
		    if (errBuf[0] != '\0') {
				/* Yes, save it */
				result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
		    }
#endif
			if (decorate) {
				if ( fileName ) {
					/* the names specifies a path */
					portLibrary->str_printf(portLibrary, mangledName, 1024, "%.*slib%s.a", (UDATA)fileName+1-(UDATA)name, name, fileName+1);
				} else {
					portLibrary->str_printf(portLibrary, mangledName, 1024, "lib%s.a", name);
				}
			
				Trc_PRT_sl_open_shared_library_Event1(mangledName);
				loadAndInit(mangledName, L_RTLD_LOCAL, NULL);
				handle = dlopen(mangledName, lazyOrNow);
				if( handle == NULL) {
					/* update the error string and return code only if a file was actually found */
					if (portLibrary->file_attr(portLibrary, mangledName) == EsIsFile) {
						getDLError(portLibrary, errBuf, sizeof(errBuf));
						result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
					}
#if defined(J9OS_I5)
					/* now check to see if it is an iSeries library */
					handle = Xj9LoadIleLibrary(mangledName,errBuf,sizeof(errBuf));
					if (handle == NULL) {
				    	/* was an error message return?  */
				   		if (errBuf[0] != '\0') {
							/* Yes, save it */
							result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
				    	}
						if ( fileName ) {
							/* the names specifies a path */
							portLibrary->str_printf(portLibrary, mangledName, 1024, "%.*s%s.srvpgm", (UDATA)fileName+1-(UDATA)name, name, fileName+1);
				    	} else {
							portLibrary->str_printf(portLibrary, mangledName, 1024, "%s.srvpgm", name);
				    	}
					
						Trc_PRT_sl_open_shared_library_Event1(mangledName);
				    	handle = dlopen(mangledName, lazyOrNow);
						if( handle == NULL) {
							/* update the error string and return code only if a file was actually found */
							if (portLibrary->file_attr(portLibrary, mangledName) == EsIsFile) {
					    		getDLError(portLibrary, errBuf, sizeof(errBuf));
					   			result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
							}
							/* now check to see if it is an iSeries library */
							handle = Xj9LoadIleLibrary(mangledName,errBuf,sizeof(errBuf));
							if (handle == NULL) {
					    		/* was an error message return?  */
					   			if (errBuf[0] != '\0') {
									/* Yes, save it */
									result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
					    		}
							}
#endif
							if( strchr(fileName, '(') != NULL && strchr(fileName, ')') != NULL ) {
								Trc_PRT_sl_open_shared_library_Event1(name);

								loadAndInit(name, L_RTLD_LOCAL, NULL);
								handle = dlopen(name, lazyOrNow | RTLD_MEMBER);
								if( handle == NULL ) {
									char *actualFileName = strrchr(fileName, '(');
									portLibrary->str_printf(portLibrary, mangledName, 1024, "%.*s%.*s", 
										(UDATA)fileName+1-(UDATA)name, 
										name, 
										(UDATA)actualFileName-1-(UDATA)fileName,
										fileName+1);
									if (portLibrary->file_attr(portLibrary, mangledName) == EsIsFile) {
										getDLError(portLibrary, errBuf, sizeof(errBuf));
										result = portLibrary->error_set_last_error_with_message(portLibrary, J9PORT_SL_INVALID, errBuf);
									}
								}
							}
#if defined(J9OS_I5)
						}
					}
#endif
				}
			}
#if defined(J9OS_I5)
		}
#endif
	}
	
	if (handle == NULL) {
		Trc_PRT_sl_open_shared_library_Exit2(result);
		return result;
	}

	Trc_PRT_sl_open_shared_library_Exit1(handle);
	*descriptor = (UDATA) handle;
	return 0;
}
/**
 * Search for a function named 'name' taking argCount in the shared library 'descriptor'.
 *
 * @param[in] portLibrary The port library.
 * @param[in] descriptor Shared library to search.
 * @param[in] name Function to look up.
 * @param[out] func Pointer to the function.
 * @param[in] argSignature Argument signature.
 *
 * @return 0 on success, any other value on failure.
 *
 * argSignature is a C (ie: NUL-terminated) string with the following possible values for each character:
 *
 *		V	- void
 *		Z	- boolean
 *		B	- byte
 *		C	- char (16 bits)
 *		I	- integer (32 bits)
 *		J	- long (64 bits)
 *		F	- float (32 bits) 
 *		D	- double (64 bits) 
 *		L	- object / pointer (32 or 64, depending on platform)
 *		P	- pointer-width platform data. (in J9 terms an IDATA)
 *
 * Lower case signature characters imply unsigned value.
 * Upper case signature characters imply signed values.
 * If it doesn't make sense to be signed/unsigned (eg: V, L, F, D Z) the character is upper case.
 * 
 * argList[0] is the return type from the function.
 * The argument list is as it appears in english: list is left (1) to right (argCount)
 *
 * @note contents of func are undefined on failure.
 */
UDATA VMCALL
j9sl_lookup_name(struct J9PortLibrary *portLibrary, UDATA descriptor, char *name, UDATA *func, const char *argSignature)
{
	void *address = NULL;

	Trc_PRT_sl_lookup_name_Entry(descriptor, name, argSignature);

#if defined(J9OS_I5)
	address = Xj9dlsym(descriptor, name, argSignature);
#else
	address = dlsym((void *)descriptor, name);
#endif
	if( address == NULL ) {
		Trc_PRT_sl_lookup_name_Exit2(name, argSignature, descriptor, 1);
		return 1;
	}
	*func = (UDATA) address;
	
	Trc_PRT_sl_lookup_name_Exit1(*func);
	return 0;
}

static void
getDLError(struct J9PortLibrary *portLibrary, char *errBuf, UDATA bufLen)
{
	const char *error;
	char charbuf[1024];
	UDATA dlopen_errno = errno;

	if (bufLen == 0) {
		return;
	}

	error = dlerror();

	/* dlerror can be misleading on AIX.  If the file we tried to dlopen exists maybe 
	  * loadquery will generate a better message. 
	  */
	if (dlopen_errno == ENOENT) {
		char *buffer[1024];
		if (loadquery(L_GETMESSAGES, buffer, sizeof(buffer)) != -1) {
			/* This checks for an error loading a dependant module which dlopen seems to report incorrectly */
			if (atoi(buffer[0]) == L_ERROR_NOLIB && atoi(buffer[1]) == L_ERROR_DEPENDENT) {
				error = portLibrary->nls_lookup_message(portLibrary, 
					J9NLS_ERROR|J9NLS_DO_NOT_APPEND_NEWLINE, 
					J9NLS_PORT_SL_ERROR_LOADING_DEPENDANT_MODULE,  
					NULL);
				portLibrary->str_printf(portLibrary, errBuf, bufLen, error, &buffer[1][3]);
				return;
			}
		}
	} else if (dlopen_errno == ENOEXEC && (error == NULL || error[0] == '\0')) {
		/* Testing shows that this special case occurs
		when there is a symbol resolution problem */
		error = portLibrary->nls_lookup_message(portLibrary, 
			J9NLS_ERROR|J9NLS_DO_NOT_APPEND_NEWLINE, 
			J9NLS_PORT_SL_SYMBOL_RESOLUTION_FAILURE, 
			NULL);
		strncpy(errBuf, error, bufLen);
		errBuf[bufLen - 1] = '\0';
		return;
	}

	if (error == NULL || error[0] == '\0') {
		/* just in case another thread consumed our error message */
		error = portLibrary->nls_lookup_message(portLibrary, 
			J9NLS_ERROR|J9NLS_DO_NOT_APPEND_NEWLINE, 
			J9NLS_PORT_SL_UNKOWN_ERROR, 
			NULL);
		strncpy(errBuf, error, bufLen);
		errBuf[bufLen - 1] = '\0';
		return;
	}

#if defined(J9VM_USE_MBTOWC)
	convertWithMBTOWC(portLibrary, error, errBuf, bufLen);	
#elif defined(J9VM_USE_ICONV)
	convertWithIConv(portLibrary, error, errBuf, bufLen);	
#else
	strncpy(errBuf, error, bufLen);
	errBuf[bufLen - 1] = '\0';
#endif
}


/**
 * determine the path to the port library
 * 
 * @param[out] buf Pointer to memory which is filled in with path to port library
 * 
 * @return 0 on failure, any other value on failure
 */
static UDATA
getDirectoryOfLibrary(struct J9PortLibrary *portLib, char *buf, UDATA bufLen)
{
	struct ld_info *linfo, *linfop;
	int             linfoSize, rc;
	char           *myAddress;

	/* get loader information */
	linfoSize = 1024;
	linfo = portLib->mem_allocate_memory(portLib, linfoSize, J9_GET_CALLSITE(), J9MEM_CATEGORY_PORT_LIBRARY);
	if (NULL==linfo) {
		return 0;
	}
	for (;;) {
		rc = loadquery(L_GETINFO, linfo, linfoSize);
		if (rc != -1) {
			break;
		}
		linfoSize *= 2; /* insufficient buffer size - increase */
		linfop = portLib->mem_reallocate_memory(portLib, linfo, linfoSize, J9_GET_CALLSITE(), J9MEM_CATEGORY_PORT_LIBRARY);
		if (NULL==linfop) {
			portLib->mem_free_memory(portLib, linfo);
			return 0;
		}
		linfo = linfop;
	}

	/* find entry for my loaded object */
	myAddress = ((char **)&j9sl_open_shared_library)[0];
	for (linfop = linfo;;) {
		char *textorg  = (char *)linfop->ldinfo_textorg;
		char *textend  = textorg + (unsigned long)linfop->ldinfo_textsize;
		if ((myAddress >= textorg) && (myAddress < textend)) {
			break;
		}
		if (!linfop->ldinfo_next) {
			portLib->mem_free_memory(portLib, linfo);
			return 0;
		}
		linfop = (struct ld_info *)((char *)linfop + linfop->ldinfo_next);
	}

	strncpy(buf, linfop->ldinfo_filename, bufLen);
	/* remove libj9prt...so, reused myAddress here */
	myAddress = strrchr(buf,'/');
	if (NULL!=myAddress) {
		*myAddress = '\0';
	}

	portLib->mem_free_memory(portLib, linfo);

	return 1;
}


#if (defined(J9VM_USE_ICONV)) /* priv. proto (autogen) */

static void
convertWithIConv(struct J9PortLibrary *portLibrary, const char *error, char *errBuf, UDATA bufLen)
{
	iconv_t converter;
	size_t inbytesleft, outbytesleft;
	char* inbuf, *outbuf;

	converter = iconv_get(portLibrary, J9SL_ICONV_DESCRIPTOR, "UTF-8", nl_langinfo(CODESET));

	if (J9VM_INVALID_ICONV_DESCRIPTOR == converter) {
		/* no converter available for this code set. Just dump the platform chars */
		strncpy(errBuf, error, bufLen);
		errBuf[bufLen - 1] = '\0';
		return;
	}

	inbuf = (char*)error; /* for some reason this argument isn't const */
	outbuf = errBuf;
	inbytesleft = strlen(error);
	outbytesleft = bufLen - 1;

	while ( (outbytesleft > 0) && (inbytesleft > 0) ) {
		if ( (size_t)-1 == iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft) ) {
			if (errno == E2BIG) {
				break;
			}

			/* if we couldn't translate this character, copy one byte verbatim */
			*outbuf = *inbuf;
			outbuf++;
			inbuf++;
			inbytesleft--;
			outbytesleft--;
		}
	}

	*outbuf = '\0';
	iconv_free(portLibrary, J9SL_ICONV_DESCRIPTOR, converter);
}

#endif /* J9VM_USE_ICONV (autogen) */


#if (defined(J9VM_USE_MBTOWC)) /* priv. proto (autogen) */

static void
convertWithMBTOWC(struct J9PortLibrary *portLibrary, char *error, char *errBuf, UDATA bufLen)
{
	char *out, *end, *walk;
	wchar_t ch;
	int ret;

	out = errBuf;
	end = &errBuf[bufLen - 1];

	walk = error;

	/* reset the shift state */
	mbtowc(NULL, NULL, 0);

	while(*walk) {
		ret = mbtowc(&ch,walk,MB_CUR_MAX);
		if (ret < 0) { 
			ch = *walk++;
		} else if(ret == 0) {
			break;
		} else {
			walk += ret;
		}

		if (ch == '\r') continue;
		if (ch == '\n') ch = ' ';
		if (ch < 0x80) {
			if ((out + 1) > end) break;
			*out++ = (char)ch;
		} else if (ch < 0x800) {
			if ((out + 2) > end) break;
			*out++ = (char)(0xc0 | ((ch >> 6) & 0x1f));
			*out++ = (char)(0x80 | (ch & 0x3f));
		} else {
			if ((out + 3) > end) break;
			*out++ = (char)(0xe0 | ((ch >> 12) & 0x0f));
			*out++ = (char)(0x80 | ((ch >> 6) & 0x3f));
			*out++ = (char)(0x80 | (ch & 0x3f));
		}
	}

	*out = '\0';
}

#endif /* J9VM_USE_MBTOWC (autogen) */


/**
 * PortLibrary shutdown.
 *
 * This function is called during shutdown of the portLibrary.  Any resources that were created by @ref j9sl_startup
 * should be destroyed here.
 *
 * @param[in] portLibrary The port library.
 *
 * @note Most implementations will be empty.
 */
void VMCALL
j9sl_shutdown(struct J9PortLibrary *portLibrary)
{
}
/**
 * PortLibrary startup.
 *
 * This function is called during startup of the portLibrary.  Any resources that are required for
 * the shared library operations may be created here.  All resources created here should be destroyed
 * in @ref j9sl_shutdown.
 *
 * @param[in] portLibrary The port library.
 *
 * @return 0 on success, negative error code on failure.  Error code values returned are
 * \arg J9PORT_ERROR_STARTUP_SL
 *
 * @note Most implementations will simply return success.
 */
I_32 VMCALL
j9sl_startup(struct J9PortLibrary *portLibrary)
{
	return 0;
}
