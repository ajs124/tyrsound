#include "tyrsound_internal.h"
#include "SoundObject.h"
#include "tyrDecoderBase.h"
#include "tyrChannelBase.h"

#include "tyrsound_begin.h"

SoundObject *SoundObject::create(DecoderBase *decoder, ChannelBase *output)
{
    void *p = Alloc(sizeof(SoundObject));
    return new(p) SoundObject(decoder, output);
}

void SoundObject::destroy()
{
    this->~SoundObject();
    Free(this);
}

SoundObject::~SoundObject()
{
    stop();
    _decoder->destroy();
}

SoundObject::SoundObject(DecoderBase *decoder, ChannelBase *channel)
: _idxInStore(unsigned(-1))
, _decoder(decoder)
, _channel(channel)
, _dead(false)
{
}

void SoundObject::update()
{
    _channel->update();
    while(_channel->wantData())
    {
        void *buf = NULL;
        size_t size = 0;
        _channel->getBuffer(&buf, &size);
        if(buf && size)
        {
            size_t filled = _decoder->fillBuffer(buf, size);
            tyrsound_Format fmt;
            _decoder->getFormat(&fmt);
            tyrsound_Error err = _channel->filledBuffer(filled, fmt);
            if(err != TYRSOUND_ERR_OK)
            {
                breakpoint();
                return;
            }
        }
        if(_decoder->isEOF())
            break;
    }
}

tyrsound_Error SoundObject::setVolume(float vol)
{
    return _channel->setVolume(vol);
}

tyrsound_Error SoundObject::setSpeed(float speed)
{
    return _channel->setSpeed(speed);
}

tyrsound_Error SoundObject::play()
{
    return _channel->play();
}

tyrsound_Error SoundObject::stop()
{
    return _channel->stop();
}

tyrsound_Error SoundObject::pause()
{
    return _channel->pause();
}

tyrsound_Error SoundObject::seek(float seconds)
{
    return _decoder->seek(seconds);
}

float SoundObject::tell()
{
    return _decoder->tell();
}

tyrsound_Error SoundObject::setLoop(float seconds, int loops)
{
    return _decoder->setLoop(seconds, loops);
}

float SoundObject::getLength()
{
    return _decoder->getLength();
}

bool SoundObject::isPlaying()
{
    return _channel->isPlaying();
}

float SoundObject::getPlayPosition()
{
    return _channel->getPlayPosition();
}

tyrsound_Error SoundObject::setPosition(float x,  float y, float z)
{
    return _channel->setPosition(x, y, z);
}

#include "tyrsound_end.h"
