#pragma once

#include"pch.h"

#include"utilty_ObjectPool.hpp"

class ExpensiveObject
{
public:
	ExpensiveObject() {}
	virtual ~ExpensiveObject() = default;

private:
	std::array<double, 4 * 1024> m_data;
};

using MyPool = rw::ObjectPool<ExpensiveObject>;

std::shared_ptr<ExpensiveObject> getExpensiveObject(MyPool& pool)
{
	auto object{ pool.acquireObject() };
	return object;
}

void processExpensiveObject(ExpensiveObject& object) {}

class TestObject {
public:
	TestObject(int value) : value(value) {}
	int getValue() const { return value; }
private:
	int value;
};