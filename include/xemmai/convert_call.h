#define XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__COMMA_IF(XEMMAI__MACRO__N)
#define XEMMAI__MACRO__TYPENAME_T_AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TYPENAME_T_AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__T_AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__T_AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__T_AN_A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__T_AN_A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__TRANSFER_A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TRANSFER_A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__AS_A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__AS_A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__AS_AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__AS_AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__IS(n) if (!f_is<T_a##n>(a_stack.f_at(XEMMAI__MACRO__N - 1 - n))) return false;
#define XEMMAI__MACRO__LIMIT_N XEMMAI__MACRO__SUBTRACT(XEMMAI__MACRO__ARGUMENTS_LIMIT, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__COMMA_UNSPECIFIED__ XEMMAI__MACRO__COMMA_IF(XEMMAI__MACRO__LIMIT_N) XEMMAI__MACRO__JOIN(XEMMAI__MACRO__UNSPECIFIED, XEMMAI__MACRO__LIMIT_N)
#define XEMMAI__MACRO__CALL_MEMBERN_WITH XEMMAI__MACRO__CONCATENATE(XEMMAI__MACRO__CONCATENATE(t_call_member, XEMMAI__MACRO__N), _with)
#define XEMMAI__MACRO__CALL_MEMBERN XEMMAI__MACRO__CONCATENATE(t_call_member, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__CALL_STATICN_WITH XEMMAI__MACRO__CONCATENATE(XEMMAI__MACRO__CONCATENATE(t_call_static, XEMMAI__MACRO__N), _with)
#define XEMMAI__MACRO__CALL_STATICN XEMMAI__MACRO__CONCATENATE(t_call_static, XEMMAI__MACRO__N)

template<XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_construct<t_transfer (*)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)>
{
	static void f_call(t_transfer (*a_function)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), t_object* a_class, size_t a_n, t_stack& a_stack)
	{
		if (a_n != XEMMAI__MACRO__N) t_throwable::f_throw(L"must be called with " XEMMAI__MACRO__L(XEMMAI__MACRO__N) L" argument(s).");
		XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__POP, XEMMAI__MACRO__N)
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CHECK, XEMMAI__MACRO__N)
		a_stack.f_return(a_function(a_class XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_AN__));
	}
	static void f_call(t_transfer (*a_function)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), t_object* a_class, t_stack& a_stack)
	{
		XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__POP, XEMMAI__MACRO__N)
		a_stack.f_return(a_function(a_class XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_AN__));
	}
	static bool f_match(size_t a_n, t_stack& a_stack)
	{
		if (a_n != XEMMAI__MACRO__N) return false;
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__IS, XEMMAI__MACRO__N)
		return true;
	}
};

template<typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_construct<T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__ XEMMAI__MACRO__COMMA_UNSPECIFIED__, t_unspecified>
{
	typedef t_call_construct<t_transfer (*)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)> t_type;

	static t_transfer f_default(t_object* a_class XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN_A_N__)
	{
		t_transfer object = t_object::f_allocate(a_class);
		object->v_pointer = new T_self(XEMMAI__MACRO__A_N__);
		return object;
	}
	static void f_call(t_object* a_class, size_t a_n, t_stack& a_stack)
	{
		t_type::f_call(f_default, a_class, a_n, a_stack);
	}
	static void f_call(t_object* a_class, t_stack& a_stack)
	{
		t_type::f_call(f_default, a_class, a_stack);
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, bool A_function, bool A_void, bool A_extension>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		return f_extension<T_extension>(a_module)->f_as((f_as<T_self*>(a_self)->*a_function)(XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, false, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		return f_extension<T_extension>(a_module)->f_as((f_as<T_self*>(a_self)->*a_function)(f_extension<T_extension>(a_module) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, true, false>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		(f_as<T_self*>(a_self)->*a_function)(XEMMAI__MACRO__AS_A_N__);
		return f_global()->f_null();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, true, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		(f_as<T_self*>(a_self)->*a_function)(f_extension<T_extension>(a_module) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return f_global()->f_null();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, false, false>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		return f_extension<T_extension>(a_module)->f_as(a_function(f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, false, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		return f_extension<T_extension>(a_module)->f_as(a_function(f_extension<T_extension>(a_module), f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, true, false>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		a_function(f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return f_global()->f_null();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, true, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, t_object* a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		a_function(f_extension<T_extension>(a_module), f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return f_global()->f_null();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN
{
	template<bool A_function, bool A_void, bool A_extension>
	struct t_bind
	{
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
		{
			if (!a_self) t_throwable::f_throw(L"must be called with this.");
			f_check<T_self>(a_self, L"this");
			if (a_n != XEMMAI__MACRO__N) t_throwable::f_throw(L"must be called with " XEMMAI__MACRO__L(XEMMAI__MACRO__N) L" argument(s).");
			XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__POP, XEMMAI__MACRO__N)
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CHECK, XEMMAI__MACRO__N)
			a_stack.f_return(XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_function, A_void, A_extension>::f_call(a_function, a_module, a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, t_object* a_self, t_stack& a_stack)
		{
			XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__POP, XEMMAI__MACRO__N)
			a_stack.f_return(XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_function, A_void, A_extension>::f_call(a_function, a_module, a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
	};

	static bool f_match(t_object* a_self, size_t a_n, t_stack& a_stack)
	{
		if (!a_self) return false;
		if (!f_is<T_self>(a_self)) return false;
		if (a_n != XEMMAI__MACRO__N) return false;
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__IS, XEMMAI__MACRO__N)
		return true;
	}
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_member<T_extension, T_r (T_self::*)(XEMMAI__MACRO__T_AN__) const>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, false> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_member<T_extension, T_r (T_self::*)(T_extension* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__) const>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, true> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_member<T_extension, T_r (T_self::*)(XEMMAI__MACRO__T_AN__)>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, false> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_member<T_extension, T_r (T_self::*)(T_extension* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, true> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_member<T_extension, T_r (*)(T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<true, t_is_void<T_r>::v_value, false> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_member<T_extension, T_r (*)(T_extension*, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<true, t_is_void<T_r>::v_value, true> t_call;
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, bool A_void, bool A_extension>
struct XEMMAI__MACRO__CALL_STATICN_WITH
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, T_extension* a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		return a_extension->f_as(a_function(XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, T_extension* a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		return a_extension->f_as(a_function(a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, false>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, T_extension* a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		a_function(XEMMAI__MACRO__AS_A_N__);
		return f_global()->f_null();
	}
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, T_extension* a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		a_function(a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return f_global()->f_null();
	}
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_STATICN
{
	template<bool A_void, bool A_extension>
	struct t_bind
	{
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, size_t a_n, t_stack& a_stack)
		{
			if (a_n != XEMMAI__MACRO__N) t_throwable::f_throw(L"must be called with " XEMMAI__MACRO__L(XEMMAI__MACRO__N) L" argument(s).");
			XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__POP, XEMMAI__MACRO__N)
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CHECK, XEMMAI__MACRO__N)
			a_stack.f_return(XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_void, A_extension>::f_call(a_function, static_cast<T_extension*>(f_as<t_library*>(a_module)->v_extension) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, t_stack& a_stack)
		{
			XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__POP, XEMMAI__MACRO__N)
			a_stack.f_return(XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_void, A_extension>::f_call(a_function, a_module XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
	};

	static bool f_match(size_t a_n, t_stack& a_stack)
	{
		if (a_n != XEMMAI__MACRO__N) return false;
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__IS, XEMMAI__MACRO__N)
		return true;
	}
};

template<typename T_extension, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_static<T_extension, T_r (*)(XEMMAI__MACRO__T_AN__)>
{
	typedef XEMMAI__MACRO__CALL_STATICN<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<t_is_void<T_r>::v_value, false> t_call;
};

template<typename T_extension, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_static<T_extension, T_r (*)(T_extension* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)>
{
	typedef XEMMAI__MACRO__CALL_STATICN<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<t_is_void<T_r>::v_value, true> t_call;
};

#undef XEMMAI__MACRO__COMMA_IF_N
#undef XEMMAI__MACRO__TYPENAME_T_AN__
#undef XEMMAI__MACRO__T_AN__
#undef XEMMAI__MACRO__T_AN_A_N__
#undef XEMMAI__MACRO__TRANSFER_A_N__
#undef XEMMAI__MACRO__A_N__
#undef XEMMAI__MACRO__AS_A_N__
#undef XEMMAI__MACRO__AN__
#undef XEMMAI__MACRO__AS_AN__
#undef XEMMAI__MACRO__IS
#undef XEMMAI__MACRO__LIMIT_N
#undef XEMMAI__MACRO__COMMA_UNSPECIFIED__
#undef XEMMAI__MACRO__CALL_MEMBERN_WITH
#undef XEMMAI__MACRO__CALL_MEMBERN
#undef XEMMAI__MACRO__CALL_STATICN_WITH
#undef XEMMAI__MACRO__CALL_STATICN
