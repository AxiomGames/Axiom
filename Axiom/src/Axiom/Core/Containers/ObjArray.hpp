#include "../Array.hpp"
#include "../TemplateBase.hpp"

template<typename T>
class ObjArray
{
public:
	using iterator_base = ArrayIteratorBase<T>;
	using iterator = ArrayIterator<T>;
	using const_iterator = ArrayConstIterator<T>;
	using size_type = int;

	explicit ObjArray(size_type capacity = 12)
		: m_Data(new T[capacity]), m_Capacity(capacity)
	{}

	ObjArray(size_type capacity, const T& filledObj)
		: m_Data(new T[capacity]), m_Capacity(capacity)
	{
		while (m_Size < capacity)
			m_Data[m_Size++] = filledObj;
	}

	~ObjArray()
	{
		if (m_Data)
		{
			Destroy(m_Data, m_Size); // call deconstructors
			delete[] m_Data;
			m_Data = nullptr;
			m_Size = 0; m_Capacity = 0;
		}
	}

	ObjArray(const ObjArray& other)
		: m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		m_Data = new T[other.m_Size];
		ObjCopy(m_Data, other.m_Data, m_Size);
	}

	ObjArray(ObjArray&& other) noexcept
		: m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		m_Data = other.m_Data;
		other.m_Data = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
	}

	ObjArray& operator=(const ObjArray& other)
	{
		m_Capacity = other.m_Capacity;
		m_Size = other.m_Size;

		if (m_Data != nullptr) {
			Realloc(other.m_Capacity * sizeof(T));
		}
		else {
			m_Data = new T[m_Capacity];
			ObjCopy(m_Data, other.m_Data, m_Size);
		}
		return *this;
	}

	static inline void ObjCopy(T* dst, const T* src, size_type len)
	{
		for (size_type i = 0; i < len; ++i) dst[i] = src[i];
	}

	static inline void ObjMove(T* dst, T* src, size_type len)
	{
		for (size_type i = 0; i < len; ++i) dst[i] = Move(src[i]);
	}

	static void Destroy(T* dst, size_type len)
	{
		for (size_type i = 0; i < len; ++i) dst[i].~T();
	}

	void Realloc(size_type size)
	{
		T* old = m_Data;
		T* newData = new T[size];
		if (size >= m_Size)
		{
			for (size_type i = 0; i < m_Size; ++i)
				newData[i] = (T&&)old[i];
		}
		else { // size < m_Size
			Destroy(m_Data, m_Size);
		}

		delete[] old;
		m_Data = newData;
		m_Capacity = size;
	}

	iterator begin() { return iterator { m_Data }; }
	iterator end() { return iterator { m_Data + m_Size }; }
	const_iterator begin() const { return const_iterator { m_Data }; }
	const_iterator end() const { return const_iterator { m_Data + m_Size }; }

	[[nodiscard]] bool Any() const { return m_Size > 0; }
	[[nodiscard]] bool Empty() const { return m_Size == 0; }

	T& operator[](size_type index) { return m_Data[index]; }
	T& At(uint32_t index)  { return m_Data[index]; }

	void Clear(size_type capacity = 12)
	{
		if (m_Size == 0) return;

		m_Size = 0;
		m_Capacity = capacity;
		Realloc(capacity);
	}

	T& Add(T type)
	{
		if (m_Size + 1 > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		return (m_Data[m_Size++] = type);
	}

	void Add(const ObjArray& other)
	{
		if (m_Size + other.m_Size > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + other.m_Size));
		}

		for (size_type i = 0; i < other.m_Size; ++i)
		{
			m_Data[m_Size++] = other[i];
		}
	}

	template<typename... Args>
	T& Emplace(Args&&... args)
	{
		if (m_Size + 1 > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		return (m_Data[m_Size++] = T(args...)); // forward here but without stl
	}

	void Emplace(ObjArray& other)
	{
		if (m_Size + other.m_Size > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + other.m_Size));
		}
		for (size_type i = 0; i < other.m_Size; ++i) {
			m_Data[m_Size++] = Move(other[i]);
		}
		other.~T();
	}

	void EmplaceAt(uint32_t index, T& type)
	{
		ax_assert(index > m_Size);

		if (m_Size + 1 > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		MakeSpace(index, 1);
		m_Data[index] = Move(type);
		m_Size++;
	}

	void EmplaceAtUnordered(uint32_t index, T& type)
	{
		ax_assert(index > m_Size);

		if (m_Size + 1 > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		// move at Index to end of the array
		m_Data[m_Size++] = m_Data[index];
		// place new object
		m_Data[index] = Move(type);
	}

	void AddRange(T* begin, T* end)
	{
		uint64 len = Distance(begin, end);

		if (m_Size + len - 1 > m_Capacity)
		{
			m_Capacity = CalculateGrowth(m_Size + len);
		}

		while (begin != end) {
			m_Data[m_Size++] = *begin++;
		}
	}

	void InsertAt(size_type index, T type)
	{
		ax_assert(index > m_Size);

		if (m_Size + 1 > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		MakeSpace(index, 1);
		m_Data[m_Size++] = type;
	}

	void InsertAtUnordered(uint32_t index, T type)
	{
		ax_assert(index > m_Size);

		if (m_Size + 1 > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		// move at Index to end of the array
		m_Data[m_Size++] = m_Data[index];
		// place new object
		m_Data[index] = type;
	}

	void Insert(const ObjArray& other)
	{
		if (m_Size + other.m_Size > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + other.m_Size));
		}

		for (size_type i = 0; i < other.m_Size; ++i) {
			m_Data[m_Size++] = other[i];
		}
	}

	size_type AddUninitialized(size_type count = 1)
	{
		ax_assert(count > 0);
		size_type oldNum = m_Size;
		if (m_Size + count > m_Capacity) {
			Realloc(CalculateGrowth(m_Size + 1));
		}
		for (size_type i = 0; i < count; ++i) m_Data[m_Size++] = T();
		return oldNum;
	}

	void SetRange(size_type start, size_type end, const T& value)
	{
		for (; start < end; ++start)
		{
			m_Data[start].~T();
			m_Data[start] = value;
		}
	}

	template<typename Func_t>
	size_type RemoveAll(Func_t match)
	{
		size_type freeIndex = 0;   // the first free slot in items array

		// Find the first item which needs to be removed.
		while (freeIndex < m_Size && match(m_Data[freeIndex]) == false) freeIndex++;
		if (freeIndex >= m_Size) return 0;

		size_type current = freeIndex + 1;
		while (current < m_Size)
		{
			// Find the first item which needs to be kept.
			while (current < m_Size && match(m_Data[current]) == true) current++;

			if (current < m_Size) {
				m_Data[freeIndex].~T();
				// copy item to the free slot.
				m_Data[freeIndex++] = (T&&)(m_Data[current++]);
			}
		}

		size_type numCleared = m_Size - freeIndex;
		Destroy(m_Data + freeIndex, numCleared);
		m_Size = freeIndex;
		return numCleared; // removed item count
	}

	// removes first matched obj
	template<typename Func_t>
	void Remove(Func_t match)
	{
		for (size_type i = 0; i < m_Size; ++i)
		{
			if (match(m_Data[i]))
			{
				RemoveSpace(i, i + 1);
				break;
			}
		}
	}

	void RemoveAt(size_type index)
	{
		RemoveSpace(index, index + 1);
	}

	void RemoveAtUnordered(size_type index)
	{
		ax_assert(index > m_Size);
		m_Data[index].~T();
		m_Data[index] = Move(m_Data[--m_Size]);
	}

	template<typename Func_t>
	void RemoveUnordered(Func_t match)
	{
		for (size_type i = 0; i < m_Size; ++i)
		{
			if (match(m_Data[i]))
			{
				m_Data[i].~T();
				m_Data[i] =	Move(m_Data[--m_Size]);
				break;
			}
		}
	}

private:
	void MakeSpace(size_type index, size_type len)
	{
		size_type curr = m_Size + len;
		while (curr > index)
		{
			m_Data[curr] = m_Data[--curr];
		}
		size_type spaceEnd = index + len;
		while (curr < spaceEnd)
			m_Data[curr++].~T();
	}

	void RemoveSpace(size_type begin, size_type end)
	{
		size_type current = begin;

		while (current < end)
			m_Data[current++].~T();

		current = begin;
		while (end < m_Size)
		{
			m_Data[current++] = m_Data[end++];
		}
	}

	size_type CalculateGrowth(size_type capacity) const
	{
		return capacity + (capacity / 2);
	}

private:
	T* m_Data = nullptr;
	int m_Size = 0;
	int m_Capacity = 0;
};