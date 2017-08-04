#ifndef __SINGLETON_20170722_H__
#define __SINGLETON_20170722_H__

namespace CN
{
#define DECLARE_SINGLETON(cls)\
    private:\
            static cls* m_poInstance;\
    public:\
           static bool CreateInstance()\
    {\
        if(NULL == m_poInstance)\
        m_poInstance = new cls;\
        return m_poInstance != NULL;\
    }\
    static cls* Instance(){ return m_poInstance; }\
    static void DestroyInstance()\
    {\
        if(m_poInstance != NULL)\
        {\
            delete m_poInstance;\
            m_poInstance = NULL;\
        }\
    }

#define IMPLEMENT_SINGLETON(cls) \
    cls* cls::m_poInstance = NULL;

    //The second way to declare a singleton class
#define  SINGLETON_FRIEND_CLASS_REGISTER(T)  friend  class  CSingleton<T> 

        template<class  T>
        class   CSingleton
        {
            public:

                static bool CreateInstance()
                {
                    if(NULL == m_poInstance)
                    {
                        m_poInstance = new T;
                    }
                    return m_poInstance != NULL;
                }
                static  T* Instance(void)
                {
                    return m_poInstance;
                }

                static void DestroyInstance(void)
                {
                    delete  m_poInstance;
                    m_poInstance = NULL;
                }

            protected: 
                CSingleton(void){}
                virtual ~CSingleton(void){}

            private:
                static T*   m_poInstance;
        };
    
    template<class T>
        T* CSingleton<T>::m_poInstance = NULL;

};

#endif
