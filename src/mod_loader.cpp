#include <mcpelauncher/mod_loader.h>
#include <log.h>
#include <mcpelauncher/linker.h>
#include <dirent.h>
#include <elf.h>
#include <queue>
#include <mcpelauncher/hook.h>

void* ModLoader::loadMod(std::string const& path) {
    void* handle = linker::dlopen(path.c_str(), 0);
    if (handle == nullptr) {
        Log::error("ModLoader", "Failed to load mod %s: %s", path.c_str(), linker::dlerror());
        return nullptr;
    }
    HookManager::instance.addLibrary(handle);

    void (*initFunc)();
    initFunc = (void (*)()) linker::dlsym(handle, "mod_init");
    if (((void*) initFunc) == nullptr) {
        Log::warn("ModLoader", "Mod %s does not have an init function", path.c_str());
        return handle;
    }
    initFunc();

    return handle;
}

void ModLoader::loadModMulti(std::string const& path, std::string const& fileName, std::set<std::string>& otherMods) {
    auto deps = getModDependencies(path + fileName);
    for (auto const& dep : deps) {
        if (otherMods.count(dep) > 0) {
            std::string modName = dep;
            otherMods.erase(dep);
            loadModMulti(path, modName, otherMods);
            otherMods.erase(dep);
        }
    }

    Log::info("ModLoader", "Loading mod: %s", fileName.c_str());
    void* mod = loadMod(path + fileName);
    if (mod != nullptr)
        mods.push_back(mod);
}

void ModLoader::loadModsFromDirectory(std::string const& path) {
    DIR* dir = opendir(path.c_str());
    dirent* ent;
    if (dir == nullptr)
        return;
    Log::info("ModLoader", "Loading mods");
    std::set<std::string> modsToLoad;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;
        std::string fileName(ent->d_name);
        int len = fileName.length();
        if (len < 4 || fileName[len - 3] != '.' || fileName[len - 2] != 's' || fileName[len - 1] != 'o')
            continue;

        modsToLoad.insert(fileName);
    }
    closedir(dir);
    while (!modsToLoad.empty()) {
        auto it = modsToLoad.begin();
        auto fileName = *it;
        modsToLoad.erase(it);

        loadModMulti(path, fileName, modsToLoad);
    }
    Log::info("ModLoader", "Loaded %li mods", mods.size());
    HookManager::instance.applyHooks();
}

std::vector<std::string> ModLoader::getModDependencies(std::string const& path) {
    ElfW(Ehdr) header;
    FILE* file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        Log::error("ModLoader", "getModDependencies: failed to open mod");
        return {};
    }
    if (fread(&header, sizeof(ElfW(Ehdr)), 1, file) != 1) {
        Log::error("ModLoader", "getModDependencies: failed to read header");
        fclose(file);
        return {};
    }

    fseek(file, (long) header.e_phoff, SEEK_SET);

    char phdr[header.e_phentsize * header.e_phnum];
    if (fread(phdr, header.e_phentsize, header.e_phnum, file) != header.e_phnum) {
        Log::error("ModLoader", "getModDependencies: failed to read phnum");
        fclose(file);
        return {};
    }

    // find dynamic
    ElfW(Phdr)* dynamicEntry = nullptr;
    for (int i = 0; i < header.e_phnum; i++) {
        ElfW(Phdr)& entry = *((ElfW(Phdr)*) &phdr[header.e_phentsize * i]);
        if (entry.p_type == PT_DYNAMIC)
            dynamicEntry = &entry;
    }
    if (dynamicEntry == nullptr) {
        Log::error("ModLoader", "getModDependencies: couldn't find PT_DYNAMIC");
        fclose(file);
        return {};
    }
    size_t dynamicDataCount = dynamicEntry->p_filesz / sizeof(ElfW(Dyn));
    ElfW(Dyn) dynamicData[dynamicDataCount];
    fseek(file, (long) dynamicEntry->p_offset, SEEK_SET);
    if (fread(dynamicData, sizeof(ElfW(Dyn)), dynamicDataCount, file) != dynamicDataCount) {
        Log::error("ModLoader", "getModDependencies: failed to read PT_DYNAMIC");
        fclose(file);
        return {};
    }

    // find strtab
    size_t strtabOff = 0;
    size_t strtabSize = 0;
    for (int i = 0; i < dynamicDataCount; i++) {
        if (dynamicData[i].d_tag == DT_STRTAB) {
            strtabOff = dynamicData[i].d_un.d_val;
        } else if (dynamicData[i].d_tag == DT_STRSZ) {
            strtabSize = dynamicData[i].d_un.d_val;
        }
    }
    if (strtabOff == 0 || strtabSize == 0) {
        Log::error("ModLoader", "getModDependencies: couldn't find strtab");
        fclose(file);
        return {};
    }
    std::vector<char> strtab;
    strtab.resize(strtabSize);
    fseek(file, (long) strtabOff, SEEK_SET);
    if (fread(strtab.data(), 1, strtabSize, file) != strtabSize) {
        Log::error("ModLoader", "getModDependencies: failed to read strtab");
        fclose(file);
        return {};
    }
    std::vector<std::string> ret;
    for (int i = 0; i < dynamicDataCount; i++) {
        if (dynamicData[i].d_tag == DT_NEEDED)
            ret.emplace_back(&strtab[dynamicData[i].d_un.d_val]);
    }
    return ret;
}