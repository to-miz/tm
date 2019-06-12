#if defined(TMF_USE_WINDOWS_H)
#ifndef TMF_NO_SHLWAPI_H
#include <Shlwapi.h> /* PathRelativePathToW */
#endif

void tmf_make_relative_path_n_internal(const char* from, tm_size_t from_len, tmf_path_type from_type, const char* to,
                                       tm_size_t to_len, tmf_path_type to_type, tmf_contents_result* out) {
    tmf_platform_path platform_from;
    platform_from.path = TM_NULL;
    tmf_platform_path platform_to;
    platform_to.path = TM_NULL;
    if (tmf_to_platform_path_n(from, from_len, &platform_from) && tmf_to_platform_path_n(to, to_len, &platform_to)) {
    	WCHAR buffer[MAX_PATH];
    	DWORD from_attr = (from_type == tmf_path_is_directory) ? (FILE_ATTRIBUTE_DIRECTORY) : (0);
    	DWORD to_attr = (to_type == tmf_path_is_directory) ? (FILE_ATTRIBUTE_DIRECTORY) : (0);
    	if (PathRelativePathToW(buffer, platform_from.path, from_attr, platform_to.path, to_attr)) {
    		*out = tmf_to_utf8(buffer, 0);
    	} else {
    		out->ec = TM_EINVAL;
    	}
    }
    tmf_destroy_platform_path(&platform_from);
    tmf_destroy_platform_path(&platform_to);
}

#endif /* defined(TMF_USE_WINDOWS_H) */

#if !defined(TMF_USE_WINDOWS_H)

#error Not implemented.
#if 0
static tm_bool tmf_from_str(const char* str, tmf_contents_result* out) {
	TM_ASSERT(out);
	TM_ASSERT(!out->contents.data);
	TM_ASSERT(str);

	size_t len = TMF_STRLEN(str);
	out->contents.data = (char*)TMF_MALLOC(len * sizeof(char), sizeof(char));
	if (!out->contents.data) {
		out->ec = TM_ENOMEM;
		return TM_FALSE;
	} else {
		out->contents.size = (tm_size_t)len;
		out->contents.capacity = (tm_size_t)len;
		TMF_MEMCPY(out->contents.data, "./", len * sizeof(char));
		return TM_TRUE;
	}
}

static const char* tmf_find_char(const char* first, const char* last, char c) {
    const void* result = TMF_MEMCHR(first, (unsigned char)c, (size_t)(last - first));
    if (!result) return last;
    return (const char*)result;
}

void tmf_make_relative_path_n_internal(const char* from, tm_size_t from_len, tmf_path_type from_type, const char* to,
                                       tm_size_t to_len, tmf_path_type to_type, tmf_contents_result* out) {
    /* Implementation based on this: https://stackoverflow.com/a/9978227/6182234 */

    if (!from || from_len <= 0 || !to || to_len <= 0) {
        out->ec = TM_EINVAL;
        return;
    }

#ifdef WIN32
    /* Check whether paths are on same device. */
    if (*from != *to) {
        out->ec = TM_EXDEV;
        return;
    }
#endif

    /*
	C:/Users/TM/Desktop/a/b/c/test.txt
	C:/Users/TM/Desktop/d/e/f
	../../../a/b/c/test.txt
    */

    /* Skip common paths. */
    const char* from_last = from + from_len;
    const char* to_last = to + to_len;

    for (;;) {
	    const char* from_next = tmf_find_char(from, from_last, '/');
	    const char* to_next = tmf_find_char(to, to_last, '/');


    }
}
#endif

#endif /* !defined(TMF_USE_WINDOWS_H) */


tmf_contents_result tmf_make_relative_path(const char* from, tmf_path_type from_type, const char* to,
                                           tmf_path_type to_type) {
    tmf_contents_result result = {{TM_NULL, 0, 0}, TM_OK};
    if (!from || !to) {
        result.ec = TM_EINVAL;
    } else {
        tmf_make_relative_path_n_internal(from, from_len, from_type, to, to_len, to_type, &result);
    }

    return result;
}