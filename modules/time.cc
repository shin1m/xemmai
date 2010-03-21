#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cwctype>
#include <sys/time.h>
#include <xemmai/convert.h>
#include <xemmai/array.h>

namespace xemmai
{

class t_time : public t_extension
{
public:
	t_time(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(T a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

namespace
{

double f_now()
{
#ifdef __unix__
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec * 0.000001;
#endif
#ifdef _WIN32
	std::time_t t;
	std::time(&t);
	return t;
#endif
}

inline int f_item(const t_array& a_array, size_t a_index)
{
	const t_slot& a = a_array[a_index];
	f_check<int>(a, L"item");
	return f_as<int>(a);
}

inline double f_item_with_fraction(const t_array& a_array, size_t a_index)
{
	const t_slot& a = a_array[a_index];
	if (f_is<int>(a)) return f_as<int>(a);
	f_check<double>(a, L"item");
	return f_as<double>(a);
}

const int v_epoch_days = 1969 * 365 + 1969 / 4 - 1969 / 100 + 1969 / 400;

const size_t v_month_base_days[] = {
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

double f_compose(const t_array& a_value)
{
	size_t n = a_value.f_size();
	if (n < 3) t_throwable::f_throw(L"must have at least 3 items.");
	int year = f_item(a_value, 0);
	int month = f_item(a_value, 1) - 1;
	year += month / 12;
	month %= 12;
	if (month < 0) {
		--year;
		month += 12;
	}
	int days = v_month_base_days[month];
	if (month > 1 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ++days;
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
}

t_transfer f_decompose(double a_value)
{
	std::time_t t0 = static_cast<std::time_t>(std::floor(a_value));
	double fraction = a_value - t0;
	std::tm* t1 = std::gmtime(&t0);
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	array.f_push(f_global()->f_as(t1->tm_year + 1900));
	array.f_push(f_global()->f_as(t1->tm_mon + 1));
	array.f_push(f_global()->f_as(t1->tm_mday));
	array.f_push(f_global()->f_as(t1->tm_hour));
	array.f_push(f_global()->f_as(t1->tm_min));
	array.f_push(f_global()->f_as(t1->tm_sec + fraction));
	array.f_push(f_global()->f_as(t1->tm_wday));
	array.f_push(f_global()->f_as(t1->tm_yday + 1));
	return p;
}

int f_offset()
{
	tzset();
	return -timezone;
}

int f_month_name_to_number(const wchar_t* a_name)
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

int f_zone_to_offset_us(const wchar_t* a_zone, int a_military, int a_us)
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

int f_zone_to_offset(const wchar_t* a_zone)
{
	switch (a_zone[0]) {
	case L'+':
	case L'-':
		if (std::iswdigit(a_zone[1]) && std::iswdigit(a_zone[2])) {
			size_t i = a_zone[3] == L':' ? 4 : 3;
			if (!std::iswdigit(a_zone[i]) || !std::iswdigit(a_zone[i + 1]) || a_zone[i + 2] != L'\0') return 0;
			int hour = (a_zone[1] - L'0') * 10 + (a_zone[2] - L'0');
			int minute = (a_zone[i] - L'0') * 10 + (a_zone[i + 1] - L'0');
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

t_transfer f_parse_rfc2822(const std::wstring& a_value)
{
	const wchar_t* s = a_value.c_str();
	int day;
	wchar_t month[4];
	int year;
	int hour;
	int minute;
	int i;
	int n = std::swscanf(s, L"%*3ls, %2d %3ls %4d %2d:%2d%n", &day, month, &year, &hour, &minute, &i);
	if (n < 5) t_throwable::f_throw(L"invalid format.");
	int m = f_month_name_to_number(month);
	if (m <= 0) t_throwable::f_throw(L"invalid format.");
	int second;
	wchar_t zone[6];
	if (s[i] == L':') {
		n = std::swscanf(s + ++i, L"%2d %5ls", &second, zone);
		if (n < 2) t_throwable::f_throw(L"invalid format.");
	} else {
		second = 0;
		n = std::swscanf(s + i, L" %5ls", zone);
		if (n < 1) t_throwable::f_throw(L"invalid format.");
	}
	if (year < 50)
		year += 2000;
	else if (year < 1000)
		year += 1900;
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	array.f_push(f_global()->f_as(year));
	array.f_push(f_global()->f_as(m));
	array.f_push(f_global()->f_as(day));
	array.f_push(f_global()->f_as(hour));
	array.f_push(f_global()->f_as(minute));
	array.f_push(f_global()->f_as(second));
	array.f_push(f_global()->f_as(f_zone_to_offset(zone)));
	return p;
}

std::wstring f_format_rfc2822(const t_array& a_value, int a_offset)
{
	size_t n = a_value.f_size();
	if (n < 7) t_throwable::f_throw(L"must have at least 7 items.");
	int year = f_item(a_value, 0);
	int month = f_item(a_value, 1);
	int day = f_item(a_value, 2);
	int hour = f_item(a_value, 3);
	int minute = f_item(a_value, 4);
	int second = static_cast<int>(std::floor(f_item_with_fraction(a_value, 5)));
	int week = f_item(a_value, 6);
	wchar_t sign = a_offset > 0 ? L'+' : L'-';
	a_offset = std::abs(a_offset) / 60;
	wchar_t cs[32];
	std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%ls, %d %ls %04d %02d:%02d:%02d %lc%02d%02d", v_rfc2822_days[week], day, v_rfc2822_months[month - 1], year, hour, minute, second, sign, a_offset / 60, a_offset % 60);
	return cs;
}

t_transfer f_parse_http(const std::wstring& a_value)
{
	int day;
	wchar_t month[4];
	int year;
	int hour;
	int minute;
	int second;
	int n = std::swscanf(a_value.c_str(), L"%*3ls, %2d %3ls %4d %2d:%2d:%2d GMT", &day, month, &year, &hour, &minute, &second);
	if (n < 6) {
		n = std::swscanf(a_value.c_str(), L"%*l[A-Za-z], %2d-%3ls-%2d %2d:%2d:%2d GMT", &day, month, &year, &hour, &minute, &second);
		if (n < 6) {
			n = std::swscanf(a_value.c_str(), L"%*3ls %3ls %2d %2d:%2d:%2d %4d", month, &day, &hour, &minute, &second, &year);
			if (n < 6) t_throwable::f_throw(L"invalid format.");
		}
	}
	int m = f_month_name_to_number(month);
	if (m <= 0) t_throwable::f_throw(L"invalid format.");
	if (year < 50)
		year += 2000;
	else if (year < 1000)
		year += 1900;
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	array.f_push(f_global()->f_as(year));
	array.f_push(f_global()->f_as(m));
	array.f_push(f_global()->f_as(day));
	array.f_push(f_global()->f_as(hour));
	array.f_push(f_global()->f_as(minute));
	array.f_push(f_global()->f_as(second));
	return p;
}

std::wstring f_format_http(const t_array& a_value)
{
	if (a_value.f_size() < 7) t_throwable::f_throw(L"must have at least 7 items.");
	int year = f_item(a_value, 0);
	int month = f_item(a_value, 1);
	int day = f_item(a_value, 2);
	int hour = f_item(a_value, 3);
	int minute = f_item(a_value, 4);
	int second = static_cast<int>(std::floor(f_item_with_fraction(a_value, 5)));
	int week = f_item(a_value, 6);
	wchar_t cs[30];
	std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%ls, %02d %ls %04d %02d:%02d:%02d GMT", v_rfc2822_days[week], day, v_rfc2822_months[month - 1], year, hour, minute, second);
	return cs;
}

t_transfer f_parse_xsd(const std::wstring& a_value)
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	double second;
	wchar_t zone[7];
	int n = std::swscanf(a_value.c_str(), L"%5d-%2d-%2dT%2d:%2d:%lf%6ls", &year, &month, &day, &hour, &minute, &second, zone);
	if (n < 6) t_throwable::f_throw(L"invalid format.");
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	array.f_push(f_global()->f_as(year));
	array.f_push(f_global()->f_as(month));
	array.f_push(f_global()->f_as(day));
	array.f_push(f_global()->f_as(hour));
	array.f_push(f_global()->f_as(minute));
	array.f_push(f_global()->f_as(second));
	if (n >= 7) array.f_push(f_global()->f_as(f_zone_to_offset(zone)));
	return p;
}

std::wstring f_format_xsd(const t_array& a_value, int a_offset, int a_precision)
{
	if (a_value.f_size() < 6) t_throwable::f_throw(L"must have at least 6 items.");
	int year = f_item(a_value, 0);
	int month = f_item(a_value, 1);
	int day = f_item(a_value, 2);
	int hour = f_item(a_value, 3);
	int minute = f_item(a_value, 4);
	double second = f_item_with_fraction(a_value, 5);
	wchar_t cs[30];
	if (a_offset == 0) {
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%04d-%02d-%02dT%02d:%02d:%02.*fZ", year, month, day, hour, minute, a_precision, second);
	} else {
		wchar_t sign = a_offset > 0 ? L'+' : L'-';
		a_offset = std::abs(a_offset) / 60;
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%04d-%02d-%02dT%02d:%02d:%02.*f%lc%02d:%02d", year, month, day, hour, minute, a_precision, second, sign, a_offset / 60, a_offset % 60);
	}
	return cs;
}

}

t_time::t_time(t_object* a_module) : t_extension(a_module)
{
	f_define<double (*)(), f_now>(this, L"now");
	f_define<double (*)(const t_array&), f_compose>(this, L"compose");
	f_define<t_transfer (*)(double), f_decompose>(this, L"decompose");
	f_define<int (*)(), f_offset>(this, L"offset");
	f_define<t_transfer (*)(const std::wstring&), f_parse_rfc2822>(this, L"parse_rfc2822");
	f_define<std::wstring (*)(const t_array&, int), f_format_rfc2822>(this, L"format_rfc2822");
	f_define<t_transfer (*)(const std::wstring&), f_parse_http>(this, L"parse_http");
	f_define<std::wstring (*)(const t_array&), f_format_http>(this, L"format_http");
	f_define<t_transfer (*)(const std::wstring&), f_parse_xsd>(this, L"parse_xsd");
	f_define<std::wstring (*)(const t_array&, int, int), f_format_xsd>(this, L"format_xsd");
}

void t_time::f_scan(t_scan a_scan)
{
}

}

#ifdef _MSC_VER
extern "C" __declspec(dllexport) xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#else
extern "C" xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#endif
{
	return new xemmai::t_time(a_module);
}