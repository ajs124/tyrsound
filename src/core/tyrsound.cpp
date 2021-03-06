#include <cstdarg>
#include <cstdio>

#include "tyrsound_internal.h"
#include "tyrDeviceBase.h"

#include "tyrsound_begin.h"

// Global state
static tyrsound_Alloc s_alloc = NULL;
static void *s_alloc_user = NULL;

static void *(*s_newMutexFunc)(void) = NULL;
static void (*s_deleteMutexFunc)(void*)= NULL;
static int (*s_lockMutexFunc)(void*) = NULL;
static void (*s_unlockMutexFunc)(void*) = NULL;

static void *s_msgPtr = NULL;
#if !TYRSOUND_IS_DEBUG
static tyrsound_MessageCallback s_msgCallback = NULL;
#else
static void _debugMsg(tyrsound_MessageSeverity severity, const char *str, void *user)
{
    if(severity >= TYRSOUND_MSG_DEBUG)
        printf("tyrsound[%d]: %s\n", severity, str);
}
static tyrsound_MessageCallback s_msgCallback = _debugMsg;
#endif

#include "tyrsound_end.h"

extern "C" {

void *tyrsound_ex_alloc(void *ptr, size_t size)
{
    if(tyrsound::s_alloc)
        return tyrsound::s_alloc(ptr, size, tyrsound::s_alloc_user);

    return realloc(ptr, size);
}

int tyrsound_ex_hasMT()
{
    return tyrsound::s_newMutexFunc && tyrsound::s_deleteMutexFunc && tyrsound::s_lockMutexFunc && tyrsound::s_unlockMutexFunc;
}

void *tyrsound_ex_newMutex()
{
    return tyrsound::s_newMutexFunc ? tyrsound::s_newMutexFunc() : NULL;
}

void tyrsound_ex_deleteMutex(void *mtx)
{
    if(tyrsound::s_deleteMutexFunc)
        tyrsound::s_deleteMutexFunc(mtx);
}

int tyrsound_ex_lockMutex(void *mtx)
{
    return tyrsound::s_lockMutexFunc ? tyrsound::s_lockMutexFunc(mtx) : (mtx ? TYRSOUND_ERR_NOT_READY : TYRSOUND_ERR_INVALID_HANDLE);
}

void tyrsound_ex_unlockMutex(void *mtx)
{
    if(tyrsound::s_unlockMutexFunc)
        tyrsound::s_unlockMutexFunc(mtx);
}

void *tyrsound_ex_loadLibrary(const char *name)
{
    return tyrsound::dynopen(name);
}

void tyrsound_ex_unloadLibrary(void *h)
{
    return tyrsound::dynclose(h);
}

void *tyrsound_ex_loadFunction(void *h, const char *name)
{
    return tyrsound::dynsym(h, name);
}

void tyrsound_ex_message(tyrsound_MessageSeverity severity, const char *str)
{
    if(tyrsound::s_msgCallback)
        tyrsound::s_msgCallback(severity, str, tyrsound::s_msgPtr);
}

void tyrsound_ex_messagef(tyrsound_MessageSeverity severity, const char *fmt, ...)
{
    if(tyrsound::s_msgCallback)
    {
        const int BUFSIZE = 1024;
        char buf[BUFSIZE];
        va_list va;
        va_start(va, fmt);
#ifdef _MSC_VER
        vsnprintf_s(&buf[0], BUFSIZE-1, _TRUNCATE, fmt, va);
#else
        vsnprintf(&buf[0], BUFSIZE-1, fmt, va);
#endif
        va_end(va);
        tyrsound::s_msgCallback(severity, buf, tyrsound::s_msgPtr);
    }
}

tyrsound_Error tyrsound_init(const tyrsound_Format *fmt, const char *output)
{
    if(tyrsound::getDevice())
    {
        tyrsound_ex_message(TYRSOUND_MSG_ERROR, "Already initialized");
        return TYRSOUND_ERR_UNSPECIFIED;
    }

#if TYRSOUND_IS_DEBUG
    tyrsound_ex_message(TYRSOUND_MSG_INFO, "This is a debug build");
#endif

    bool haveDevice = false;

    if(!output || !*output)
    {
        haveDevice = tyrsound::initDevice(NULL, fmt);
    }
    else
    {
        char buf[32];
        const char *next, *prev = output;
        while(true)
        {
            next = strchr(prev, ' ');
            unsigned int len = tyrsound::Min<unsigned int>(next - prev, sizeof(buf) - 2);
            memcpy(buf, prev, len);
            buf[len+1] = 0;
            prev = next + 1;

            if(tyrsound::initDevice(&buf[0], fmt))
            {
                haveDevice = true;
                break;
            }

            if(!next)
                break;
        }
    }

    if(!haveDevice)
    {
        tyrsound_ex_message(TYRSOUND_MSG_ERROR, "No usable device");
        return TYRSOUND_ERR_NO_DEVICE;
    }

    tyrsound::initDecoders();

    return tyrsound::initSounds();
}

tyrsound_Error tyrsound_shutdown()
{
    tyrsound::shutdownSounds();
    tyrsound::shutdownDevice();
    tyrsound::shutdownDecoders();
    return TYRSOUND_ERR_OK;
}

tyrsound_Error tyrsound_setupMT(void *(*newMutexFunc)(void), void (*deleteMutexFunc)(void*), int (*lockFunc)(void*), void (*unlockFunc)(void*))
{
    tyrsound::s_newMutexFunc = newMutexFunc;
    tyrsound::s_deleteMutexFunc = deleteMutexFunc;
    tyrsound::s_lockMutexFunc = lockFunc;
    tyrsound::s_unlockMutexFunc = unlockFunc;

    return TYRSOUND_ERR_OK;
}

void tyrsound_setAlloc(tyrsound_Alloc allocFunc, void *user)
{
    tyrsound::s_alloc = allocFunc;
    tyrsound::s_alloc_user = user;
}



tyrsound_Error tyrsound_update(void)
{
    tyrsound::DeviceBase *device = tyrsound::getDevice();
    if(!device)
        return TYRSOUND_ERR_NO_DEVICE;

    device->update();
    tyrsound_Error err = tyrsound::updateSounds();
    return err;
}

} // end extern "C"
