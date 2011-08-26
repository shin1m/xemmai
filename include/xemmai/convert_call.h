#define XEMMAI__MACRO__TYPENAME_T_AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TYPENAME_T_AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__COMMA_IF(XEMMAI__MACRO__N)
#define XEMMAI__MACRO__T_AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__T_AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__AS_STACK__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__AS_STACK, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__LIMIT_N XEMMAI__MACRO__SUBTRACT(XEMMAI__MACRO__ARGUMENTS_LIMIT, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__COMMA_UNSPECIFIED__ XEMMAI__MACRO__COMMA_IF(XEMMAI__MACRO__LIMIT_N) XEMMAI__MACRO__JOIN(XEMMAI__MACRO__UNSPECIFIED, XEMMAI__MACRO__LIMIT_N)
#define XEMMAI__MACRO__T_AN_A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__T_AN_A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__TRANSFER_A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TRANSFER_A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__AS_A_N__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__AS_A_N, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__CALL_MEMBERN_WITH XEMMAI__MACRO__CONCATENATE(XEMMAI__MACRO__CONCATENATE(t_call_member, XEMMAI__MACRO__N), _with)
#define XEMMAI__MACRO__CALL_MEMBERN XEMMAI__MACRO__CONCATENATE(t_call_member, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__AN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__AN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__CALL_STATICN_WITH XEMMAI__MACRO__CONCATENATE(XEMMAI__MACRO__CONCATENATE(t_call_static, XEMMAI__MACRO__N), _with)
#define XEMMAI__MACRO__CALL_STATICN XEMMAI__MACRO__CONCATENATE(t_call_static, XEMMAI__MACRO__N)

template<XEMMAI__MACRO__TYPENAME_T_AN__>
struct t_call_construct<t_transfer (*)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__)>
{
	static t_transfer f_call(t_transfer (*a_function)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), t_object* a_class, t_slot* a_stack, size_t a_n)
	{
		if (a_n != XEMMAI__MACRO__N) t_throwable::f_throw(L"must be called with " XEMMAI__MACRO__L(XEMMAI__MACRO__N) L" argument(s).");
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CHECK_STACK, XEMMAI__MACRO__N)
		return a_function(a_class XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_STACK__);
	}
	static t_transfer f_call(t_transfer (*a_function)(t_object* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), t_object* a_class, t_slot* a_stack)
	{
		return a_function(a_class XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_STACK__);
	}
	static bool f_match(t_slot* a_stack, size_t a_n)
	{
		if (a_n != XEMMAI__MACRO__N) return false;
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__IS_STACK, XEMMAI__MACRO__N)
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
		object.f_pointer__(new T_self(XEMMAI__MACRO__A_N__));
		return object;
	}
	static t_transfer f_call(t_object* a_class, t_slot* a_stack, size_t a_n)
	{
		return t_type::f_call(f_default, a_class, a_stack, a_n);
	}
	static t_transfer f_call(t_object* a_class, t_slot* a_stack)
	{
		return t_type::f_call(f_default, a_class, a_stack);
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, bool A_function, bool A_void, bool A_extension, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		return f_extension<T_extension>(a_module)->f_as((f_as<T_self&>(a_self).*a_function)(XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, false, true, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		return f_extension<T_extension>(a_module)->f_as((f_as<T_self&>(a_self).*a_function)(f_extension<T_extension>(a_module) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, true, false, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		(f_as<T_self&>(a_self).*a_function)(XEMMAI__MACRO__AS_A_N__);
		return t_transfer();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, false, true, true, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		(f_as<T_self&>(a_self).*a_function)(f_extension<T_extension>(a_module) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return t_transfer();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, false, false, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		return f_extension<T_extension>(a_module)->f_as(a_function(f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, false, true, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		return f_extension<T_extension>(a_module)->f_as(a_function(f_extension<T_extension>(a_module), f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__));
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, true, false, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		a_function(f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return t_transfer();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, true, true, T_with>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, t_object* a_module, const t_value& a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		T_with with(a_self);
		a_function(f_extension<T_extension>(a_module), f_as<T_self>(a_self) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return t_transfer();
	}
};

template<typename T_extension, typename T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_MEMBERN
{
	template<bool A_function, bool A_void, bool A_extension, typename T_with>
	struct t_bind
	{
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			f_check<T_self>(a_self, L"this");
			if (a_n != XEMMAI__MACRO__N) t_throwable::f_throw(L"must be called with " XEMMAI__MACRO__L(XEMMAI__MACRO__N) L" argument(s).");
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__STACK_TRANSFER, XEMMAI__MACRO__N)
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CHECK_AN, XEMMAI__MACRO__N)
			a_stack[0].f_construct(XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_function, A_void, A_extension, T_with>::f_call(a_function, a_module, a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, const t_value& a_self, t_slot* a_stack)
		{
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__STACK_TRANSFER, XEMMAI__MACRO__N)
			a_stack[0].f_construct(XEMMAI__MACRO__CALL_MEMBERN_WITH<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_function, A_void, A_extension, T_with>::f_call(a_function, a_module, a_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
	};

	static bool f_match(const t_value& a_self, t_slot* a_stack, size_t a_n)
	{
		if (!f_is<T_self>(a_self)) return false;
		if (a_n != XEMMAI__MACRO__N) return false;
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__IS_STACK, XEMMAI__MACRO__N)
		return true;
	}
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct t_call_member<T_extension, T_r (T_self::*)(XEMMAI__MACRO__T_AN__) const, T_with>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, false, T_with> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct t_call_member<T_extension, T_r (T_self::*)(T_extension* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__) const, T_with>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, true, T_with> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct t_call_member<T_extension, T_r (T_self::*)(XEMMAI__MACRO__T_AN__), T_with>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, false, T_with> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct t_call_member<T_extension, T_r (T_self::*)(T_extension* XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), T_with>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<false, t_is_void<T_r>::v_value, true, T_with> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct t_call_member<T_extension, T_r (*)(T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), T_with>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<true, t_is_void<T_r>::v_value, false, T_with> t_call;
};

template<typename T_extension, typename T_self, typename T_r XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__, typename T_with>
struct t_call_member<T_extension, T_r (*)(T_extension*, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__), T_with>
{
	typedef XEMMAI__MACRO__CALL_MEMBERN<T_extension, T_self XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__> t_type;
	typedef typename t_type::template t_bind<true, t_is_void<T_r>::v_value, true, T_with> t_call;
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
		return t_transfer();
	}
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, true, true>
{
	template<typename T_function>
	static t_transfer f_call(T_function a_function, T_extension* a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TRANSFER_A_N__)
	{
		a_function(a_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AS_A_N__);
		return t_transfer();
	}
};

template<typename T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__TYPENAME_T_AN__>
struct XEMMAI__MACRO__CALL_STATICN
{
	template<bool A_void, bool A_extension>
	struct t_bind
	{
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, t_slot* a_stack, size_t a_n)
		{
			if (a_n != XEMMAI__MACRO__N) t_throwable::f_throw(L"must be called with " XEMMAI__MACRO__L(XEMMAI__MACRO__N) L" argument(s).");
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__STACK_TRANSFER, XEMMAI__MACRO__N)
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CHECK_AN, XEMMAI__MACRO__N)
			a_stack[0].f_construct(XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_void, A_extension>::f_call(a_function, f_extension<T_extension>(a_module) XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
		template<typename T_function>
		static void f_call(T_function a_function, t_object* a_module, t_slot* a_stack)
		{
			XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__STACK_TRANSFER, XEMMAI__MACRO__N)
			a_stack[0].f_construct(XEMMAI__MACRO__CALL_STATICN_WITH<T_extension XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__T_AN__, A_void, A_extension>::f_call(a_function, a_module XEMMAI__MACRO__COMMA_IF_N XEMMAI__MACRO__AN__));
		}
	};

	static bool f_match(t_slot* a_stack, size_t a_n)
	{
		if (a_n != XEMMAI__MACRO__N) return false;
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__IS_STACK, XEMMAI__MACRO__N)
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

#undef XEMMAI__MACRO__TYPENAME_T_AN__
#undef XEMMAI__MACRO__COMMA_IF_N
#undef XEMMAI__MACRO__T_AN__
#undef XEMMAI__MACRO__AS_STACK__
#undef XEMMAI__MACRO__LIMIT_N
#undef XEMMAI__MACRO__COMMA_UNSPECIFIED__
#undef XEMMAI__MACRO__T_AN_A_N__
#undef XEMMAI__MACRO__A_N__
#undef XEMMAI__MACRO__TRANSFER_A_N__
#undef XEMMAI__MACRO__AS_A_N__
#undef XEMMAI__MACRO__CALL_MEMBERN_WITH
#undef XEMMAI__MACRO__CALL_MEMBERN
#undef XEMMAI__MACRO__AN__
#undef XEMMAI__MACRO__CALL_STATICN_WITH
#undef XEMMAI__MACRO__CALL_STATICN
