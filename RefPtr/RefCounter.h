#pragma once

class RefCounter
{
public:
	RefCounter()
	{}
	virtual ~RefCounter() = 0 {}

public:
	virtual void AddRef() final { RefCount += 1; }
	virtual void Release() final
	{
		RefCount -= 1;
		if (RefCount == 0)
			delete this;
	}
	virtual int GetRefCount() const { return RefCount; }

protected:
	int RefCount = 0;

};

template <typename T>
class RefCounterPtr
{
public:
	explicit RefCounterPtr()
		: RefCounter(nullptr)
	{}
	explicit RefCounterPtr(std::nullptr_t _NullPtr)
		: RefCounterPtr()
	{}
	explicit RefCounterPtr(T* _RefObject)
		: RefCounter(_RefObject)
	{
		RefCounter->AddRef();
	}

	RefCounterPtr(const RefCounterPtr& _Other)
	{
		RefCounter = _Other.RefCounter;
		RefCounter->AddRef();
	}
	RefCounterPtr(RefCounterPtr&& _Other) noexcept
	{
		RefCounter = _Other.RefCounter;
		_Other.RefCounter = nullptr;
	}
	~RefCounterPtr()
	{
		if (RefCounter)
			RefCounter->Release();
	}
	RefCounterPtr& operator = (T* _RefObject)
	{
		if (this->RefCounter == _RefObject)
			return *this;
		if (RefCounter)
			RefCounter->Release();
		RefCounter = _RefObject;
		RefCounter->AddRef();
		return *this;
	}
	RefCounterPtr& operator = (const RefCounterPtr& _Other)
	{
		if (this == &_Other)
			return *this;
		if (RefCounter)
			RefCounter->Release();
		RefCounter = _Other.RefCounter;
		RefCounter->AddRef();
		return *this;
	}
	RefCounterPtr& operator = (RefCounterPtr&& _Other) noexcept
	{
		if (this == &_Other)
			return *this;
		if (RefCounter)
			RefCounter->Release();
		RefCounter = _Other.RefCounter;
		_Other.RefCounter = nullptr;
		return *this;
	}

public:
	void Reset()
	{
		if (RefCounter == nullptr)
			return;
		RefCounter->Release();
		RefCounter = nullptr;
	}

	T* operator -> () { return RefCounter; }
	const T* operator -> () const { return RefCounter; }

	T* Get() { return RefCounter; }

private:
	T* RefCounter;

};
