#include "baron/impl/field.h"

namespace Baron::Internal {
 jvalue JFieldID::get(const JavaVM * vm, FakeJni::JObject * obj) const {
  auto jvm = (Baron::Jvm *)vm;
  if (jvm->isFabricated(obj)) {
   auto clazz = resolveType(jvm);
   if (!clazz) {
    throw std::runtime_error("FATAL: Tried to fabricate a value for an unregistered type!");
   }
   auto pair = std::pair{obj, this};
   jvalue value;
   bool found = false;
   for (auto &[k, v]: jvm->fabricatedValues) {
    if (k.first == pair.first && k.second == pair.second) {
     found = true;
     break;
    }
   }
   if (found) {
    value = jvm->fabricatedValues[pair];
   } else {
    FakeJni::LocalFrame frame(*jvm);
    value = jvm->fabricateValue(frame.getJniEnv(), clazz);
    jvm->fabricatedValues[pair] = value;
   }
   return value;
  }
  return FakeJni::JFieldID::get(vm, obj);
 }

 void JFieldID::set(const JavaVM * vm, FakeJni::JObject * obj, void * value) const {
  auto jvm = (Baron::Jvm *)vm;
  if (jvm->isFabricated(obj)) {
   auto clazz = resolveType(jvm);
   if (!clazz) {
    throw std::runtime_error("FATAL: Tried to fabricate a value for an unregistered type!");
   }
   auto pair = std::pair{obj, this};
   FakeJni::LocalFrame frame(*jvm);
   jvm->fabricatedValues[pair] = jvm->fabricateValue(frame.getJniEnv(), clazz);
  }
  FakeJni::JFieldID::set(vm, obj, value);
 }
}