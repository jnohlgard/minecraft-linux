#include <jnivm.h>
#ifdef _WIN32
#define pthread_self() GetCurrentThreadId()
#endif
#include <jnivm/internal/scopedVaList.h>
#include <jnivm/internal/skipJNIType.h>
#include <jnivm/internal/findclass.h>
#include <jnivm/internal/jValuesfromValist.h>
#include <jnivm/internal/codegen/namespace.h>
#include "internal/vm.hpp"
#include <locale>
#include <sstream>
#include <climits>
#include "internal/log.h"

using namespace jnivm;

jint GetVersion(JNIEnv *) {
#ifdef JNI_DEBUG
	LOG("JNIVM", "GetVersion unsupported");
#endif
	return 0;
};
jclass DefineClass(JNIEnv *, const char *, jobject, const jbyte *, jsize) {
#ifdef JNI_DEBUG
	LOG("JNIVM", "DefineClass unsupported");
#endif
	return 0;
};

jclass jnivm::FindClass(JNIEnv *env, const char *name) {
	// std::lock_guard<std::mutex> lock(((VM *)(env->functions->reserved1))->mtx);
	auto&& nenv = *(ENV*)env->functions->reserved0;
	std::lock_guard<std::mutex> lock(nenv.vm->mtx);
	return InternalFindClass(env, name);
};
jmethodID FromReflectedMethod(JNIEnv *env, jobject obj) {
	if(obj && env->functions->IsSameObject(env, env->functions->GetObjectClass(env, obj), FindClass(env, "java/lang/reflect/Method"))) {
		return (jmethodID) obj;
	} else {
		return nullptr;
	}
};
jfieldID FromReflectedField(JNIEnv *env, jobject obj) {
	if(obj && env->functions->IsSameObject(env, env->functions->GetObjectClass(env, obj), FindClass(env, "java/lang/reflect/Field"))) {
		return (jfieldID) obj;
	} else {
		return nullptr;
	}
};
jobject ToReflectedMethod(JNIEnv * env, jclass c, jmethodID mid, jboolean isStatic) {
	auto method = (Method*)mid;
	if(c && method && method->_static == (bool)isStatic) {
		return env->NewLocalRef((jobject) method);
	}
	return 0;
};
jclass GetSuperclass(JNIEnv * env, jclass c) {
	auto cl = (Class*) c;
	return cl->superclass ? (jclass)cl->superclass((ENV *)env->functions->reserved0).get() : env->FindClass("java/lang/Object");
};
jboolean IsAssignableFrom(JNIEnv *env, jclass c1, jclass c2) {
	jclass ct1 = c1;
	while(!env->IsSameObject(ct1, c2)) {
		jclass nc = GetSuperclass(env, ct1);
		if(nc == ct1) {
			auto cl = (Class*) c1;
			if(cl->interfaces) {
				for(auto&& i : cl->interfaces((ENV *)env->functions->reserved0)) {
					if(env->IsSameObject((jobject)i.get(), c2)) {
						return JNI_TRUE;
					}
				}
			}
			return JNI_FALSE;
		}
		ct1 = nc;
	}
	return JNI_TRUE;
};
jobject ToReflectedField(JNIEnv * env, jclass c, jfieldID fid, jboolean isStatic) {
	auto field = (Field*)fid;
	if(c && field && field->_static == (bool)isStatic) {
		return env->NewLocalRef((jobject) field);
	}
	return 0;
};
jint Throw(JNIEnv *, jthrowable) {
	LOG("JNIVM", "Not Implemented Method Throw called");
	return 0;
};
jint ThrowNew(JNIEnv *, jclass, const char *) {
	LOG("JNIVM", "Not Implemented Method ThrowNew called");
	return 0;
};
jthrowable ExceptionOccurred(JNIEnv * env) {
	return (jthrowable) JNITypes<std::shared_ptr<Throwable>>::ToJNIType((ENV *)(env->functions->reserved0), ((ENV *)(env->functions->reserved0))->current_exception) ;
};
void ExceptionDescribe(JNIEnv *env) {
	if(((ENV *)(env->functions->reserved0))->current_exception) {
		try {
			std::rethrow_exception(((ENV *)(env->functions->reserved0))->current_exception->except);
		} catch (const std::exception& ex) {
			LOG("JNIVM", "Exception with Message `%s` was thrown", ex.what());
		}
	} else {
		LOG("JNIVM", "No pending Exception");
	}
};
void ExceptionClear(JNIEnv *env) {
	((ENV *)(env->functions->reserved0))->current_exception = nullptr;
};
void FatalError(JNIEnv *, const char * err) {
	LOG("JNIVM", "FatalError called: %s", err);
	abort();
};
jint PushLocalFrame(JNIEnv * env, jint cap) {
#ifdef EnableJNIVMGC
	auto&& nenv = *(ENV*)env->functions->reserved0;
	// Add it to the list
	if(nenv.freeframes.empty()) {
		nenv.localframe.emplace_front();
	} else {
		nenv.localframe.splice_after(nenv.localframe.before_begin(), nenv.freeframes, nenv.freeframes.before_begin());
	}
	// Reserve Memory for the new Frame
	if(cap)
		nenv.localframe.front().reserve(cap);
#endif
	return 0;
};
jobject PopLocalFrame(JNIEnv * env, jobject previousframe) {
#ifdef EnableJNIVMGC
	auto&& nenv = *(ENV*)env->functions->reserved0;
	// Clear current Frame and move to freelist
	nenv.localframe.front().clear();
	nenv.freeframes.splice_after(nenv.freeframes.before_begin(), nenv.localframe, nenv.localframe.before_begin());
	if(nenv.localframe.empty()) {
		LOG("JNIVM", "Freed top level frame of this ENV, recreate it");
		nenv.localframe.emplace_front();
	}
#endif
	return 0;
};
jobject NewGlobalRef(JNIEnv * env, jobject obj) {
#ifdef EnableJNIVMGC
	if(!obj) return nullptr;
	auto&& nenv = *(ENV*)env->functions->reserved0;
	auto&& nvm = *nenv.vm;
	std::lock_guard<std::mutex> guard{nvm.mtx};
	nvm.globals.emplace_back((*(Object*)obj).shared_from_this());
#endif
	return obj;
};
void DeleteGlobalRef(JNIEnv * env, jobject obj) {
#ifdef EnableJNIVMGC
	if(!obj) return;
	auto&& nenv = *(ENV*)env->functions->reserved0;
	auto&& nvm = *nenv.vm;
	std::lock_guard<std::mutex> guard{nvm.mtx};
	auto fe = nvm.globals.end();
	auto f = std::find(nvm.globals.begin(), fe, (*(Object*)obj).shared_from_this());
	if(f != fe) {
		nvm.globals.erase(f);
	} else {
		LOG("JNIVM", "Failed to delete Global Reference");
	}
#endif
};
void DeleteLocalRef(JNIEnv * env, jobject obj) {
#ifdef EnableJNIVMGC
	if(!obj) return;
	auto&& nenv = *(ENV*)env->functions->reserved0;
	for(auto && frame : nenv.localframe) {
		auto fe = frame.end();
		auto f = std::find(frame.begin(), fe, (*(Object*)obj).shared_from_this());
		if(f != fe) {
			frame.erase(f);
			return;
		}
	}
	LOG("JNIVM", "Failed to delete Local Reference");
#endif
};
jboolean IsSameObject(JNIEnv *, jobject lobj, jobject robj) {
	return lobj == robj;
};
jobject NewLocalRef(JNIEnv * env, jobject obj) {
#ifdef EnableJNIVMGC
	if(!obj) return nullptr;
	auto&& nenv = *(ENV*)env->functions->reserved0;
	// Get the current localframe and create a ref
	nenv.localframe.front().emplace_back((*(Object*)obj).shared_from_this());
#endif
	return obj;
};
jint EnsureLocalCapacity(JNIEnv * env, jint cap) {
#ifdef EnableJNIVMGC
	auto&& nenv = *(ENV*)env->functions->reserved0;
	nenv.localframe.front().reserve(cap);
#endif
	return 0;
};
jobject AllocObject(JNIEnv *env, jclass cl) {
	LOG("JNIVM", "Not Implemented Method AllocObject called");
	return nullptr;
};

jclass GetObjectClass(JNIEnv *env, jobject jo) {
	return jo ? (jclass)&((Object*)jo)->getClass() : (jclass)env->FindClass("Invalid");
};
jboolean IsInstanceOf(JNIEnv *env, jobject jo, jclass cl) {
	return jo && IsAssignableFrom(env, GetObjectClass(env, jo), cl);
};

#include "internal/method.h"

#include "internal/field.hpp"

#include "internal/string.hpp"

#include "internal/array.hpp"

jint RegisterNatives(JNIEnv *env, jclass c, const JNINativeMethod *method, jint i) {
	auto&& clazz = (Class*)c;
	if(!clazz) {
		LOG("JNIVM", "RegisterNatives failed, class is nullptr");
	} else {
		std::lock_guard<std::mutex> lock(clazz->mtx);
		while(i--) {
			clazz->natives[method->name] = method->fnPtr;
			auto m = std::make_shared<Method>();
			m->name = method->name;
			m->signature = method->signature;
			m->native = true;
			using Funk = std::function<void(ENV*, Object*, const jvalue *)>;
			// m->nativehandle = std::shared_ptr<void>(new Funk([p=method->fnPtr](ENV* e, Object*o, const jvalue *v) {
				
			// }), [](void * v) {
            //     delete (Funk*)v;
            // });
			m->nativehandle = std::shared_ptr<void>(method->fnPtr, [](void * v) { });
			clazz->methods.push_back(m);
			method++;
		}
	}
	return 0;
};
jint UnregisterNatives(JNIEnv *env, jclass c) {
	auto&& clazz = (Class*)c;
	if(!clazz) {
		LOG("JNIVM", "UnRegisterNatives failed, class is nullptr");
	} else {
		std::lock_guard<std::mutex> lock(clazz->mtx);
		((Class*)c)->natives.clear();
	}
	return 0;
};

jint MonitorEnter(JNIEnv *, jobject o) {
	((Object*)o)->lock.lock.lock();
	return 0;
};
jint MonitorExit(JNIEnv *, jobject o) {
	((Object*)o)->lock.lock.unlock();
	return 0;
}

jint GetJavaVM(JNIEnv * env, JavaVM ** vm) {
	if(vm) {
		std::lock_guard<std::mutex> lock(((ENV *)(env->functions->reserved0))->vm->mtx);
		*vm = ((ENV *)(env->functions->reserved0))->vm->GetJavaVM();
	}
	return 0;
};
jweak NewWeakGlobalRef(JNIEnv *, jobject obj) {
	return obj;
};
void DeleteWeakGlobalRef(JNIEnv *, jweak) {
};

jboolean ExceptionCheck(JNIEnv *env) {
	return (bool)((ENV *)(env->functions->reserved0))->current_exception;
}
#include "internal/bytebuffer.hpp"

jobjectRefType GetObjectRefType(JNIEnv *, jobject) {
	return jobjectRefType::JNIInvalidRefType;
};

template<class ...jnitypes> struct JNINativeInterfaceCompose;
template<class X, class ...jnitypes> struct JNINativeInterfaceCompose<X, jnitypes...> {
	using Type = decltype(std::tuple_cat(std::declval<std::tuple<X, X, X>>(), std::declval<typename JNINativeInterfaceCompose<jnitypes...>::Type>()));
	template<class Y> struct index : std::conditional_t<std::is_same<X,Y>::value, std::integral_constant<size_t, 0>, std::integral_constant<size_t, 1 + JNINativeInterfaceCompose<jnitypes...>::template index<Y>::value>> {};
};
template<> struct JNINativeInterfaceCompose<> {
	using Type = std::tuple<>;
	template<class Y> using index = std::integral_constant<size_t, 0>;	
};

template<class ...jnitypes> constexpr JNINativeInterface GetInterface() {
	using compose = JNINativeInterfaceCompose<jnitypes...>;
	using composeType = typename compose::Type;
	return {
		NULL,
		NULL,
		NULL,
		NULL,
		GetVersion,
		DefineClass,
		FindClass,
		FromReflectedMethod,
		FromReflectedField,
		/* spec doesn't show jboolean parameter */
		ToReflectedMethod,
		GetSuperclass,
		IsAssignableFrom,
		ToReflectedField,
		Throw,
		ThrowNew,
		ExceptionOccurred,
		ExceptionDescribe,
		ExceptionClear,
		FatalError,
		PushLocalFrame,
		PopLocalFrame,
		NewGlobalRef,
		DeleteGlobalRef,
		DeleteLocalRef,
		IsSameObject,
		NewLocalRef,
		EnsureLocalCapacity,
		AllocObject,
		CallStaticMethod<jobject>,
		CallStaticMethod<jobject>,
		CallStaticMethod<jobject>,
		GetObjectClass,
		IsInstanceOf,
		GetMethodID<false>,
		CallMethod<jobject>,
		CallMethod<jobject>,
		CallMethod<jobject>,
		CallMethod<std::tuple_element_t<compose::template index<jnitypes>::value, composeType>> ...,
		CallMethod<std::tuple_element_t<compose::template index<jnitypes>::value + sizeof...(jnitypes), composeType>> ...,
		CallMethod<std::tuple_element_t<compose::template index<jnitypes>::value + 2*sizeof...(jnitypes), composeType>> ...,
		CallMethod<void>,
		CallMethod<void>,
		CallMethod<void>,
		CallNonvirtualMethod<jobject>,
		CallNonvirtualMethod<jobject>,
		CallNonvirtualMethod<jobject>,
		CallNonvirtualMethod<std::tuple_element_t<compose::template index<jnitypes>::value, composeType>> ...,
		CallNonvirtualMethod<std::tuple_element_t<compose::template index<jnitypes>::value + sizeof...(jnitypes), composeType>> ...,
		CallNonvirtualMethod<std::tuple_element_t<compose::template index<jnitypes>::value + 2*sizeof...(jnitypes), composeType>> ...,
		CallNonvirtualMethod<void>,
		CallNonvirtualMethod<void>,
		CallNonvirtualMethod<void>,
		GetFieldID<false>,
		GetField<jobject>,
		GetField<jnitypes>...,
		SetField<jobject>,
		SetField<jnitypes>...,
		GetMethodID<true>,
		CallStaticMethod<jobject>,
		CallStaticMethod<jobject>,
		CallStaticMethod<jobject>,
		CallStaticMethod<std::tuple_element_t<compose::template index<jnitypes>::value, composeType>> ...,
		CallStaticMethod<std::tuple_element_t<compose::template index<jnitypes>::value + sizeof...(jnitypes), composeType>> ...,
		CallStaticMethod<std::tuple_element_t<compose::template index<jnitypes>::value + 2*sizeof...(jnitypes), composeType>> ...,
		CallStaticMethod<void>,
		CallStaticMethod<void>,
		CallStaticMethod<void>,
		GetFieldID<true>,
		GetStaticField<jobject>,
		GetStaticField<jnitypes>...,
		SetStaticField<jobject>,
		SetStaticField<jnitypes>...,
		NewString,
		GetStringLength,
		GetStringChars,
		ReleaseStringChars,
		NewStringUTF,
		GetStringUTFLength,
		GetStringUTFChars,
		ReleaseStringUTFChars,
		GetArrayLength,
		NewObjectArray,
		GetObjectArrayElement,
		SetObjectArrayElement,
		NewArray<jnitypes>...,
		GetArrayElements<jnitypes>...,
		ReleaseArrayElements<jnitypes>...,
		GetArrayRegion<jnitypes>...,
		SetArrayRegion<jnitypes>...,
		RegisterNatives,
		UnregisterNatives,
		MonitorEnter,
		MonitorExit,
		::GetJavaVM,
		GetStringRegion,
		GetStringUTFRegion,
		GetArrayElements<void>,
		ReleaseArrayElements<void>,
		GetStringChars,
		ReleaseStringChars,
		NewWeakGlobalRef,
		DeleteWeakGlobalRef,
		ExceptionCheck,
		NewDirectByteBuffer,
		GetDirectBufferAddress,
		GetDirectBufferCapacity,
		GetObjectRefType,
	};
}

VM::VM() : ninterface(GetInterface<jboolean, jbyte, jchar, jshort, jint, jlong, jfloat, jdouble>()), iinterface({
			this,
			NULL,
			NULL,
			[](JavaVM *) -> jint {

				return JNI_OK;
			},
			[](JavaVM *vm, JNIEnv **penv, void * args) -> jint {
#ifdef EnableJNIVMGC
				auto&& nvm = *(VM *)vm->functions->reserved0;
				std::lock_guard<std::mutex> lock(nvm.mtx);
				auto&& nenv = nvm.jnienvs[pthread_self()];
				if(!nenv) {
					nenv = std::make_shared<ENV>(&nvm, nvm.ninterface);
				}
				if(penv) {
					*penv = &nenv->env;
				}
				FakeJni::JniEnvContext::env = nenv.get();
#else
				if(penv) {
					std::lock_guard<std::mutex> lock(((VM *)(vm->functions->reserved0))->mtx);
					*penv = ((VM *)(vm->functions->reserved0))->GetJNIEnv();
				}
				FakeJni::JniEnvContext::env = ((VM *)(vm->functions->reserved0))->GetEnv().get();
#endif
				return JNI_OK;
			},
			[](JavaVM *vm) -> jint {
#ifdef EnableJNIVMGC
				auto&& nvm = *(VM *)vm->functions->reserved0;
				std::lock_guard<std::mutex> lock(nvm.mtx);
				auto fe = nvm.jnienvs.end();
				auto f = nvm.jnienvs.find(pthread_self());
				if(f != fe) {
					nvm.jnienvs.erase(f);
				}
#endif
				return JNI_OK;
			},
			[](JavaVM *vm, void **penv, jint) -> jint {
				if(penv) {
#ifdef EnableJNIVMGC
					return vm->AttachCurrentThread((JNIEnv**)penv, nullptr);
#else
					std::lock_guard<std::mutex> lock(((VM *)(vm->functions->reserved0))->mtx);
					*penv = ((VM *)(vm->functions->reserved0))->GetJNIEnv();
#endif
				}
				return JNI_OK;
			},
			[](JavaVM * vm, JNIEnv ** penv, void * args) -> jint {
				return vm->AttachCurrentThread(penv, args);
			},
	}) {

#ifdef JNI_DEBUG
	np.name = "jnivm";
#endif
	auto env = jnienvs[pthread_self()] = std::make_shared<ENV>(this, ninterface);
	javaVM.functions = &iinterface;
	env->GetClass<Object>("java/lang/Object");
	env->GetClass<Class>("java/lang/Class");
	env->GetClass<String>("java/lang/String");
	env->GetClass<ByteBuffer>("java/nio/ByteBuffer");
}

JavaVM *VM::GetJavaVM() {
	return &javaVM;
}

JNIEnv *VM::GetJNIEnv() {
#ifdef EnableJNIVMGC
	return &jnienvs[pthread_self()]->env;
#else
	return &jnienvs.begin()->second->env;
#endif
}

std::shared_ptr<ENV> VM::GetEnv() {
#ifdef EnableJNIVMGC
	return jnienvs[pthread_self()];
#else
	return jnienvs.begin()->second;
#endif
}

std::shared_ptr<jnivm::Class> jnivm::VM::findClass(const char *name) {
    return GetEnv()->GetClass(name);
}

jobject jnivm::VM::createGlobalReference(std::shared_ptr<jnivm::Object> obj) {
    return GetEnv()->env.NewGlobalRef((jobject)obj.get());
}

std::shared_ptr<jnivm::Class> jnivm::Object::GetBaseClass(jnivm::ENV *env) {
	return env->GetClass("java/lang/Object");
}