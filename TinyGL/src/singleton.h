#ifndef SINGLETON_H
#define SINGLETON_H

#include <cstdlib>
#include <assert.h>

template <class T>
class Singleton {
public:
	inline static T* getInstance()
	{
		if (!m_pInstance)
			m_pInstance = new T;
		assert(m_pInstance != NULL);
		return m_pInstance;
	}

	inline static void destroyInstance()
	{
		if (!m_pInstance)
			return;
		delete m_pInstance;
		m_pInstance = NULL;
	}

protected:
    Singleton(void) {}
    virtual ~Singleton(void) {}

private:
	static T* m_pInstance;
	Singleton(const Singleton& rhs) {}
	Singleton& operator =(const Singleton& rhs) {}
};

template <class T> T* Singleton<T>::m_pInstance = NULL;

#endif // SINGLETON_H
