#pragma once

#include <memory>
#include <vector>
#include <numeric>
#include<cassert>
#include <mutex>
#include<iostream>

namespace rw {
	constexpr bool gThreadSafe = true;
	constexpr bool gNotThreadSafe = false;

	/**
	 * @brief: template class ObjectPool
	 *      This class is a template class that implements an object pool.
	 * @template variable:
	 *   - T: The type of the objects that the pool will manage.
	 *   - Allocator: The type of the allocator that will be used to allocate memory for the objects.
	 *   - isThreadSafe: A boolean value that indicates whether the object pool should be thread-safe.
	 *
	 * @details:
	 *
	 */
	template<typename T, typename Allocator = std::allocator<T>, bool IsThreadSafe = gNotThreadSafe>
	class ObjectPool;

	template<typename T, typename Allocator>
	class ObjectPool<T, Allocator, gNotThreadSafe> final {
	public:
		ObjectPool() = default;
		explicit ObjectPool(const Allocator& allocator);
		~ObjectPool();

		/**
		 * @brief  Prevent move construction
		 *
		 * Prevents the object pool from being move-constructed.
		 */
		ObjectPool(ObjectPool&&) = delete;

		/**
		 * @brief  Prevent move assignment
		 *
		 * Prevents the object pool from being move-assigned.
		 */
		ObjectPool& operator=(ObjectPool&&) = delete;

		/**
		 * @brief  Prevent copy construction
		 *
		 * Prevents the object pool from being copy-constructed.
		 */
		ObjectPool(const ObjectPool&) = delete;

		/**
		 * @brief  Prevent copy assignment
		 *
		 * Prevents the object pool from being copy-assigned.
		 */
		ObjectPool& operator=(const ObjectPool&) = delete;

		/**
		 *@Parameters:
		 *  -args: The arguments to be passed to the constructor of the object.
		 *@Methods:
		 *  Acquire an object from the pool.
		 *@Returns: std::shared_ptr<T>
		 *  Return a shared pointer to the accquired object.
		 *@Throws:
		 *
		 */
		template<typename... Args>
		std::shared_ptr<T> acquireObject(Args && ... args);

		/**
		 *@Parameters:
		 *  void
		 *@Methods:
		 *  Get the total allocated memory.
		 *@Returns: size_t
		 *  The total allocated memory.
		 *@Throws:
		 *
		 */
		std::size_t getTotalAllocatedMemory() const;

	private:

		/**
		 *@Parameters:
		 *  void
		 *@Methods:
		 *  Creates a new block of uninitialized memory, big enough to hold m_newChunkSize instances of T.
		 *@Returns: types
		 *  void
		 *@Throws:
		 *
		 */
		void addChunk();

		/**
		 * @brief The pool of objects
		 *
		 * 1.Contains chunks of memory in which instances of T will be created.
		 * 2.For each chunk, the pointer to its first object is stored.
		 */
		std::vector<T*> m_pool;

		/**
		 * @brief The free objects
		 *
		 * Contains pointers to all free instances of T that are available in the pool.
		 */
		std::vector<T*> m_freeObjects;

		/**
		 * @brief The initial chunk size
		 *
		 * The number of T instances that should fit in the first allocated chunk.
		 */
		static constexpr std::size_t ms_initialChunkSize{ 5 };

		/**
		 * @brief The new chunk size
		 *
		 * The number of T instances that should fit in a newly allocated chunk.
		 * This value is doubled after each newly created chunk.
		 */
		std::size_t m_newChunkSize{ ms_initialChunkSize };

		/**
		 * @brief ObjectPool Allocator
		 *
		 * The allocator to use for allocating and deallocating chunks
		 */
		Allocator m_allocator;
	};

	template<typename T, typename Allocator>
	class ObjectPool<T, Allocator, gThreadSafe> final {
	public:
		ObjectPool() = default;
		explicit ObjectPool(const Allocator& allocator);
		~ObjectPool();

		ObjectPool(ObjectPool&&) = delete;
		ObjectPool& operator=(ObjectPool&&) = delete;

		ObjectPool(const ObjectPool&) = delete;
		ObjectPool& operator=(const ObjectPool&) = delete;

		template<typename... Args>
		std::shared_ptr<T> acquireObject(Args&& ... args);

		std::size_t getTotalAllocatedMemory() const;
	private:
		void addChunk();
		std::vector<T*> m_pool;
		std::vector<T*> m_freeObjects;
		static constexpr std::size_t ms_initialChunkSize{ 5 };
		std::size_t m_newChunkSize{ ms_initialChunkSize };
		Allocator m_allocator;
		mutable std::mutex m_mutex; // 互斥锁
	};

	/*------------------------------------------------------------------------------------
	 *                          Inline implementation of ObjectPool<T, Allocator, NotThreadSafe>
	 *-----------------------------------------------------------------------------------*/

	template<typename T, typename Allocator>
	ObjectPool<T, Allocator, gNotThreadSafe>::ObjectPool(const Allocator& allocator)
		:m_allocator{ allocator }
	{
	}

	template<typename T, typename Allocator>
	void ObjectPool<T, Allocator, gNotThreadSafe>::addChunk()
	{
		std::cout << "Allocating new chunk...";
		// Allocate a new chunk of uninitialized memory big enough to hold
		// m_newChunkSize instances of T, and add the chunk to the pool.
		// Care is taken that everything is cleaned up in the event of an exception.
		m_pool.push_back(nullptr);
		try
		{
			m_pool.back() = m_allocator.allocate(m_newChunkSize);
		}
		catch (const std::exception&)
		{
			m_pool.pop_back();
			throw;
		}

		// Create pointers to each individual object in the new chunk
		// and store them in the list of free objects.
		auto oldFreeObjectsSize{ m_freeObjects.size() };
		m_freeObjects.resize(oldFreeObjectsSize + m_newChunkSize);
		std::iota(std::begin(m_freeObjects) + oldFreeObjectsSize, std::end(m_freeObjects), m_pool.back());

		// Double the chunk size for next time.
		m_newChunkSize *= 2;
	}

	template<typename T, typename Allocator>
	template<typename... Args>
	std::shared_ptr<T> ObjectPool<T, Allocator, gNotThreadSafe>::acquireObject(Args && ... args)
	{
		// If there are no free objects, allocate a new chunk.
		if (m_freeObjects.empty()) {
			addChunk();
		}

		// Get a free object.
		T* object{ m_freeObjects.back() };

		// Initialize, i.e. construct, an instance of T in an
		// uninitialized block of memory using placement new, and
		// perfectly forward any provided arguments to the constructor.
		object = new T(std::forward<Args>(args)...);

		// Launder the object pointer.
		T* constructedObject{ std::launder(object) };

		// Remove the object from the list of free objects.
		m_freeObjects.pop_back();

		// Wrap the constructed object and return it.
		return std::shared_ptr<T>{constructedObject, [this](T* object) {
			// Destroy object.
			std::destroy_at(object);
			// Put the object back in the list of free objects.
			m_freeObjects.push_back(object);
			}
		};
	}

	template<typename T, typename Allocator>
	ObjectPool<T, Allocator, gNotThreadSafe>::~ObjectPool()
	{
		// Note: this implementation assumes that all objects handed out by this
		//      pool have been returned to the pool before the pool is destroyed.
		//      The following statement asserts if that is not the case.

		assert(m_freeObjects.size() == ms_initialChunkSize * (std::pow(2, m_pool.size()) - 1));

		// Deallocate all allocated memory.
		std::size_t chunkSize{ ms_initialChunkSize };
		for (auto* chunk : m_pool) {
			m_allocator.deallocate(chunk, chunkSize);
			chunkSize *= 2;
		}
		m_pool.clear();
	}

	template<typename T, typename Allocator>
	std::size_t ObjectPool<T, Allocator, gNotThreadSafe>::getTotalAllocatedMemory() const
	{
		std::size_t totalMemory = 0;
		std::size_t chunkSize = ms_initialChunkSize;
		for (const auto* chunk : m_pool) {
			totalMemory += chunkSize * sizeof(T);
			chunkSize *= 2;
		}
		return totalMemory;
	}

	/*------------------------------------------------------------------------------------
	 *                          Inline implementation of ObjectPool<T, Allocator, ThreadSafe>
	 *-----------------------------------------------------------------------------------*/

	template<typename T, typename Allocator>
	ObjectPool<T, Allocator, gThreadSafe>::ObjectPool(const Allocator& allocator)
		: m_allocator{ allocator }
	{
	}

	template<typename T, typename Allocator>
	void ObjectPool<T, Allocator, gThreadSafe>::addChunk()
	{
		std::cout << "Allocating new chunk...";
		m_pool.push_back(nullptr);
		try
		{
			m_pool.back() = m_allocator.allocate(m_newChunkSize);
		}
		catch (const std::exception&)
		{
			m_pool.pop_back();
			throw;
		}

		auto oldFreeObjectsSize{ m_freeObjects.size() };
		m_freeObjects.resize(oldFreeObjectsSize + m_newChunkSize);
		std::iota(std::begin(m_freeObjects) + oldFreeObjectsSize, std::end(m_freeObjects), m_pool.back());
		m_newChunkSize *= 2;
	}

	template<typename T, typename Allocator>
	template<typename... Args>
	std::shared_ptr<T> ObjectPool<T, Allocator, gThreadSafe>::acquireObject(Args&& ... args)
	{
		std::lock_guard<std::mutex> lock(m_mutex); // 加锁
		if (m_freeObjects.empty()) {
			addChunk();
		}

		T* object{ m_freeObjects.back() };
		object = new T(std::forward<Args>(args)...);
		T* constructedObject{ std::launder(object) };
		m_freeObjects.pop_back();

		return std::shared_ptr<T>{constructedObject, [this](T* object) {
			std::lock_guard<std::mutex> lock(m_mutex); // 加锁
			std::destroy_at(object);
			m_freeObjects.push_back(object);
			}
		};
	}

	template<typename T, typename Allocator>
	ObjectPool<T, Allocator, gThreadSafe>::~ObjectPool()
	{
		std::lock_guard<std::mutex> lock(m_mutex); // 加锁
		assert(m_freeObjects.size() == ms_initialChunkSize * (std::pow(2, m_pool.size()) - 1));
		std::size_t chunkSize{ ms_initialChunkSize };
		for (auto* chunk : m_pool) {
			m_allocator.deallocate(chunk, chunkSize);
			chunkSize *= 2;
		}
		m_pool.clear();
	}

	template<typename T, typename Allocator>
	std::size_t ObjectPool<T, Allocator, gThreadSafe>::getTotalAllocatedMemory() const
	{
		std::lock_guard<std::mutex> lock(m_mutex); // 加锁
		std::size_t totalMemory = 0;
		std::size_t chunkSize = ms_initialChunkSize;
		for (const auto* chunk : m_pool) {
			totalMemory += chunkSize * sizeof(T);
			chunkSize *= 2;
		}
		return totalMemory;
	}
}