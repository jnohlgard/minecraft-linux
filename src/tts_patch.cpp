#include "tts_patch.h"

#include <mcpelauncher/patch_utils.h>
#include <mcpelauncher/minecraft_version.h>

void TTSPatch::install(void* handle) {
    if (!MinecraftVersion::isAtLeast(1, 8))
        return;
    void* sym = minecraft_dlsym(handle, "_ZN18TextToSpeechSystem15createTTSClientEb");
    PatchUtils::patchCallInstruction(sym, (void*) &TTSPatch::createTTSClient, true);
}

mcpe::shared_ptr<NullTextToSpeechClient> TTSPatch::createTTSClient() {
    auto ret = new NullTextToSpeechClient;
    ret->vtable = &NullTextToSpeechClient::myVtable[2];
    return mcpe::shared_ptr<NullTextToSpeechClient>(ret);
}