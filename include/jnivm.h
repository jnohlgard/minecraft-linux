#pragma once
#include <string>
#include <pthread.h>
#include <jni.h>
#include <stack>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <type_traits>
#include <forward_list>

#define JNI_DEBUG

namespace jnivm {
    class Method {
    public:
#ifdef JNI_DEBUG
        std::string name;
#endif
        std::string signature;
#ifdef JNI_DEBUG
        bool _static = false;
#endif
        // Unspecified Wrapper Types
        std::shared_ptr<void> nativehandle;

#ifdef JNI_DEBUG
        std::string GenerateHeader(const std::string &cname);
        std::string GenerateStubs(std::string scope, const std::string &cname);
        std::string GenerateJNIBinding(std::string scope, const std::string &cname);
#endif
    };

    class Field {
    public:
#ifdef JNI_DEBUG
        std::string name;
        std::string type;
        bool _static = false;
#endif
        // Unspecified Wrapper Types
        std::shared_ptr<void> getnativehandle;
        std::shared_ptr<void> setnativehandle;
#ifdef JNI_DEBUG
        std::string GenerateHeader();
        std::string GenerateStubs(std::string scope, const std::string &cname);
        std::string GenerateJNIBinding(std::string scope);
#endif
    };

        enum class FunctionType {
        None = 0,
        Instance = 1,
        Property = 2
    };

    template<class=void()> struct Function;
    template<class R, class ...P> struct Function<R(P...)> {
        using Return = R;
        template<size_t I=0> using Parameter = typename std::tuple_element_t<I, std::tuple<P...,void>>;
        using Type = std::function<R(P...)>;
        static constexpr size_t plength = sizeof...(P);
        static constexpr FunctionType type = FunctionType::None;
    };

    template<class R, class ...P> struct Function<R(*)(P...)> : Function<R(P...)> {
    };

    template<class T, class R, class ...P> struct Function<R(T::*)(P...)> {
        using Return = R;
        template<size_t I=0> using Parameter = typename std::tuple_element_t<I, std::tuple<T, P...,void>>;
        using Type = std::function<R(T, P...)>;
        static constexpr size_t plength = sizeof...(P) + 1;
        static constexpr FunctionType type = FunctionType::Instance;
    };

    template<class T, class R> struct Function<R(T::*)> {
        using Return = R;
        template<size_t I=0> using Parameter = typename std::tuple_element_t<I, std::tuple<T, Return,void>>;
        static constexpr size_t plength = 2;
        static constexpr FunctionType type = (FunctionType)((int)FunctionType::Instance | (int)FunctionType::Property);
    };

    template<class R> struct Function<R*> {
        using Return = R;
        template<size_t I=0> using Parameter = typename std::tuple_element_t<I, std::tuple<Return,void>>;
        static constexpr size_t plength = 1;
        static constexpr FunctionType type = FunctionType::Property;
    };

    template<class Funk> struct Wrap {
        using Function = Function<Funk>;
        using IntSeq = std::make_index_sequence<Function::plength>;
        template<class I> class __StaticFuncWrapper;
        template<size_t...I> class __StaticFuncWrapper<std::index_sequence<I...>> {
            Funk handle;
        public:
            __StaticFuncWrapper(Funk handle) : handle(handle) {}

            constexpr auto Invoke(const std::vector<jvalue> & values) {
                return handle(((typename Function::template Parameter<I>&)(values[I]))...);
            }
        };
        template<class I> class __InstanceFuncWrapper;
        template<size_t Z, size_t...I> class __InstanceFuncWrapper<std::index_sequence<Z, I...>> {
            Funk handle;
        public:
            __InstanceFuncWrapper(Funk handle) : handle(handle) {}

            constexpr auto Invoke(const std::vector<jvalue> & values) {
                return (((typename Function::template Parameter<Z>&)(values[Z])).*handle)(((typename Function::template Parameter<I>&)(values[I]))...);
            }
        };
        template<class I> class __InstancePropWrapper;
        template<size_t Z, size_t...I> class __InstancePropWrapper<std::index_sequence<Z, I...>> {
            Funk handle;
        public:
            __InstancePropWrapper(Funk handle) : handle(handle) {}

            constexpr auto&& set(const std::vector<jvalue> & values) {
                return ((typename Function::template Parameter<Z>&)(values[Z])).*handle = ((typename Function::template Parameter<1>&)(values[1]));
            }

            constexpr auto&& get(const std::vector<jvalue> & values) {
                return (((typename Function::template Parameter<Z>&)(values[Z])).*handle);
            }
        };
        template<class I> class __StaticPropWrapper;
        template<size_t...I> class __StaticPropWrapper<std::index_sequence<I...>> {
            Funk handle;
        public:
            __StaticPropWrapper(Funk handle) : handle(handle) {}

            constexpr auto&& set(const std::vector<jvalue> & values) {
                return *handle = ((typename Function::template Parameter<0>&)(values[0]));
            }

            constexpr auto&& get(const std::vector<jvalue> & values) {
                return *handle;
            }
        };
        using Wrapper = std::conditional_t<((int)Function::type & (int)FunctionType::Instance) != 0, std::conditional_t<((int)Function::type & (int)FunctionType::Property) != 0, __InstancePropWrapper<IntSeq>, __InstanceFuncWrapper<IntSeq>>, std::conditional_t<((int)Function::type & (int)FunctionType::Property) != 0, __StaticPropWrapper<IntSeq>, __StaticFuncWrapper<IntSeq>>>;
    };

    namespace java {

        namespace lang {

            class Class;

            class Object : public std::enable_shared_from_this<Object> {
            public:
                std::shared_ptr<Class> clazz;
                Object(const std::shared_ptr<Class>& clazz) : clazz(clazz) {}
            };

            template<FunctionType, class> struct HookManager;

            class Class : public Object {
            public:
                std::mutex mtx;
                std::unordered_map<std::string, void*> natives;
                std::string name;
                std::string nativeprefix;
#ifdef JNI_DEBUG
                std::vector<std::shared_ptr<Class>> classes;
#endif
                std::vector<std::shared_ptr<Field>> fields;
                std::vector<std::shared_ptr<Method>> methods;

                Class() : Object(std::shared_ptr<Class>(this, [](Class*) {
                    // Fake Weak reference, hopefully works
                })) {
                }

                template<class T> void Hook(const std::string& method, T&& t);
#ifdef JNI_DEBUG
                std::string GenerateHeader(std::string scope);
                std::string GeneratePreDeclaration();
                std::string GenerateStubs(std::string scope);
                std::string GenerateJNIBinding(std::string scope);
#endif
            };

            template<class w> struct HookManager<FunctionType::None, w> {
                static void install(Class * cl, const std::string& id, typename w::T&& t) {
                    auto method = std::make_shared<Method>();
                    method->name = id;
#ifdef JNI_DEBUG
                    method->_static = true;
#endif
                    // ToDo
                    // method->signature = 
                    method->nativehandle = std::shared_ptr<void>(new typename w::Wrapper::Function::Type(std::bind(t, typename w::Wrapper {t}, std::placeholders::_1, std::placeholders::_2)), [](void * v) {
                        delete (typename w::Wrapper::Function::Type*)v;
                    });
                    cl->methods.emplace_back(std::move(method));
                }
            
            };

            template<class T> void Class::Hook(const std::string& id, T&& t) {
                using w = Wrap<T>;
                HookManager<w::Function::type, w>::install(this, id, std::forward(t));
            }

            class String : public Object, public std::string {
            public:
                String(std::string && str) : std::string(std::move(str)), Object(nullptr) {}
            };
        }
    }

    template<class T, class = void>
    class Array : public java::lang::Object {
    public:
        Array(T* data, jsize length) : data(data), length(length), Object(nullptr) {}
        jsize length;
        T* data;
        ~Array() {
            delete[] data;
        }
    };

    template<class T> class Array<T, std::enable_if_t<std::is_base_of_v<java::lang::Object, T>>> : public Array<std::shared_ptr<T>> {
    public:
        using Array<std::shared_ptr<T>>::Array;
    };

    namespace java {
        namespace lang {
            using Array = jnivm::Array<void>;
        }
    }

    template <class T> struct JNITypes { using Array = jarray; };

    template <> struct JNITypes<jboolean> { using Array = jbooleanArray; };

    template <> struct JNITypes<jbyte> { using Array = jbyteArray; };

    template <> struct JNITypes<jshort> { using Array = jshortArray; };

    template <> struct JNITypes<jint> { using Array = jintArray; };

    template <> struct JNITypes<jlong> { using Array = jlongArray; };

    template <> struct JNITypes<jfloat> { using Array = jfloatArray; };

    template <> struct JNITypes<jdouble> { using Array = jdoubleArray; };

    template <> struct JNITypes<jchar> { using Array = jcharArray; };

    struct ScopedVaList {
        va_list list;
        ~ScopedVaList();
    };

#ifdef JNI_DEBUG
    class Namespace {
    public:
        std::string name;
        std::vector<std::shared_ptr<Namespace>> namespaces;
        std::vector<std::shared_ptr<java::lang::Class>> classes;

        std::string GenerateHeader(std::string scope);
        std::string GeneratePreDeclaration();
        std::string GenerateStubs(std::string scope);
        std::string GenerateJNIBinding(std::string scope);
    };
#endif
    class VM;
    class ENV {
    public:
        // Reference to parent VM
        VM * vm;
        // Invocation Table
        JNINativeInterface ninterface;
        // Holder of the invocation table
        JNIEnv env;
        // All returned Objects are cached here to be freed later 
        std::vector<std::shared_ptr<java::lang::Object>> localcache;
        // All explicit local Objects are stored here controlled by push and pop localframe
        std::forward_list<std::vector<std::shared_ptr<java::lang::Object>>> localframe;
        ENV(VM * vm, const JNINativeInterface & defaultinterface) : vm(vm), ninterface(defaultinterface), env{&ninterface}, localframe({}) {
            ninterface.reserved0 = this;
        }
    };

    class VM {
        // Invocation Table
        JNIInvokeInterface iinterface;
        // Holder of the invocation table
        JavaVM javaVM;
        // Native Interface base Invocation Table
        JNINativeInterface ninterface;
        // Map of all jni threads and local stuff by thread id
        std::unordered_map<pthread_t, std::shared_ptr<ENV>> jnienvs;
        // Map of all classes hooked or implicitly declared
        std::unordered_map<std::string, std::shared_ptr<java::lang::Class>> classes;
#ifdef JNI_DEBUG
        // For Generating Stub header files out of captured jni usage
        Namespace np;
#endif
    public:
        std::mutex mtx;
        // Stores all global references
        std::vector<std::shared_ptr<jnivm::java::lang::Object>> globals;
        // Initialize the native VM instance
        VM();
        // Hook Functions and Properties

        // Returns the jni JavaVM
        JavaVM * GetJavaVM();
        // Returns the jni JNIEnv of the current thread
        JNIEnv * GetJNIEnv();
    };
#ifdef JNI_DEBUG
    std::string GeneratePreDeclaration(JNIEnv *env);
    std::string GenerateHeader(JNIEnv *env);
    std::string GenerateStubs(JNIEnv *env);
    std::string GenerateJNIBinding(JNIEnv *env);
#endif
}