#pragma once

#include <deque>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include<chrono>

namespace rw {
	namespace dsl {
		template <typename Time, typename T>
		class TimeBasedCache {
		public:
			explicit TimeBasedCache(size_t capacity) : _capacity(capacity) {}

			// ��������
			void insert(const Time& time, const T& data) {
				std::lock_guard<std::mutex> lock(_mutex);

				// �������������ɾ����ɵ�����
				if (_cache.size() >= _capacity) {
					_cache.pop_front();
				}

				// ����������
				_cache.emplace_back(time, data);
			}

			std::vector<T> query(const Time& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// 1. ��ɸѡ����
				std::vector<std::pair<Time, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first < time) {
						candidates.push_back(entry);
					}
					if (!isBefore && entry.first > time) {
						candidates.push_back(entry);
					}
				}

				// 2. ���� time �ľ�������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs(a.first - time) < std::abs(b.first - time);
					});

				// 3. ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// 4. ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// 5. ֻ�������ݲ���
				std::vector<T> result;
				for (const auto& entry : candidates) {
					result.push_back(entry.second);
				}
				return result;
			}

			std::vector<T> query(const Time& beginTime, const Time& endTime, bool hasLeft = true, bool hasRight = true,
				bool ascending = true) const
			{
				std::lock_guard<std::mutex> lock(_mutex);

				std::vector<std::pair<Time, T>> candidates;
				for (const auto& entry : _cache) {
					if (entry.first >= beginTime && entry.first <= endTime) {
						candidates.push_back(entry);
					}
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ���� hasLeft/hasRight
				if (!candidates.empty() && !hasLeft && candidates.front().first == beginTime) {
					candidates.erase(candidates.begin());
				}
				if (!candidates.empty() && !hasRight && candidates.back().first == endTime) {
					candidates.pop_back();
				}

				std::vector<T> result;
				for (const auto& entry : candidates) {
					result.push_back(entry.second);
				}
				return result;
			}

			std::vector<T> queryWithTime(const Time& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// �ռ������� time �ľ���
				std::vector<std::pair<Time, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first > time) continue;
					if (!isBefore && entry.first < time) continue;
					candidates.push_back(entry);
				}

				// ����������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs(a.first - time) < std::abs(b.first - time);
					});

				// ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ֻ�������ݲ���
				std::vector<T> result;
				for (const auto& entry : candidates) {
					result.push_back(entry.second);
				}
				std::cout << result.size() << std::endl;
				return result;
			}

			std::unordered_map<Time, T> queryToMap(const Time& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// 1. ��ɸѡ����
				std::vector<std::pair<Time, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first < time) {
						candidates.push_back(entry);
					}
					if (!isBefore && entry.first > time) {
						candidates.push_back(entry);
					}
				}

				// 2. ���� time �ľ�������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs(a.first - time) < std::abs(b.first - time);
					});

				// 3. ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// 4. ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// 5. ���� map
				std::unordered_map<Time, T> result;
				for (const auto& entry : candidates) {
					result[entry.first] = entry.second;
				}
				return result;
			}

			std::unordered_map<Time, T> queryWithTimeToMap(const Time& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// �ռ������� time �ľ���
				std::vector<std::pair<Time, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first > time) continue;
					if (!isBefore && entry.first < time) continue;
					candidates.push_back(entry);
				}

				// ����������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs(a.first - time) < std::abs(b.first - time);
					});

				// ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ���� map
				std::unordered_map<Time, T> result;
				for (const auto& entry : candidates) {
					result[entry.first] = entry.second;
				}
				return result;
			}

			// 设置数据（更新或插入）
			void set(const Time& time, const T& data) {
				std::lock_guard<std::mutex> lock(_mutex);

				auto it = std::find_if(_cache.begin(), _cache.end(), [&time](const auto& entry) {
					return entry.first == time;
					});

				if (it != _cache.end()) {
					it->second = data; // 更新已有数据
				}
				else {
					// �������������ɾ����ɵ�����
					if (_cache.size() >= _capacity) {
						_cache.pop_front();
					}

					// ����������
					_cache.emplace_back(time, data);
				}
			}

			// 获取数据
			std::optional<T> get(const Time& time) const {
				std::lock_guard<std::mutex> lock(_mutex);

				auto it = std::find_if(_cache.begin(), _cache.end(), [&time](const auto& entry) {
					return entry.first == time;
					});

				if (it != _cache.end()) {
					return it->second; // 返回找到的数据
				}

				return std::nullopt; // 未找到返回空值
			}

			// ��ȡ�����С
			size_t size() const {
				std::lock_guard<std::mutex> lock(_mutex);
				return _cache.size();
			}

			void clear() {
				std::lock_guard<std::mutex> lock(_mutex);
				_cache = std::deque<std::pair<Time, T>>();
			}

		private:
			mutable std::mutex _mutex; // ����������̰߳�ȫ
			size_t _capacity;          // ��������
			std::deque<std::pair<Time, T>> _cache; // �������ݣ��洢ʱ�������ݵļ�ֵ��
		};

		template <typename T>
		class TimeBasedCache<std::chrono::system_clock::time_point, T> {
		public:
			explicit TimeBasedCache(size_t capacity) : _capacity(capacity) {}

			void insert(const std::chrono::system_clock::time_point& time, const T& data) {
				std::lock_guard<std::mutex> lock(_mutex);
				if (_cache.size() >= _capacity) {
					_cache.pop_front();
				}
				_cache.emplace_back(time, data);
			}

			std::vector<T> queryWithTime(const std::chrono::system_clock::time_point& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// �ռ������� time �ľ���
				std::vector<std::pair<std::chrono::system_clock::time_point, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first > time) continue;
					if (!isBefore && entry.first < time) continue;
					candidates.push_back(entry);
				}

				// ����������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs((a.first - time).count()) < std::abs((b.first - time).count());
					});

				// ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ֻ�������ݲ���
				std::vector<T> result;
				for (const auto& entry : candidates) {
					result.push_back(entry.second);
				}
				return result;
			}

			std::unordered_map<std::chrono::system_clock::time_point, T> queryToMap(const std::chrono::system_clock::time_point& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// �ռ������� time �ľ���
				std::vector<std::pair<std::chrono::system_clock::time_point, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first < time) {
						candidates.push_back(entry);
					}
					if (!isBefore && entry.first > time) {
						candidates.push_back(entry);
					}
				}

				// ����������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs((a.first - time).count()) < std::abs((b.first - time).count());
					});

				// ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ���� map
				std::unordered_map<std::chrono::system_clock::time_point, T> result;
				for (const auto& entry : candidates) {
					result[entry.first] = entry.second;
				}
				return result;
			}

			std::unordered_map<std::chrono::system_clock::time_point, T> queryWithTimeToMap(const std::chrono::system_clock::time_point& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				if (count <= 0) return {};

				// �ռ������� time �ľ���
				std::vector<std::pair<std::chrono::system_clock::time_point, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first > time) continue;
					if (!isBefore && entry.first < time) continue;
					candidates.push_back(entry);
				}

				// ����������
				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs((a.first - time).count()) < std::abs((b.first - time).count());
					});

				// ȡǰ count ��
				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ���� map
				std::unordered_map<std::chrono::system_clock::time_point, T> result;
				for (const auto& entry : candidates) {
					result[entry.first] = entry.second;
				}
				return result;
			}

			std::vector<T> query(const std::chrono::system_clock::time_point& time, int count, bool isBefore = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);
				if (count <= 0) return {};

				std::vector<std::pair<std::chrono::system_clock::time_point, T>> candidates;
				for (const auto& entry : _cache) {
					if (isBefore && entry.first < time) {
						candidates.push_back(entry);
					}
					if (!isBefore && entry.first > time) {
						candidates.push_back(entry);
					}
				}

				std::sort(candidates.begin(), candidates.end(), [&time](const auto& a, const auto& b) {
					return std::abs((a.first - time).count()) < std::abs((b.first - time).count());
					});

				if (candidates.size() > static_cast<size_t>(count)) {
					candidates.resize(count);
				}

				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				std::vector<T> result;
				for (const auto& entry : candidates) {
					result.push_back(entry.second);
				}
				return result;
			}

			std::vector<T> query(const std::chrono::system_clock::time_point& beginTime, const std::chrono::system_clock::time_point& endTime, bool hasLeft = true, bool hasRight = true, bool ascending = true) const {
				std::lock_guard<std::mutex> lock(_mutex);

				std::vector<std::pair<std::chrono::system_clock::time_point, T>> candidates;
				for (const auto& entry : _cache) {
					if (entry.first >= beginTime && entry.first <= endTime) {
						candidates.push_back(entry);
					}
				}

				// ��ʱ������
				if (ascending) {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first < b.first;
						});
				}
				else {
					std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
						return a.first > b.first;
						});
				}

				// ���� hasLeft/hasRight
				if (!candidates.empty() && !hasLeft && candidates.front().first == beginTime) {
					candidates.erase(candidates.begin());
				}
				if (!candidates.empty() && !hasRight && candidates.back().first == endTime) {
					candidates.pop_back();
				}

				std::vector<T> result;
				for (const auto& entry : candidates) {
					result.push_back(entry.second);
				}
				return result;
			}

			size_t size() const {
				std::lock_guard<std::mutex> lock(_mutex);
				return _cache.size();
			}
			// 设置数据（更新或插入）
			void set(const std::chrono::system_clock::time_point& time, const T& data) {
				std::lock_guard<std::mutex> lock(_mutex);

				auto it = std::find_if(_cache.begin(), _cache.end(), [&time](const auto& entry) {
					return entry.first == time;
					});

				if (it != _cache.end()) {
					it->second = data; // 更新已有数据
				}
				else {
					if (_cache.size() >= _capacity) {
						_cache.pop_front();
					}
					_cache.emplace_back(time, data);
				}
			}

			// 获取数据
			std::optional<T> get(const std::chrono::system_clock::time_point& time) const {
				std::lock_guard<std::mutex> lock(_mutex);

				auto it = std::find_if(_cache.begin(), _cache.end(), [&time](const auto& entry) {
					return entry.first == time;
					});

				if (it != _cache.end()) {
					return it->second; // 返回找到的数据
				}

				return std::nullopt; // 未找到返回空值
			}

			void clear() {
				std::lock_guard<std::mutex> lock(_mutex);
				_cache = std::deque<std::pair<Time, T>>();
			}
		private:
			mutable std::mutex _mutex;
			size_t _capacity;
			std::deque<std::pair<std::chrono::system_clock::time_point, T>> _cache;
		};
	}
}