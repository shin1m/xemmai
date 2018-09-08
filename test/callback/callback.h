#ifndef CALLBACK_H
#define CALLBACK_H

#include <vector>
#include <string>

class t_server;

class t_client
{
	friend class t_server;

	t_server* v_server = nullptr;

public:
	virtual ~t_client();
	virtual void f_on_message(std::wstring_view a_message);
	void f_remove();
};

class t_server
{
	friend class t_client;

	std::vector<t_client*> v_clients;

	void f_remove(t_client* a_client);

public:
	~t_server();
	void f_add(t_client& a_client);
	void f_post(std::wstring_view a_message);
	void f_run();
};

#endif
