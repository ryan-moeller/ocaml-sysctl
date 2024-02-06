#include <sys/param.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>

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
caml_sysctl_name2mib(value name)
{
	CAMLparam1 (name);
	CAMLlocal1 (res);
	int mib[CTL_MAXNAME];
	size_t size;
	int err;

	size = nitems(mib);
	err = sysctlnametomib(String_val(name), mib, &size);
	if (err) {
		caml_failwith(strerror(errno));
	}

	res = caml_alloc(size, 0);
	for (size_t i = 0; i < size; i++) {
		Store_field(res, i, Val_int(mib[i]));
	}

	CAMLreturn (res);
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

	len = sizeof(buf);
	err = sysctl(qoid, size + 2, buf, &len, NULL, 0);
	if (err) {
		caml_failwith(strerror(errno));
	}

	kind = *(u_int *)buf;
	ctltype = kind & CTLTYPE;
	if (ctltype == CTLTYPE_NODE) {
		res = Val_int(0); /* first constant constructor */
		CAMLreturn (res);
	}

	len = 0;
	err = sysctl(qoid + 2, size, NULL, &len, NULL, 0);
	if (err) {
		caml_failwith(strerror(errno));
	}
	len += len; /* double buffer size to be safe */
	p = malloc(len);
	if (p == NULL) {
		caml_failwith(strerror(errno));
	}
	err = sysctl(qoid + 2, size, p, &len, NULL, 0);
	if (err) {
		int e = errno;
		free(p);
		caml_failwith(strerror(e));
	}

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
			caml_failwith(strerror(errno));
		}
		*(int *)p = v;
		break;
	}
	case ctlval_String_tag: {
		const char *s = String_val(Field(val, 0));
		len = strlen(s);
		p = strdup(s);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		break;
	}
	case ctlval_S64_tag: {
		int64_t v = Int64_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(int64_t *)p = v;
		break;
	}
	case ctlval_Opaque_tag: {
		u_char *v = Bytes_val(Field(val, 0));
		len = caml_string_length(Field(val, 0));
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		memcpy(p, v, len);
		break;
	}
	case ctlval_Uint_tag: {
		u_int v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(u_int *)p = v;
		break;
	}
	case ctlval_Long_tag: {
		long v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(long *)p = v;
		break;
	}
	case ctlval_Ulong_tag: {
		u_long v = Int_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(u_long *)p = v;
		break;
	}
	case ctlval_U64_tag: {
		uint64_t v = Int64_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(uint64_t *)p = v;
		break;
	}
	case ctlval_U8_tag: {
		uint8_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(uint8_t *)p = v;
		break;
	}
	case ctlval_U16_tag: {
		uint16_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(uint16_t *)p = v;
		break;
	}
	case ctlval_S8_tag: {
		int8_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(int8_t *)p = v;
		break;
	}
	case ctlval_S16_tag: {
		int16_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(int16_t *)p = v;
		break;
	}
	case ctlval_S32_tag: {
		int32_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(int32_t *)p = v;
		break;
	}
	case ctlval_U32_tag: {
		uint32_t v = Int32_val(Field(val, 0));
		len = sizeof v;
		p = malloc(len);
		if (p == NULL) {
			caml_failwith(strerror(errno));
		}
		*(uint32_t *)p = v;
		break;
	}
	default:
		caml_failwith("Unhandled ctlval tag");
	}
	
	err = sysctl(oid, size, NULL, NULL, p, len);
	if (err) {
		int e = errno;
		free(p);
		caml_failwith(strerror(e));
	}
	free(p);
	CAMLreturn (Val_unit);
}
