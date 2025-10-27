#pragma once
#include <Core/CoreTypes.h>
#include <type_traits>
#include <memory>

namespace ME::Memory::Pointer
{
	enum class PtrMode
	{
		ThreadSafe = 0,
		SingleThreaded = 1,
	};

	template <PtrMode Mode>
	class BaseReferenceController
	{
	private:
		using ReferenceCountType = std::conditional_t<Mode == PtrMode::ThreadSafe, std::atomic<int32>, int32>;

	public:
		BaseReferenceController() = default;

		virtual ~BaseReferenceController() {};
		
	public:
		BaseReferenceController(const BaseReferenceController& ) = delete;
		BaseReferenceController& operator=(const BaseReferenceController&& ) = delete;

	public:
		virtual void Destroy() = 0;

	public:
		int32 GetReferenceCount()
		{
			if (Mode == PtrMode::ThreadSafe)
				return m_ReferenceCount.load(std::memory_order_relaxed);
			else
				return m_ReferenceCount
		}

		int32 GetWeakReferenceCount()
		{
			if (Mode == PtrMode::ThreadSafe)
				return m_WeakReferenceCount.load(std::memory_order_relaxed);
			return m_WeakReferenceCount
		}

		void IncrementReference()
		{
			if (Mode == PtrMode::ThreadSafe)
				m_ReferenceCount.store(1, std::memory_order_relaxed);
			else
				m_ReferenceCount++;
		}

		void IncrementWeakReference()
		{
			if (Mode == PtrMode::ThreadSafe)
				m_WeakReferenceCount.store(1, std::memory_order_relaxed);
			else
				++m_WeakReferenceCount;
		}

		void DecrementReference()
		{
			if (Mode == PtrMode::ThreadSafe)
			{
				m_ReferenceCount.fetch_sub(1, std::memory_order_relaxed);

				if (m_ReferenceCount == 0)
				{
					Destroy();
					DecrementWeakReference();
				}
			}
			else
			{
				m_ReferenceCount--;

				if (m_ReferenceCount == 0)
				{
					Destroy();
					DecrementWeakReference();
				}
			}
		}

		void DecrementWeakReference()
		{
			if (Mode == PtrMode::ThreadSafe)
			{
				m_WeakReferenceCount.fetch_sub(1, std::memory_order_relaxed);

				if (m_WeakReferenceCount == 0)
				{
					delete this;
				}
			}
			else
			{
				m_WeakReferenceCount--;

				if (m_WeakReferenceCount == 0)
				{
					delete this;
				}
			}
		}

		bool ConditionallyAddSharedReference()
		{
			if (Mode == PtrMode::ThreadSafe)
			{
				bool result = false;

				int32 OriginalCount = m_ReferenceCount.load(std::memory_order_relaxed);

				for (; ; )
				{
					if (OriginalCount == 0)
					{
						result = false;
						break;
					}

					if (m_ReferenceCount.compare_exchange_weak(OriginalCount, OriginalCount + 1, std::memory_order_relaxed))
					{
						result = true;
						break;
					}
				}

				if (result)
					DecrementReference();

				return result;
			}
			else
			{
				if (m_ReferenceCount == 0)
					return false;

				m_ReferenceCount++;
				return true;
			}
		}

	protected:
		std::atomic<int32> m_ReferenceCount;
		ReferenceCountType m_WeakReferenceCount;

	};

	template <class DataType, PtrMode Mode>
	class ReferenceController : public BaseReferenceController<Mode>
	{
	public:
		explicit ReferenceController(DataType* pointer)	: m_Data(pointer) {}
		
		ReferenceController(const ReferenceController&) = delete;
		ReferenceController& operator&(const ReferenceController&) = delete;

		virtual void Delete() override
		{
			delete m_Data;
		}

	private:
		DataType* m_Data;

	};

	template<PtrMode Mode>
	class ReferenceHandler
	{
	public:
		ReferenceHandler() : m_Controller(nullptr) {}
		ReferenceHandler(BaseReferenceController<Mode>* controller) : m_Controller(controller) {}

		ReferenceHandler(const ReferenceHandler& other) : m_Controller(other.m_Controller) 
		{
			if (m_Controller)
				m_Controller->IncrementReference();
		}
		ReferenceHandler(ReferenceHandler&& other) : m_Controller(other.m_Controller)
		{
			other.m_Controller = nullptr;
		}

		ReferenceHandler(WeakReferenceHandler<Mode>&& other)
			: m_Controller(other.m_Controller)
		{
			if (m_Controller)
			{
				if (!m_Controller->ConditionallyAddSharedReference())
					m_Controller = nullptr;

				other.m_Controller.DecrementWeakReference();
				other.m_Controller = nullptr;
			}
		}

		~ReferenceHandler()
		{
			if (m_Controller)
				m_Controller->DecrementReference();
		}

	public:
		ReferenceHandler& operator=(const ReferenceHandler& other)
		{
			if (m_Controller)
				m_Controller->IncrementReference();

			return *this;
		}

		ReferenceHandler& operator=(ReferenceHandler&& other)
		{
			if (other.m_Controller != m_Controller)
			{
				if (m_Controller) m_Controller->DecrementReference();

				m_Controller = other.m_Controller;
				other.m_Controller = nullptr;
			}

			return *this;
		}

		bool IsActive() const
		{
			return m_Controller != nullptr;
		}

		int32 GetReferenceCount() const
		{
			return m_Controller != nullptr ? m_Controller->GetReferenceCount() : 0;
		}

	private:
		template<PtrMode Mode> friend class WeakReferenceController;

	private:
		BaseReferenceController<Mode>* m_Controller;

	};

	template<PtrMode Mode>
	class WeakReferenceHandler
	{
	public:
		WeakReferenceHandler() : m_Controller(nullptr) {}
		WeakReferenceHandler(BaseReferenceController<Mode>* controller) : m_Controller(controller) {}
		WeakReferenceHandler(const WeakReferenceHandler& other) : m_Controller(other.m_Controller)
		{
			if (m_Controller)
				m_Controller->IncrementReference();
		}

		WeakReferenceHandler(WeakReferenceHandler&& other) : m_Controller(other.m_Controller)
		{
			other.m_Controller = nullptr;
		}

		WeakReferenceHandler(const ReferenceController& other) : m_Controller(other.m_Controller)
		{
			if (m_Controller)
				m_Controller->IncrementWeakReference();
		}

		~WeakReferenceHandler()
		{
			if (m_Controller)
				m_Controller->DecrementWeakReference();
		}

	public:
		WeakReferenceHandler& operator=(const WeakReferenceHandler& other)
		{
			Assign(other.m_Controller);

			return *this;
		}

		WeakReferenceHandler& operator=(WeakReferenceHandler&& other)
		{
			if (m_Controller)
				m_Controller->DecrementWeakReference();

			m_Controller = other.m_Controller;
			other.m_Controller = nullptr;

			return *this;
		}

		WeakReferenceHandler& operator=(const ReferenceController& other)
		{
			Assign(other.m_Controller)
			return *this;
		}

		bool IsActive() const
		{
			return m_Controller != nullptr;
		}

	private:
		void Assign(BaseReferenceController<Mode>* other)
		{
			if (other != ReferenceHandler)
			{
				if (other)
					other->IncrementWeakReference();

				if (m_Controller)
					m_Controller->DecrementWeakReference();

				m_Controller = other;
			}
		}

	private:
		template<PtrMode Mode> friend class ReferenceController;

	private:
		BaseReferenceController<Mode>* m_Controller;

	};

	template<class T, PtrMode Mode>
	BaseReferenceController<Mode>* CreateReferenceController(T* pointer)
	{
		return new ReferenceController<T, Mode>(pointer);
	}


}