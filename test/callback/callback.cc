#include "callback.h"
#include <algorithm>

t_client::~t_client()
{
	f_remove();
}

void t_client::f_on_message(std::wstring_view a_message)
{
	std::wprintf(L"t_client::f_on_message(%.*ls);\n", a_message.size(), a_message.data());
}

void t_client::f_remove()
{
	if (!v_server) return;
	v_server->f_remove(this);
	v_server = nullptr;
}

void t_server::f_remove(t_client* a_client)
{
	auto i = std::find(v_clients.begin(), v_clients.end(), a_client);
	if (i != v_clients.end()) v_clients.erase(i);
}

t_server::~t_server()
{
	while (!v_clients.empty()) v_clients.back()->f_remove();
}

void t_server::f_add(t_client& a_client)
{
	if (a_client.v_server) a_client.f_remove();
	v_clients.push_back(&a_client);
	a_client.v_server = this;
}

void t_server::f_post(std::wstring_view a_message)
{
	for (auto p : v_clients) p->f_on_message(a_message);
}

void t_server::f_run()
{
	wchar_t cs[256];
	while (true) {
		wchar_t* p = std::fgetws(cs, sizeof(cs) / sizeof(wchar_t), stdin);
		if (!p) break;
		std::wstring_view s = p;
		f_post(s.substr(0, s.size() - 1));
	}
}
