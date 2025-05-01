#pragma once
#include <utility>

template <typename T>
class RefTable
{
public:
	RefTable(T* _RefObject)
		: RefObject(_RefObject)
		, RefCount(0)
	{}
	~RefTable()
	{
		if (RefObject)
			delete RefObject;
	}

public:
	void Refer() { RefCount += 1; }
	void Release()
	{
		RefCount -= 1;
		if (RefCount == 0)
			delete this;
	}
	T* GetRefObject() { return RefObject; }
	const T* GetConstRefObject() const { return RefObject; }

private:
	T* RefObject;
	unsigned long long RefCount;
};

template <typename T>
class RefPtr
{
public:
	RefPtr()
		: ReferenceTable(nullptr)
	{}
	explicit RefPtr(T* _RefObject)
		: ReferenceTable(new RefTable<T>(_RefObject))
	{
		ReferenceTable->Refer();
	}
	RefPtr(const RefPtr& _Other);
	RefPtr(RefPtr&& _Other) noexcept;
	~RefPtr();

	RefPtr& operator = (const RefPtr& _Other);
	RefPtr& operator = (RefPtr&& _Other) noexcept;

public:
	void Reset()
	{
		delete ReferenceTable; 
		ReferenceTable = nullptr;
	}

	T* operator -> () { return ReferenceTable->GetRefObject(); }
	const T* operator -> () const { return ReferenceTable->GetConstRefObject(); }

	T* Get() { return ReferenceTable->GetRefObject(); }

private:
	RefTable<T>* ReferenceTable;

};

template <typename T, typename... Args>
RefPtr<T> MakeRef(Args&&... args)
{
	return RefPtr<T>(new T(args)...);
}

template <typename T>
RefPtr<T>::RefPtr(const RefPtr& _Other)
	: ReferenceTable(_Other.ReferenceTable)
{
	ReferenceTable->Refer();
}

template <typename T>
RefPtr<T>::RefPtr(RefPtr&& _Other) noexcept
	: ReferenceTable(_Other.ReferenceTable)
{
	_Other.ReferenceTable = nullptr;
}

template <typename T>
RefPtr<T>::~RefPtr()
{
	if (ReferenceTable)
		ReferenceTable->Release();
}

template <typename T>
RefPtr<T>& RefPtr<T>::operator=(const RefPtr& _Other)
{
	_Other.ReferenceTable->Refer();
	if (ReferenceTable)
		ReferenceTable->Release();
	ReferenceTable = _Other.ReferenceTable;
	return *this;
}

template <typename T>
RefPtr<T>& RefPtr<T>::operator=(RefPtr&& _Other) noexcept
{
	if (ReferenceTable == _Other.ReferenceTable)
		return *this;

	if (ReferenceTable)					// 내 테이블이 있으면 소유권 없애기
		ReferenceTable->Release();

	ReferenceTable = _Other.ReferenceTable;

	_Other.ReferenceTable = nullptr;

	return *this;
}
