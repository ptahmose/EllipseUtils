#pragma once

class CUtilities
{
public:
	static int strcmp_caseinsensitive(const char *string1,const char *string2);
};

template<typename T>
class Deleter
{
private:
	T* toDelete;
public:
	Deleter() : toDelete(nullptr) {}
	~Deleter() { delete this->toDelete; /* calling delete on nullptr is documented to do nothing */ }
	void SetToDelete(T* t) { this->toDelete = t; }
};