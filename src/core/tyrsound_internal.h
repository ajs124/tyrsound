#ifndef TYRSOUND_INTERNAL_H
#define TYRSOUND_INTERNAL_H

#include <stdlib.h>
#include <string.h>
#include <new>

#include "tyrsound_ex.h"

#include "tyrsound_begin.h"

// Definitions
class SoundObject;


// in tyrsound_device.cpp
DeviceBase *getDevice();
bool initDevice(const char *name, const tyrsound_Format *fmt);
void shutdownDevice();

// in tyrsound_sound.cpp
tyrsound_Handle registerSoundObject(SoundObject *);
void registerUpdate(SoundObject *);
void unregisterUpdate(SoundObject *);
tyrsound_Error initSounds();
void shutdownSounds();
tyrsound_Error updateSounds();

// in tyrsound_load.cpp
void initDecoders();
void shutdownDecoders();

// in tyrsound_misc.cpp
void breakpoint();
bool isBigEndian();
unsigned int readLE32(const void *buf);
unsigned short readLE16(const void *buf);
void writeLE32(void *buf, unsigned int i);
void writeLE16(void *buf, unsigned short i);

// in tyrsound_dyn.cpp
void *dynopen(const char *fn);
void dynclose(void *);
void *dynsym(void *, const char *name);

// Helper for static initialization
template <typename T, int SZ> class RegistrationHolder
{
public:
    RegistrationHolder() : _idx(0) {}
    static void Register(const T& item) { instance().add(item); }
    static const T& Get(size_t i) { return instance()._data[i]; }
    static size_t Size() { return instance()._idx; }

private:
    static RegistrationHolder<T, SZ>& instance()
    {
        static RegistrationHolder<T, SZ> holder;
        return holder;
    }
    void add(const T& item) { _data[_idx++] = item; }
    T _data[SZ];
    size_t _idx;
};


#include "tyrsound_end.h"
#endif

