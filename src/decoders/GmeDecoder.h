#ifndef TYRSOUND_GME_DECODER_H
#define TYRSOUND_GME_DECODER_H

#include "tyrDecoderBase.h"

#include "tyrsound_begin.h"


class GmeDecoder : public DecoderBase
{
protected:
    GmeDecoder(void *state, const tyrsound_Format& fmt);
    virtual ~GmeDecoder();

public:
    static GmeDecoder *create(const tyrsound_Format& fmt, tyrsound_Stream strm);
    static void staticInit() {}
    static void staticShutdown() {}
    static bool checkMagic(const unsigned char *magic, size_t size);

    virtual size_t fillBuffer(void *buf, size_t size);
    virtual float getLength();
    virtual tyrsound_Error seek(float seconds);
    virtual float tell();
    virtual tyrsound_Error setLoop(float seconds, int loops);
    virtual float getLoopPoint();
    virtual bool isEOF();
    virtual void getFormat(tyrsound_Format *fmt);

private:

    void *_emu;
    tyrsound_Format _fmt;
    float _loopPoint;
    int _loopCount;
    float _totaltime;
    bool _eof;
};



#include "tyrsound_end.h"
#endif
