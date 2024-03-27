#include <sys/param.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <string.h>
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/threads.h>
#include <caml/unixsupport.h>

#define ctlval_Int_tag		0
#define ctlval_String_tag	1
#define ctlval_S64_tag		2
#define ctlval_Opaque_tag	3
#define ctlval_Uint_tag		4
#define ctlval_Long_tag		5
#define ctlval_Ulong_tag	6
#define ctlval_U64_tag		7
#define ctlval_U8_tag		8
#define ctlval_U16_tag		9
#define ctlval_S8_tag		10
#define ctlval_S16_tag		11
#define ctlval_S32_tag		12
#define ctlval_U32_tag		13

CAMLprim value
caml_sysctl_nametomib(value name)
{
	CAMLparam1 (name);
	CAMLlocal1 (res);
	int mib[CTL_MAXNAME];
	size_t size;
	int err;

	caml_release_runtime_system();
	size = nitems(mib);
	err = sysctlnametomib(String_val(name), mib, &size);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctlnametomib", name);
	}
	caml_acquire_runtime_system();

	res = caml_alloc(size, 0);
	for (size_t i = 0; i < size; i++) {
		Store_field(res, i, Val_int(mib[i]));
	}

	CAMLreturn (res);
}

CAMLprim value
caml_sysctl_next(value mib)
{
	CAMLparam1 (mib);
	CAMLlocal1 (next);
	int qoid[CTL_MAXNAME+2], nextoid[CTL_MAXNAME];
	mlsize_t size;
	size_t len;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	qoid[0] = CTL_SYSCTL;
	qoid[1] = CTL_SYSCTL_NEXT;
	for (mlsize_t i = 0; i < size; i++) {
		qoid[i + 2] = Int_val(Field(mib, i));
	}

	caml_release_runtime_system();
	len = sizeof nextoid;
	err = sysctl(qoid, size + 2, nextoid, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		if (err == ENOENT) {
			CAMLreturn (Val_none);
		}
		caml_unix_error(err, "sysctl", mib);
	}
	len /= sizeof nextoid[0];
	caml_acquire_runtime_system();

	next = caml_alloc(len, 0);
	for (mlsize_t i = 0; i < len; i++) {
		Store_field(next, i, Val_int(nextoid[i]));
	}
	CAMLreturn (caml_alloc_some(next));
}

CAMLprim value
caml_sysctl_next_noskip(value mib)
{
	CAMLparam1 (mib);
	CAMLlocal1 (next);
	int qoid[CTL_MAXNAME+2], nextoid[CTL_MAXNAME];
	mlsize_t size;
	size_t len;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	qoid[0] = CTL_SYSCTL;
	qoid[1] = CTL_SYSCTL_NEXTNOSKIP;
	for (mlsize_t i = 0; i < size; i++) {
		qoid[i + 2] = Int_val(Field(mib, i));
	}

	caml_release_runtime_system();
	len = sizeof nextoid;
	err = sysctl(qoid, size + 2, nextoid, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		if (err == ENOENT) {
			CAMLreturn (Val_none);
		}
		caml_unix_error(err, "sysctl", mib);
	}
	len /= sizeof nextoid[0];
	caml_acquire_runtime_system();

	next = caml_alloc(len, 0);
	for (mlsize_t i = 0; i < len; i++) {
		Store_field(next, i, Val_int(nextoid[i]));
	}
	CAMLreturn (caml_alloc_some(next));
}

CAMLprim value
caml_sysctl_name(value mib)
{
	CAMLparam1 (mib);
	int qoid[CTL_MAXNAME+2];
	u_char buf[BUFSIZ];
	mlsize_t size;
	size_t len;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	qoid[0] = CTL_SYSCTL;
	qoid[1] = CTL_SYSCTL_NAME;
	for (mlsize_t i = 0; i < size; i++) {
		qoid[i + 2] = Int_val(Field(mib, i));
	}

	caml_release_runtime_system();
	len = sizeof buf;
	err = sysctl(qoid, size + 2, buf, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctl", mib);
	}
	caml_acquire_runtime_system();

	CAMLreturn (caml_copy_string((const char *)buf));
}

CAMLprim value
caml_sysctl_format(value mib)
{
	CAMLparam1 (mib);
	CAMLlocal1 (res);
	int qoid[CTL_MAXNAME+2];
	u_char buf[BUFSIZ];
	const char *fmt;
	mlsize_t size;
	size_t len;
	u_int kind;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	qoid[0] = CTL_SYSCTL;
	qoid[1] = CTL_SYSCTL_OIDFMT;
	for (mlsize_t i = 0; i < size; i++) {
		qoid[i + 2] = Int_val(Field(mib, i));
	}

	caml_release_runtime_system();
	len = sizeof buf;
	err = sysctl(qoid, size + 2, buf, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctl", mib);
	}

	kind = *(u_int *)buf;
	fmt = (char *)(buf + sizeof(u_int));

	caml_acquire_runtime_system();
	res = caml_alloc(2, 0);
	Store_field(res, 0, Val_int(kind));
	Store_field(res, 1, caml_copy_string(fmt));
	CAMLreturn (res);
}

CAMLprim value
caml_sysctl_description(value mib)
{
	CAMLparam1 (mib);
	CAMLlocal1 (description);
	int qoid[CTL_MAXNAME+2];
	u_char buf[BUFSIZ];
	const char *descr;
	mlsize_t size;
	size_t len;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	qoid[0] = CTL_SYSCTL;
	qoid[1] = CTL_SYSCTL_OIDDESCR;
	for (mlsize_t i = 0; i < size; i++) {
		qoid[i + 2] = Int_val(Field(mib, i));
	}

	caml_release_runtime_system();
	len = sizeof buf;
	err = sysctl(qoid, size + 2, buf, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		if (err == ENOENT) {
			CAMLreturn (Val_none);
		}
		caml_unix_error(err, "sysctl", mib);
	}

	descr = (char *)buf;

	caml_acquire_runtime_system();
	description = caml_copy_string(descr);
	CAMLreturn (caml_alloc_some(description));
}

CAMLprim value
caml_sysctl_get(value mib)
{
	CAMLparam1 (mib);
	CAMLlocal1 (res);
	int qoid[CTL_MAXNAME+2];
	u_char buf[BUFSIZ];
	mlsize_t size;
	u_char *p;
	size_t len;
	u_int kind;
	int ctltype;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	qoid[0] = CTL_SYSCTL;
	qoid[1] = CTL_SYSCTL_OIDFMT;
	for (mlsize_t i = 0; i < size; i++) {
		qoid[i + 2] = Int_val(Field(mib, i));
	}

	caml_release_runtime_system();
	len = sizeof buf;
	err = sysctl(qoid, size + 2, buf, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctl", mib);
	}

	kind = *(u_int *)buf;
	ctltype = kind & CTLTYPE;
	if (ctltype == CTLTYPE_NODE) {
		caml_acquire_runtime_system();
		res = Val_int(0); /* first constant constructor */
		CAMLreturn (res);
	}

	len = 0;
	err = sysctl(qoid + 2, size, NULL, &len, NULL, 0);
	if (err) {
		err = errno;
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctl", mib);
	}
	len += len; /* double buffer size to be safe */
	p = malloc(len);
	if (p == NULL) {
		err = errno;
		caml_acquire_runtime_system();
		caml_unix_error(err, "malloc", Val_int(len));
	}
	err = sysctl(qoid + 2, size, p, &len, NULL, 0);
	if (err) {
		err = errno;
		free(p);
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctl", mib);
	}

	caml_acquire_runtime_system();
	switch (ctltype) {
	case CTLTYPE_INT:
		res = caml_alloc(1, ctlval_Int_tag);
		Store_field(res, 0, Val_int(*(const int *)p));
		break;
	case CTLTYPE_STRING:
		res = caml_alloc(1, ctlval_String_tag);
		Store_field(res, 0, caml_copy_string((const char *)p));
		break;
	case CTLTYPE_S64:
		res = caml_alloc(1, ctlval_S64_tag);
		Store_field(res, 0, caml_copy_int64(*(const int64_t *)p));
		break;
	case CTLTYPE_OPAQUE:
		// TODO: handle struct types
		res = caml_alloc(1, ctlval_Opaque_tag);
		Store_field(res, 0, caml_alloc_initialized_string(len, (const char *)p));
		break;
	case CTLTYPE_UINT:
		res = caml_alloc(1, ctlval_Uint_tag);
		Store_field(res, 0, Val_int(*(const unsigned int *)p));
		break;
	case CTLTYPE_LONG:
		res = caml_alloc(1, ctlval_Long_tag);
		Store_field(res, 0, Val_int(*(const long *)p));
		break;
	case CTLTYPE_ULONG:
		res = caml_alloc(1, ctlval_Ulong_tag);
		Store_field(res, 0, Val_int(*(const unsigned long *)p));
		break;
	case CTLTYPE_U64:
		res = caml_alloc(1, ctlval_U64_tag);
		Store_field(res, 0, caml_copy_int64(*(const uint64_t *)p));
		break;
	case CTLTYPE_U8:
		res = caml_alloc(1, ctlval_U8_tag);
		Store_field(res, 0, caml_copy_int32(*(const uint8_t *)p));
		break;
	case CTLTYPE_U16:
		res = caml_alloc(1, ctlval_U16_tag);
		Store_field(res, 0, caml_copy_int32(*(const uint16_t *)p));
		break;
	case CTLTYPE_S8:
		res = caml_alloc(1, ctlval_S8_tag);
		Store_field(res, 0, caml_copy_int32(*(const int8_t *)p));
		break;
	case CTLTYPE_S16:
		res = caml_alloc(1, ctlval_S16_tag);
		Store_field(res, 0, caml_copy_int32(*(const int16_t *)p));
		break;
	case CTLTYPE_S32:
		res = caml_alloc(1, ctlval_S32_tag);
		Store_field(res, 0, caml_copy_int32(*(const int32_t *)p));
		break;
	case CTLTYPE_U32:
		res = caml_alloc(1, ctlval_U32_tag);
		Store_field(res, 0, caml_copy_int32(*(const uint32_t *)p));
		break;
	default:
		free(p);
		caml_failwith("Unhandled sysctl CTLTYPE");
	}
	free(p);
	CAMLreturn (res);
}

CAMLprim value
caml_sysctl_set(value mib, value val)
{
	CAMLparam2 (mib, val);
	int oid[CTL_MAXNAME];
	mlsize_t size;
	void *p;
	size_t len;
	int err;

	size = Wosize_val(mib);
	if (size > CTL_MAXNAME) {
		caml_invalid_argument("Invalid mib: too long");
	}

	for (mlsize_t i = 0; i < size; i++) {
		oid[i] = Int_val(Field(mib, i));
	}

	/* Do nothing for ctlval Node values. */
	if (val == Int_val(0)) {
		CAMLreturn (Val_unit);
	}
	switch (Tag_val(val)) {
	case ctlval_Int_tag: {
		int v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(int *)p = v;
		break;
	}
	case ctlval_String_tag: {
		const char *s = String_val(Field(val, 0));
		len = strlen(s);
		p = strdup(s);
		if (p == NULL) {
			caml_uerror("strdup", Field(val, 0));
		}
		break;
	}
	case ctlval_S64_tag: {
		int64_t v = Int64_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(int64_t *)p = v;
		break;
	}
	case ctlval_Opaque_tag: {
		u_char *v = Bytes_val(Field(val, 0));
		len = caml_string_length(Field(val, 0));
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		memcpy(p, v, len);
		break;
	}
	case ctlval_Uint_tag: {
		u_int v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(u_int *)p = v;
		break;
	}
	case ctlval_Long_tag: {
		long v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(long *)p = v;
		break;
	}
	case ctlval_Ulong_tag: {
		u_long v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(u_long *)p = v;
		break;
	}
	case ctlval_U64_tag: {
		uint64_t v = Int64_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(uint64_t *)p = v;
		break;
	}
	case ctlval_U8_tag: {
		uint8_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(uint8_t *)p = v;
		break;
	}
	case ctlval_U16_tag: {
		uint16_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(uint16_t *)p = v;
		break;
	}
	case ctlval_S8_tag: {
		int8_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(int8_t *)p = v;
		break;
	}
	case ctlval_S16_tag: {
		int16_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(int16_t *)p = v;
		break;
	}
	case ctlval_S32_tag: {
		int32_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(int32_t *)p = v;
		break;
	}
	case ctlval_U32_tag: {
		uint32_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_uerror("malloc", Val_int(len));
		}
		*(uint32_t *)p = v;
		break;
	}
	default:
		caml_failwith("Unhandled ctlval tag");
	}
	
	caml_release_runtime_system();
	err = sysctl(oid, size, NULL, NULL, p, len);
	if (err) {
		err = errno;
		free(p);
		caml_acquire_runtime_system();
		caml_unix_error(err, "sysctl", mib);
	}
	free(p);
	caml_acquire_runtime_system();
	CAMLreturn (Val_unit);
}
