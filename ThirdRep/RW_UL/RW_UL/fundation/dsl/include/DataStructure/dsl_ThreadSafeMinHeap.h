#pragma once
#include <vector>
#include <shared_mutex>
#include <algorithm>
#include <optional>

class ThreadSafeMinHeap {
public:
	// 构造函数，允许预设容量
	explicit ThreadSafeMinHeap(size_t initialCapacity = 50) {
		heap_.reserve(initialCapacity);
	}

	// 插入元素
	void push(float value) {
		std::unique_lock<std::shared_mutex> lock(mutex_);
		heap_.push_back(value);
		std::push_heap(heap_.begin(), heap_.end(), std::greater<>()); // 保持最小堆性质
	}

	// 获取最小值（无锁读取）
	bool tryGetMin(float& minValue) {
		std::shared_lock<std::shared_mutex> lock(mutex_);
		if (heap_.empty()) {
			return false; // 堆为空
		}
		minValue = heap_.front(); // 堆顶即为最小值
		return true;
	}

	// 删除最小值
	bool tryPopMin(float& minValue) {
		std::unique_lock<std::shared_mutex> lock(mutex_);
		if (heap_.empty()) {
			return false; // 堆为空
		}
		minValue = heap_.front(); // 堆顶即为最小值
		std::pop_heap(heap_.begin(), heap_.end(), std::greater<>()); // 移动最小值到末尾
		heap_.pop_back(); // 删除末尾元素
		return true;
	}

	// 获取当前存储的元素总数
	size_t size() const {
		std::shared_lock<std::shared_mutex> lock(mutex_);
		return heap_.size();
	}

private:
	std::vector<float> heap_; // 底层容器，存储最小堆
	mutable std::shared_mutex mutex_; // 读写锁
};