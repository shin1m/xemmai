#include <cmath>
#ifdef __unix__
#include <sys/time.h>
#endif
#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

double f_now();

}

struct t_time : t_library
{
#ifdef __unix__
	double v_tick_base = f_now();
#endif

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_time, t_global, f_global())

namespace
{

double f_now()
{
#ifdef __unix__
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec / 1000000.0;
#endif
#ifdef _WIN32
	FILETIME utc;
	GetSystemTimeAsFileTime(&utc);
	ULARGE_INTEGER ui;
	ui.LowPart = utc.dwLowDateTime;
	ui.HighPart = utc.dwHighDateTime;
	return (ui.QuadPart - 116444736000000000) / 10000000.0;
#endif
}

inline intptr_t f_item(const t_tuple& a_tuple, size_t a_index)
{
	auto& a = a_tuple[a_index];
	f_check<intptr_t>(a, L"item");
	return f_as<intptr_t>(a);
}

inline double f_item_with_fraction(const t_tuple& a_tuple, size_t a_index)
{
	auto& a = a_tuple[a_index];
	if (f_is<intptr_t>(a)) return f_as<intptr_t>(a);
	f_check<double>(a, L"item");
	return f_as<double>(a);
}

const intptr_t v_epoch_days = 1969 * 365 + 1969 / 4 - 1969 / 100 + 1969 / 400;

const size_t v_month_base_days[] = {
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

intptr_t f_month_name_to_number(const wchar_t* a_name)
{
	switch (a_name[0]) {
	case L'A':
	case L'a':
		switch (a_name[1]) {
		case L'P':
		case L'p':
			switch (a_name[2]) {
			case L'R':
			case L'r':
				return a_name[3] == L'\0' ? 4 : 0;
			default:
				return 0;
			}
		case L'U':
		case L'u':
			switch (a_name[2]) {
			case L'G':
			case L'g':
				return a_name[3] == L'\0' ? 8 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'D':
	case L'd':
		switch (a_name[1]) {
		case L'E':
		case L'e':
			switch (a_name[2]) {
			case L'C':
			case L'c':
				return a_name[3] == L'\0' ? 12 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'F':
	case L'f':
		switch (a_name[1]) {
		case L'E':
		case L'e':
			switch (a_name[2]) {
			case L'B':
			case L'b':
				return a_name[3] == L'\0' ? 2 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'J':
	case L'j':
		switch (a_name[1]) {
		case L'A':
		case L'a':
			switch (a_name[2]) {
			case L'N':
			case L'n':
				return a_name[3] == L'\0' ? 1 : 0;
			default:
				return 0;
			}
		case L'U':
		case L'u':
			switch (a_name[2]) {
			case L'L':
			case L'l':
				return a_name[3] == L'\0' ? 7 : 0;
			case L'N':
			case L'n':
				return a_name[3] == L'\0' ? 6 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'M':
	case L'm':
		switch (a_name[1]) {
		case L'A':
		case L'a':
			switch (a_name[2]) {
			case L'R':
			case L'r':
				return a_name[3] == L'\0' ? 3 : 0;
			case L'Y':
			case L'y':
				return a_name[3] == L'\0' ? 5 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'N':
	case L'n':
		switch (a_name[1]) {
		case L'O':
		case L'o':
			switch (a_name[2]) {
			case L'V':
			case L'v':
				return a_name[3] == L'\0' ? 11 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'O':
	case L'o':
		switch (a_name[1]) {
		case L'C':
		case L'c':
			switch (a_name[2]) {
			case L'T':
			case L't':
				return a_name[3] == L'\0' ? 10 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	case L'S':
	case L's':
		switch (a_name[1]) {
		case L'E':
		case L'e':
			switch (a_name[2]) {
			case L'P':
			case L'p':
				return a_name[3] == L'\0' ? 9 : 0;
			default:
				return 0;
			}
		default:
			return 0;
		}
	default:
		return 0;
	}
}

intptr_t f_zone_to_offset_us(const wchar_t* a_zone, intptr_t a_military, intptr_t a_us)
{
	switch (a_zone[1]) {
	case L'\0':
		return a_military * 3600;
	case L'D':
	case L'd':
		++a_us;
		break;
	case L'S':
	case L's':
		break;
	default:
		return 0;
	}
	switch (a_zone[2]) {
	case L'T':
	case L't':
		return a_zone[3] == L'\0' ? a_us * 3600 : 0;
	default:
		return 0;
	}
}

intptr_t f_zone_to_offset(const wchar_t* a_zone)
{
	switch (a_zone[0]) {
	case L'+':
	case L'-':
		if (std::iswdigit(a_zone[1]) && std::iswdigit(a_zone[2])) {
			size_t i = a_zone[3] == L':' ? 4 : 3;
			if (!std::iswdigit(a_zone[i]) || !std::iswdigit(a_zone[i + 1]) || a_zone[i + 2] != L'\0') return 0;
			intptr_t hour = (a_zone[1] - L'0') * 10 + (a_zone[2] - L'0');
			intptr_t minute = (a_zone[i] - L'0') * 10 + (a_zone[i + 1] - L'0');
			return (a_zone[0] == L'-' ? -60 : 60) * (hour * 60 + minute);
		} else {
			return 0;
		}
	case L'A':
	case L'a':
		return a_zone[1] == L'\0' ? 1 * 3600 : 0;
	case L'B':
	case L'b':
		return a_zone[1] == L'\0' ? 2 * 3600 : 0;
	case L'C':
	case L'c':
		return f_zone_to_offset_us(a_zone, 3, -6);
	case L'D':
	case L'd':
		return a_zone[1] == L'\0' ? 4 * 3600 : 0;
	case L'E':
	case L'e':
		return f_zone_to_offset_us(a_zone, 5, -5);
	case L'F':
	case L'f':
		return a_zone[1] == L'\0' ? 6 * 3600 : 0;
	case L'G':
	case L'g':
		return a_zone[1] == L'\0' ? 7 * 3600 : 0;
	case L'H':
	case L'h':
		return a_zone[1] == L'\0' ? 8 * 3600 : 0;
	case L'I':
	case L'i':
		return a_zone[1] == L'\0' ? 9 * 3600 : 0;
	case L'K':
	case L'k':
		return a_zone[1] == L'\0' ? 10 * 3600 : 0;
	case L'L':
	case L'l':
		return a_zone[1] == L'\0' ? 11 * 3600 : 0;
	case L'M':
	case L'm':
		return f_zone_to_offset_us(a_zone, 12, -7);
	case L'N':
	case L'n':
		return a_zone[1] == L'\0' ? -1 * 3600 : 0;
	case L'O':
	case L'o':
		return a_zone[1] == L'\0' ? -2 * 3600 : 0;
	case L'P':
	case L'p':
		return f_zone_to_offset_us(a_zone, -3, -8);
	case L'Q':
	case L'q':
		return a_zone[1] == L'\0' ? -4 * 3600 : 0;
	case L'R':
	case L'r':
		return a_zone[1] == L'\0' ? -5 * 3600 : 0;
	case L'S':
	case L's':
		return a_zone[1] == L'\0' ? -6 * 3600 : 0;
	case L'T':
	case L't':
		return a_zone[1] == L'\0' ? -7 * 3600 : 0;
	case L'U':
	case L'u':
		return a_zone[1] == L'\0' ? -8 * 3600 : 0;
	case L'V':
	case L'v':
		return a_zone[1] == L'\0' ? -9 * 3600 : 0;
	case L'W':
	case L'w':
		return a_zone[1] == L'\0' ? -10 * 3600 : 0;
	case L'X':
	case L'x':
		return a_zone[1] == L'\0' ? -11 * 3600 : 0;
	case L'Y':
	case L'y':
		return a_zone[1] == L'\0' ? -12 * 3600 : 0;
	default:
		return 0;
	}
}

const wchar_t* v_rfc2822_days[] = {
	L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"
};

const wchar_t* v_rfc2822_months[] = {
	L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
	L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"
};

}

void t_time::f_scan(t_scan a_scan)
{
}

std::vector<std::pair<t_root, t_rvalue>> t_time::f_define()
{
	return t_define(this)
	(L"now"sv, t_static<double(*)(), f_now>())
	(L"tick"sv, t_static<intptr_t(*)(t_time*), [](auto a_library) -> intptr_t
	{
#ifdef __unix__
		return (f_now() - a_library->v_tick_base) * 1000.0;
#endif
#ifdef _WIN32
		return GetTickCount();
#endif
	}>())
	(L"compose"sv, t_static<double(*)(const t_tuple&), [](auto a_value)
	{
		size_t n = a_value.f_size();
		if (n < 3) f_throw(L"must have at least 3 items."sv);
		intptr_t year = f_item(a_value, 0);
		intptr_t month = f_item(a_value, 1) - 1;
		year += month / 12;
		month %= 12;
		if (month < 0) {
			--year;
			month += 12;
		}
		intptr_t days = v_month_base_days[month];
		if ((month > 1) && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ++days;
		--year;
		days += year * 365 + year / 4 - year / 100 + year / 400;
		double t = days + f_item(a_value, 2) - 1 - v_epoch_days;
		t *= 24.0;
		if (n >= 4) t += f_item(a_value, 3);
		t *= 60.0;
		if (n >= 5) t += f_item(a_value, 4);
		t *= 60.0;
		if (n >= 6) t += f_item_with_fraction(a_value, 5);
		return t;
	}>())
	(L"decompose"sv, t_static<t_object*(*)(double), [](auto a_value)
	{
		auto t0 = static_cast<std::time_t>(std::floor(a_value));
		double fraction = a_value - t0;
		std::tm* t1 = std::gmtime(&t0);
		return f_tuple(t1->tm_year + 1900, t1->tm_mon + 1, t1->tm_mday, t1->tm_hour, t1->tm_min, t1->tm_sec + fraction, t1->tm_wday, t1->tm_yday + 1);
	}>())
	(L"offset"sv, t_static<intptr_t(*)(), []
	{
		tzset();
#ifdef __unix__
		return -timezone;
#endif
#ifdef _WIN32
		return -_timezone;
#endif
	}>())
	(L"parse_rfc2822"sv, t_static<t_object*(*)(std::wstring), [](auto a_value)
	{
		const wchar_t* s = a_value.c_str();
		intptr_t day;
		wchar_t month[4];
		intptr_t year;
		intptr_t hour;
		intptr_t minute;
		int i;
		int n = std::swscanf(s, XEMMAI__MACRO__L("%*3ls, %2" SCNdPTR " %3ls %4" SCNdPTR " %2" SCNdPTR ":%2" SCNdPTR "%n"), &day, month, &year, &hour, &minute, &i);
		if (n < 5) f_throw(L"invalid format."sv);
		intptr_t m = f_month_name_to_number(month);
		if (m <= 0) f_throw(L"invalid format."sv);
		intptr_t second;
		wchar_t zone[6];
		if (s[i] == L':') {
			n = std::swscanf(s + ++i, XEMMAI__MACRO__L("%2" SCNdPTR " %5ls"), &second, zone);
			if (n < 2) f_throw(L"invalid format."sv);
		} else {
			second = 0;
			n = std::swscanf(s + i, L" %5ls", zone);
			if (n < 1) f_throw(L"invalid format."sv);
		}
		if (year < 50)
			year += 2000;
		else if (year < 1000)
			year += 1900;
		return f_tuple(year, m, day, hour, minute, second, f_zone_to_offset(zone));
	}>())
	(L"format_rfc2822"sv, t_static<t_object*(*)(const t_tuple&, intptr_t), [](auto a_value, auto a_offset)
	{
		size_t n = a_value.f_size();
		if (n < 7) f_throw(L"must have at least 7 items."sv);
		intptr_t year = f_item(a_value, 0);
		intptr_t month = f_item(a_value, 1);
		intptr_t day = f_item(a_value, 2);
		intptr_t hour = f_item(a_value, 3);
		intptr_t minute = f_item(a_value, 4);
		intptr_t second = static_cast<intptr_t>(std::floor(f_item_with_fraction(a_value, 5)));
		intptr_t week = f_item(a_value, 6);
		wchar_t sign = (a_offset > 0) ? L'+' : L'-';
		a_offset = std::abs(a_offset) / 60;
		wchar_t cs[32];
		n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), XEMMAI__MACRO__L("%ls, %" PRIdPTR " %ls %04" PRIdPTR " %02" PRIdPTR ":%02" PRIdPTR ":%02" PRIdPTR " %lc%02" PRIdPTR "%02" PRIdPTR), v_rfc2822_days[week], day, v_rfc2822_months[month - 1], year, hour, minute, second, sign, a_offset / 60, a_offset % 60);
		return t_string::f_instantiate(cs, n);
	}>())
	(L"parse_http"sv, t_static<t_object*(*)(std::wstring), [](auto a_value)
	{
		intptr_t day;
		wchar_t month[4];
		intptr_t year;
		intptr_t hour;
		intptr_t minute;
		intptr_t second;
		int n = std::swscanf(a_value.c_str(), XEMMAI__MACRO__L("%*3ls, %2" SCNdPTR " %3ls %4" SCNdPTR " %2" SCNdPTR ":%2" SCNdPTR ":%2" SCNdPTR " GMT"), &day, month, &year, &hour, &minute, &second);
		if (n < 6) {
			n = std::swscanf(a_value.c_str(), XEMMAI__MACRO__L("%*l[A-Za-z], %2" SCNdPTR "-%3ls-%2" SCNdPTR " %2" SCNdPTR ":%2" SCNdPTR ":%2" SCNdPTR " GMT"), &day, month, &year, &hour, &minute, &second);
			if (n < 6) {
				n = std::swscanf(a_value.c_str(), XEMMAI__MACRO__L("%*3ls %3ls %2" SCNdPTR " %2" SCNdPTR ":%2" SCNdPTR ":%2" SCNdPTR " %4" SCNdPTR), month, &day, &hour, &minute, &second, &year);
				if (n < 6) f_throw(L"invalid format."sv);
			}
		}
		intptr_t m = f_month_name_to_number(month);
		if (m <= 0) f_throw(L"invalid format."sv);
		if (year < 50)
			year += 2000;
		else if (year < 1000)
			year += 1900;
		return f_tuple(year, m, day, hour, minute, second);
	}>())
	(L"format_http"sv, t_static<t_object*(*)(const t_tuple&), [](auto a_value)
	{
		if (a_value.f_size() < 7) f_throw(L"must have at least 7 items."sv);
		intptr_t year = f_item(a_value, 0);
		intptr_t month = f_item(a_value, 1);
		intptr_t day = f_item(a_value, 2);
		intptr_t hour = f_item(a_value, 3);
		intptr_t minute = f_item(a_value, 4);
		intptr_t second = static_cast<intptr_t>(std::floor(f_item_with_fraction(a_value, 5)));
		intptr_t week = f_item(a_value, 6);
		wchar_t cs[30];
		size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), XEMMAI__MACRO__L("%ls, %02" PRIdPTR " %ls %04" PRIdPTR " %02" PRIdPTR ":%02" PRIdPTR ":%02" PRIdPTR " GMT"), v_rfc2822_days[week], day, v_rfc2822_months[month - 1], year, hour, minute, second);
		return t_string::f_instantiate(cs, n);
	}>())
	(L"parse_xsd"sv, t_static<t_object*(*)(std::wstring), [](auto a_value)
	{
		intptr_t year;
		intptr_t month;
		intptr_t day;
		intptr_t hour;
		intptr_t minute;
		double second;
		wchar_t zone[7];
		int n = std::swscanf(a_value.c_str(), XEMMAI__MACRO__L("%5" SCNdPTR "-%2" SCNdPTR "-%2" SCNdPTR "T%2" SCNdPTR ":%2" SCNdPTR ":%lf%6ls"), &year, &month, &day, &hour, &minute, &second, zone);
		if (n < 6) f_throw(L"invalid format."sv);
		return n < 7 ? f_tuple(year, month, day, hour, minute, second) : f_tuple(year, month, day, hour, minute, second, f_zone_to_offset(zone));
	}>())
	(L"format_xsd"sv, t_static<t_object*(*)(const t_tuple&, intptr_t, intptr_t), [](auto a_value, auto a_offset, auto a_precision)
	{
		if (a_value.f_size() < 6) f_throw(L"must have at least 6 items."sv);
		intptr_t year = f_item(a_value, 0);
		intptr_t month = f_item(a_value, 1);
		intptr_t day = f_item(a_value, 2);
		intptr_t hour = f_item(a_value, 3);
		intptr_t minute = f_item(a_value, 4);
		double second = f_item_with_fraction(a_value, 5);
		wchar_t cs[30];
		size_t n;
		if (a_offset == 0) {
			n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), XEMMAI__MACRO__L("%04" PRIdPTR "-%02" PRIdPTR "-%02" PRIdPTR "T%02" PRIdPTR ":%02" PRIdPTR ":%02.*fZ"), year, month, day, hour, minute, a_precision, second);
		} else {
			wchar_t sign = (a_offset > 0) ? L'+' : L'-';
			a_offset = std::abs(a_offset) / 60;
			n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), XEMMAI__MACRO__L("%04" PRIdPTR "-%02" PRIdPTR "-%02" PRIdPTR "T%02" PRIdPTR ":%02" PRIdPTR ":%02.*f%lc%02" PRIdPTR ":%02" PRIdPTR), year, month, day, hour, minute, a_precision, second, sign, a_offset / 60, a_offset % 60);
		}
		return t_string::f_instantiate(cs, n);
	}>())
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmai::t_time>(a_handle);
}
