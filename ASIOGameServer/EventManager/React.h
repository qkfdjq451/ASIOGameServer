#pragma once
#include "../Global.h"

template<typename T>
class React
{
public:
	T Get() { return data; }
	void Set(const T& _data) { data = _data; }

	React(T _data) : data(_data) {}
private:
	T data;
	


};