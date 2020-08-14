#include "baron/impl/interface/native.h"
#include "baron/impl/field.h"

#ifdef BARON_DEBUG
#define LOG_BLACKLIST_MATCH \
fprintf(\
 vm.getLog(),\
 "BARON INFO: Ignored blacklisted field lookup '%s#%s::%s'!\n",\
 className,\
 name,\
 sig\
);
#else
#define LOG_BLACKLIST_MATCH
#endif

#ifdef BARON_DEBUG
#define LOG_FABRICATED_FIELD \
fprintf(\
 vm.getLog(),\
 "BARON INFO: Fabricated field %s::%s -> 0x%lx on class '%s'\n",\
 name,\
 sig,\
 (intptr_t)fid,\
 className\
);
#else
#define LOG_FABRICATED_FIELD
#endif

#define CHECK_BLACKLIST \
auto clazz = std::dynamic_pointer_cast<FakeJni::JClass>(env.resolveReference(jclazz)); \
const auto className = clazz->getName();\
if (vm.isClassBlacklisted(className) || vm.isFieldBlacklisted(name, sig, className)) {\
 LOG_BLACKLIST_MATCH\
 return nullptr;\
}

//TODO
static void * fabricatedGetCallback(void * inst) {
 return nullptr;
}

//TODO
static void fabricatedSetCallback(void * inst, void * value) {

}

//TODO once fake-jni supports user-defined core classes, append the backtrace to the JFieldID for later debugging
namespace Baron::Interface {
 //TODO create backtrace of invocation
 //TODO convert to empty arbitrary JFieldID
 jfieldID NativeInterface::getFieldID(jclass jclazz, const char * name, const char * sig) const {
  CHECK_BLACKLIST
  auto fid = (FakeJni::JFieldID *)FakeJni::NativeInterface::getFieldID(jclazz, name, sig);
  if (!fid) {
   fid = new Baron::Internal::JFieldID(fabricatedGetCallback, fabricatedSetCallback, name, sig, FakeJni::JFieldID::PUBLIC);
   clazz->registerField(fid);
   LOG_FABRICATED_FIELD
  }
  //search for method again incase instrumentation occurred downstream (JClass::registerField)
  return FakeJni::NativeInterface::getFieldID(jclazz, name, sig);
 }

 //TODO create backtrace of invocation
 jfieldID NativeInterface::getStaticFieldID(jclass jclazz, const char * name, const char * sig) const {
  CHECK_BLACKLIST
  auto fid = (FakeJni::JFieldID *)FakeJni::NativeInterface::getStaticFieldID(jclazz, name, sig);
  if (!fid) {
   fid = new Baron::Internal::JFieldID(fabricatedGetCallback, fabricatedSetCallback, name, sig, FakeJni::JFieldID::PUBLIC | FakeJni::JFieldID::STATIC);
   clazz->registerField(fid);
   LOG_FABRICATED_FIELD
  }
  return FakeJni::NativeInterface::getStaticFieldID(jclazz, name, sig);
 }
}