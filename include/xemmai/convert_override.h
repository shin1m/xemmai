#if XEMMAI__MACRO__N > 0

#define XEMMAI__MACRO__TYPENAME_T_OVERLOADN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TYPENAME_T_OVERLOADN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__CONST_T_OVERLOADN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__CONST_T_OVERLOADN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__CONST_T_OVERLOADN_A_OVERLOADN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__CONST_T_OVERLOADN_A_OVERLOADN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__A_OVERLOADN__ XEMMAI__MACRO__JOIN(XEMMAI__MACRO__A_OVERLOADN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__T_OVERLOADN_OPEN__ XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__T_OVERLOADN_OPEN, XEMMAI__MACRO__N)
#define XEMMAI__MACRO__T_OVERLOADN_CLOSE__ XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__T_OVERLOADN_CLOSE, XEMMAI__MACRO__N)

	template<typename T_function, T_function A_function, typename T_with, XEMMAI__MACRO__TYPENAME_T_OVERLOADN__>
	t_define& operator()(t_object* a_name, const t_member<T_function, A_function, T_with>&, XEMMAI__MACRO__CONST_T_OVERLOADN__)
	{
		v_type.f_put(a_name, v_extension->f_function(
			t_overload<t_member<T_function, A_function, T_with>
			XEMMAI__MACRO__T_OVERLOADN_OPEN__
			XEMMAI__MACRO__T_OVERLOADN_CLOSE__
		>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, typename T_with, XEMMAI__MACRO__TYPENAME_T_OVERLOADN__>
	t_define& operator()(const std::wstring& a_name, const t_member<T_function, A_function, T_with>& a_member0, XEMMAI__MACRO__CONST_T_OVERLOADN_A_OVERLOADN__)
	{
		return (*this)(&*t_symbol::f_instantiate(a_name), a_member0, XEMMAI__MACRO__A_OVERLOADN__);
	}
	template<typename T_function, T_function A_function, XEMMAI__MACRO__TYPENAME_T_OVERLOADN__>
	t_define& operator()(t_object* a_name, const t_static<T_function, A_function>&, XEMMAI__MACRO__CONST_T_OVERLOADN__)
	{
		v_type.f_put(a_name, v_extension->f_function(
			t_overload<t_static<T_function, A_function>
			XEMMAI__MACRO__T_OVERLOADN_OPEN__
			XEMMAI__MACRO__T_OVERLOADN_CLOSE__
		>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, XEMMAI__MACRO__TYPENAME_T_OVERLOADN__>
	t_define& operator()(const std::wstring& a_name, const t_static<T_function, A_function>& a_static0, XEMMAI__MACRO__CONST_T_OVERLOADN_A_OVERLOADN__)
	{
		return (*this)(&*t_symbol::f_instantiate(a_name), a_static0, XEMMAI__MACRO__A_OVERLOADN__);
	}

#undef XEMMAI__MACRO__TYPENAME_T_OVERLOADN__
#undef XEMMAI__MACRO__CONST_T_OVERLOADN__
#undef XEMMAI__MACRO__CONST_T_OVERLOADN_A_OVERLOADN__
#undef XEMMAI__MACRO__A_OVERLOADN__
#undef XEMMAI__MACRO__T_OVERLOADN_OPEN__
#undef XEMMAI__MACRO__T_OVERLOADN_CLOSE__

#endif
