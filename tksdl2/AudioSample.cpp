/// AudioSample.cpp
///
/// (c) 2000-2019 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// TODO: this does not really belong to the SDL bindings and should be moved to a separate plugin
///

#include <math.h>
#define YAC_PRINTF defined
#include <yac.h>

#include "inc_sdl.h"

extern YAC_Host *yac_host;
extern sUI clid_AudioSample;

#include "iff.h"
#include "Bitmap.h"
#include "AudioSample.h"

_AudioSample::_AudioSample() {
   frequency=44100.0f;
   base_frequency=440.0f;
   length=0;
   repeat_offset=0;
   repeat_length=0;
   offset=0;
   channels=0;
   stype=0;
   type=0;
   //fft_m=0;
   own_buffer=0;
   buffer=0;
   //avifileaudio=0;
}

_AudioSample::~_AudioSample() {
   deleteBuffer();
}  

void _AudioSample::getNextLoop(sU16 *_loopi, sU32 *_off, sU32 *_len) {
   *_off=repeat_offset;
   *_len=repeat_length;
   (*_loopi)++;
}

sBool _AudioSample::allocBuffer(int s, int t, int l, sF32 f, int i, int sw, int fftwin ) {
   
   deleteBuffer();
   
   //    CLOG("allocBuffer: s=" << s
   //         << " t=" << t
   //         << " l=" << l
   //         << " f=" << f
   //         << " i=" << i
   //         << " sw=" << sw
   //         << " fftwin=" << fftwin
   //         << endl);
   
   tksvec2i bitmap_size;
   int z;;
   switch(s)
   {
   case Sample8:
      z=1;
      break;
   default:
   case Sample16:
      z=2;
      break;
   case Sample24:
      z=4;
      break;
   case Sample32:
      z=4;
      break;
   case SampleFloat:
      z=4;
      break;
   case SampleDouble:
      z=8;
      break;
   }
   
   if(sw<=0)
      sw=1;
   
   switch(t) {
   default:
   case TD_RAW:
      bitmap_size.x=l*i;
      bitmap_size.y=sw;
      break;
   case FD_RAW:
      bitmap_size.x=((1<<fftwin)*i)>>1;
      bitmap_size.y=l;
      break;
   case TD_AUDIO:
      return 0;//break;
   case FD_AUDIO:
      return 0;//break;
   case TD_VIRTUAL:
   case FD_VIRTUAL:
      return 0;//break;
   }
   
   stype=s;
   type=t;
   fft_m=fftwin;
   channels=i;
   frequency=f;
   offset=0;
   length=l;
   repeat_offset=0;
   repeat_length=0;
   
   buffer=new Bitmap();
   if(buffer)
   {
      own_buffer=1;
      //        CLOG("bitmap_size.x=" << bitmap_size.x
      //  	   <<  "bitmap_size.y=" << bitmap_size.y
      //  	   << " z=" << z
      //  	   << endl);
      return buffer->alloc(bitmap_size, z);
   }
   else
   {
      yac_host->printf("[---] AudioSample::allocBuffer: PANIC: failed to allocate buffer object\n");
   }
   return 0;
}

void _AudioSample::deleteBuffer() {
   if(buffer&&own_buffer)
   {
      delete buffer;
   }
   buffer=0;
   own_buffer=0;
}


void _AudioSample::setBuffer(Bitmap *b) {
   deleteBuffer();
   buffer=b;
}


sBool _AudioSample::loadAIFFFile(sBool _local, YAC_String *_fname) {
   
#define AIFF_FORM 0x464f524d //'FORM'
#define AIFF_AIFF 0x41494646 //'AIFF'
#define AIFF_AIFC 0x41494643 //'AIFC'
#define AIFF_COMM 0x434f4d4d //'COMM'
#define AIFF_SSND 0x53534e44 //'SSND'
   
   struct aiff_comm {
      unsigned short numChannels;
      unsigned long numSampleFrames;
      unsigned short sampleSize;
      unsigned short sampleRate;
   };
   
   struct aiff_comm comm;
   static sU8 buf[1024];
   
   int chunk_name;
   int chunk_size;
   int form_size;
   int i;
   bool got_buffer=0;
   unsigned int ssnd_len;
   mem t;
   
   YAC_Object *sio=yac_host->yacNewByID(YAC_CLID_PAKFILE);
   sBool isopen;
   if(_local)
      isopen=sio->yacStreamOpenLocal((sChar*)_fname->chars, 0);
   else
      isopen=sio->yacStreamOpenLogic((sChar*)_fname->chars);
   if(isopen)
   {
      //ifs.open((char *)fname->chars, ios::in);
      if(sio->yacStreamRead(buf, 256)>0)
      {
         //ifs.read((void *)buf, 256);
         getI4(buf, 0, chunk_name);
         if( chunk_name==AIFF_FORM )
         {
            getI4(buf, 4, form_size);
            
            for(i=8; i<256; i+=2)
            {
               getI4(buf, i, chunk_name);
               switch(chunk_name)
               {
               case AIFF_AIFF:
                  break;
               case AIFF_AIFC:
                  yac_host->printf("[---] AudioSample::loadAIFFFile: cannot load compressed AIFF-C file \"%s\".\n",
                     (char*)_fname->chars);
                  break;
               case AIFF_COMM:
                  getI4(buf, i+4, chunk_size);
                  getI2(buf, i+8, comm.numChannels);
                  getI4(buf, i+10, comm.numSampleFrames);
                  getI2(buf, i+14, comm.sampleSize);
                  getI2(buf, i+18, comm.sampleRate);
                  if(yac_host->yacGetDebugLevel()>1)
                     yac_host->printf("[...] AudioSample::loadAIFFFile: numChannels=%i\n[...] AudioSample::loadAIFFFile: numSampleFrames=%i\n[...] AudioSample::loadAIFFFile: sampleSize=%i\n[...] AudioSample::loadAIFFFile: sampleRate=%i\n",
                     comm.numChannels,
                     comm.numSampleFrames,
                     comm.sampleSize,
                     comm.sampleRate);
                  
                  if( ((comm.sampleSize>>3)<<3)==comm.sampleSize )
                  {
                     if(allocBuffer( comm.sampleSize>>3, // 1,2,(3),4... XXX
                        TD_RAW,
                        comm.numSampleFrames,
                        comm.sampleRate,
                        comm.numChannels,
                        1, 0))
                     {
                        got_buffer=1;
                     }
                     else
                     {
                        yac_host->printf("[---] AudioSample::loadAIFFFile: cannot allocate buffer.\n");
                        sio->yacStreamClose();
                        yac_host->yacDelete(sio);
                        return 0;
                     }
                  }
                  else
                  {
                     yac_host->printf("[---] AudioSample::loadAIFFFile: unknown comm.sampleSize (%i)\n" ,
                        comm.sampleSize);
                     sio->yacStreamClose();
                     yac_host->yacDelete(sio);
                     return 0;
                  }
                  i+=chunk_size-2;
                  break;
                  
               case AIFF_SSND:
                  if(got_buffer)
                  {
                     getI4(buf, i+4, chunk_size);
                     if(buffer->s.x*buffer->z >= chunk_size)
                     {
                        ssnd_len=chunk_size;
                     }
                     else
                     {
                        ssnd_len=buffer->s.x*buffer->z;
                     }
                     sio->yacStreamSeek(i+16, YAC_BEG);
                     //ifs.seekg(i+16, ios::beg);
                     if(!sio->yacStreamGetErrorCode())
                     {
                        sio->yacStreamRead(buffer->data.u8, ssnd_len);
                        //ifs.read(buffer->data.any, ssnd_len);
                        if(!sio->yacStreamGetErrorCode())
                        {
                           if(yac_host->yacGetDebugLevel()>1)
                           {
                              yac_host->printf("[...] AudioSample::loadAIFFFile: %i/%i frames loaded.\n",
                                 ssnd_len/(buffer->z*channels),
                                 (chunk_size-8)/(buffer->z*channels));
                           }
#ifdef YAC_LITTLE_ENDIAN
                           switch(stype)
                           {
                           case Sample16:
                              {
                                 sU32 j=0;
                                 for(i=0; i< (int)ssnd_len; i+=buffer->z, j++)
                                 {
                                    getSI2(buffer->data.s8, i, t.s16);
                                    buffer->data.s16[j]=t.s16;
                                 }
                              }
                              break;
                              
                           case Sample32:
                              break;
                           case SampleFloat:
                              break;
                           }
#else
                           switch(stype)
                           {
                           case Sample16:
                              {
                                 sU32 j=0;
                                 for(i=0; i< (int)ssnd_len; i+=buffer->z, j++)
                                 {
                                    getSI2(buffer->data.s8, i, t.s16);
                                    buffer->data.s16[j]=t.s16;
                                    // swap byte order
                                    sU8 k=buffer->data.u8[j*2+0];
                                    buffer->data.u8[j*2+0]=buffer->data.u8[j*2+1];
                                    buffer->data.u8[j*2+1]=k;
                                 }
                              }
                              break;
                              
                           case Sample32:
                              break;
                           case SampleFloat:
                              break;
                           }
#endif
                           
                           sio->yacStreamClose();
                           yac_host->yacDelete(sio);
                           return 1;
                        }
                        else
                        {
                           yac_host->printf("[---] AudioSample::loadAIFFFile: ERROR: (read)\n");
                           sio->yacStreamClose();
                           yac_host->yacDelete(sio);
                           return 0;
                        }
                     }
                     else
                     {
                        yac_host->printf("[---] AudioSample::loadAIFFFile: ERROR: (seek)\n");
                        sio->yacStreamClose();
                        yac_host->yacDelete(sio);
                        return 0;
                     }
                  }
                  else
                  {
                     yac_host->printf("[---] AudioSample::loadAIFFFile: found SSND but no COMM chunk.\n");
                  }
                  break;
               default:
                  //yac_host->printf(".");
                  break;
                    } // switch chunk name
                } // for
            } // if chunk
        } // if read header
        sio->yacStreamClose();
    } // if open
    else
    {
       yac_host->printf("[---] AudioSample::loadAIFFFile: cannot open \"%s\".\n", (char*)_fname->chars);
    }
    yac_host->yacDelete(sio);
    return 0;
}


/*
bool AudioSample::loadAudiofile(char *fname) {
bool ret=0;
#ifdef DX_AUDIOFILE
AFfilesetup fsetup=afNewFileSetup();
AFfilehandle fh=afOpenFile(fname, "r", fsetup);
if(fh)
{
int version;
int fmt=afGetFileFormat(fh, &version);
if(fmt>0)
{
CLOG("loading audiofile \"" << fname << "\"\n");
int smp_fmt;
int smp_width;
afGetSampleFormat(fh, AF_DEFAULT_TRACK, &smp_fmt, &smp_width);

 float frsz=afGetFrameSize(fh, AF_DEFAULT_TRACK, 1);
 
  int s=Sample8;
  if(smp_fmt==AF_SAMPFMT_TWOSCOMP && smp_width==8)
  s=Sample8;
  if(smp_fmt==AF_SAMPFMT_TWOSCOMP && smp_width==16)
  s=Sample16;
  if(smp_fmt==AF_SAMPFMT_TWOSCOMP && smp_width==24)
  s=Sample24;
  if(smp_fmt==AF_SAMPFMT_TWOSCOMP && smp_width==32)
  s=Sample32;
  if(smp_fmt==AF_SAMPFMT_FLOAT && smp_width==32)
  s=SampleFloat;
  if(smp_fmt==AF_SAMPFMT_DOUBLE && smp_width==64)
  s=SampleDouble;
  
	  float f=afGetRate(fh, AF_DEFAULT_TRACK);
     
      int i=afGetChannels(fh, AF_DEFAULT_TRACK);
      
       if((int)frsz!=s*i)
       {
	      CLOG("loadAudiofile: warning: framesize!=s "
         << "(framesize=" << frsz 
         << " , s=" << s << ")\n");
         }
         
          int bytes=afGetTrackBytes(fh, AF_DEFAULT_TRACK);
          
           int l=(bytes/((int)frsz<=0 ? 1 : (int) frsz));
           
            CLOG("  file_format=" << (char *) (
            fmt==AF_FILE_AIFFC ? "AIFFC" :
            fmt==AF_FILE_AIFF ? "AIFF" :
            fmt==AF_FILE_NEXTSND ? "NEXT" :
            fmt==AF_FILE_WAVE ? "WAV" :
            fmt==AF_FILE_BICSF ? "BICSF" :
            fmt==AF_FILE_MPEG1BITSTREAM ? "MPEG1" :
            fmt==AF_FILE_SOUNDDESIGNER1 ? "SOUNDDESIGNER1" :
            fmt==AF_FILE_SOUNDDESIGNER2 ? "SOUNDDESIGNER2" :
            fmt==AF_FILE_AVR ? "AVR" :
            fmt==AF_FILE_IFF_8SVX ? "IFF 8SVX" :
            fmt==AF_FILE_SAMPLEVISION ? "SAMPLEVISION" :
            fmt==AF_FILE_VOC ? "VOC" :
            fmt==AF_FILE_NIST_SPHERE ? "NIST_SPHERE" :
            fmt==AF_FILE_SOUNDFONT2 ? "SF2" : "(recognized)"
            )
            << "\n  version=" << version 
            << "\n  format=" << (char *) (
            s==Sample8 ? "Sample8" :
            s==Sample16 ? "Sample16" :
            s==Sample24 ? "Sample24" :
            s==Sample32 ? "Sample32" :
            s==SampleFloat ? "SampleFloat" :
            s==SampleDouble ? "SampleDouble" : "??n/a??"
            )
            << "\n  smp_width=" << smp_width
            << "\n  channels=" << i << " ( " << (char *) (
            i==1 ? "Mono" :
            i==2 ? "Interleaved Stereo" :
            i==4 ? "Interleaved Quadro" :
            i==5 ? "Interleaved Surround" : "Interleaved"
            )
            << " )"
            << "\n  frequency=" << f
            << "\n  framesize=" << frsz 
            << "\n  frames= " << l
            << "\n  bytes=" << bytes
            << endl);
            
             if(allocBuffer(s,TD_RAW,l,f,i,1,0))
             {
             int fr_read= afReadFrames(fh, AF_DEFAULT_TRACK, buffer->data.any, l);
             CLOG("loadAudiofile: " << fr_read << "/" << l << " frames loaded.\n");
             if(fr_read==l)
             {
             CLOG("  afReadFrames: fr_read==l -> true.\n");
             }
             else
             {
             CLOG("  afReadFrames: fr_read==l -> FALSE.\n");
             }
             ret=1;
             }
             }
             else
             {
             CLOG("loadAudiofile: Unknown format.\n");
             }
             afCloseFile(fh);
             }
             #endif
             return ret;
             }
*/

void _AudioSample::generateSinus(int s, int l, sF32 freq, sF32 frate) {
   if(allocBuffer(s, TD_RAW, l, frate, 1, 1, 0))
   {
      yacmemptr d=buffer->data;
      int i;
      double sinspeed= (((double)freq)*2.0*sM_PI)/frate;
      double sinangle=0.0;
      switch(s)
      {
      case Sample8:
         for(i=0; i<l; i++)
         {
            d.s8[i]=cAFloat28(::sin(sinangle));
            sinangle+=sinspeed;
         }
         break;
      case Sample16:
         for(i=0; i<l; i++)
         {
            d.s16[i]=cAFloat216(sin(sinangle));
            sinangle+=sinspeed;
         }
         break;
      case Sample24:
         for(i=0; i<l; i++)
         {
            d.s32[i]=cAFloat224(sin(sinangle));
            sinangle+=sinspeed;
         }
         break;
      case Sample32:
         for(i=0; i<l; i++)
         {
            d.s32[i]=cAFloat232(sin(sinangle));
            sinangle+=sinspeed;
         }
         break;
      case SampleFloat:
         for(i=0; i<l; i++)
         {
            d.f32[i]=(sF32)sin(sinangle);
            sinangle+=sinspeed;
         }
         break;
      }
   }
}

void _AudioSample::generatePulse(int s, int l, sF32 pw, sF32 freq, sF32 frate) {
   if(allocBuffer(s, TD_RAW, l, frate, 1, 1, 0))
   {
      yacmemptr d=buffer->data;
      int i;
      int j;
      int wlen= (int) (frate/freq);
      int plen= (int) (((frate/freq)*pw)/(2.0*sM_PI));
      switch(s)
      {
      case Sample8:
         for(i=0; i<l; i++)
         {
            for(j=0; j<plen&&i<l; j++)
            {
               d.s8[i++]=127;
            }
            for(; j<wlen&&i<l; j++)
               d.s8[i++]= -128;
         }
         break;
      case Sample16:
         for(i=0; i<l; i++)
         {
            for(j=0; j<plen&&i<l; j++)
            {
               d.s16[i++]=32767;
            }
            for(; j<wlen&&i<l; j++)
               d.s16[i++]= -32768;
         }
         break;
      case Sample24:
         for(i=0; i<l; i++)
         {
            for(j=0; j<plen&&i<l; j++)
            {
               d.s32[i++]= 0x7fffff;
            }
            for(; j<wlen&&i<l; j++)
               d.s32[i++]= 0x800000;
         }
         break;
      case Sample32:
         for(i=0; i<l; i++)
         {
            for(j=0; j<plen&&i<l; j++)
            {
               d.s32[i++]=0x7ffffff;
            }
            for(; j<wlen&&i<l; j++)
               d.s32[i++]= 0x8000000;
         }
         break;
      case SampleFloat:
         for(i=0; i<l; i++)
         {
            for(j=0; j<plen&&i<l; j++)
            {
               d.f32[i++]=1.0;
            }
            for(; j<wlen&&i<l; j++)
               d.f32[i++]= -1.0;
         }
         break;
      }
   }
}

void _AudioSample::generateSilence(int s, int l, int c, sF32 frate) {
   if(allocBuffer(s, TD_RAW, l, frate, c, 1, 0))
   {
      int i;
      yacmemptr d=buffer->data;
      switch(s)
      {
      case Sample8:
         for(i=0; i<buffer->s.x; i++)
            *d.s8++=0;
         break;
      case Sample16:
         for(i=0; i<buffer->s.x; i++)
            *d.s16++=0;
         break;
      case Sample24:
      case Sample32:
         for(i=0; i<buffer->s.x; i++)
            *d.s32++=0;
         break;
      case SampleFloat:
         for(i=0; i<buffer->s.x; i++)
            *d.f32++=0;
         break;
      }
   }
}


sSI _AudioSample::_convertChannels(sSI ch) {
   sBool ret=0;
   if(channels&&ch!=channels)
   {
      Bitmap *obuf=buffer;
      if(obuf)
      {
         own_buffer=0;
         sBool o_ownbuffer=own_buffer;
         int o_channels=channels;
         if(allocBuffer(stype, type, length, frequency, ch, 1, 0))
         {
            int i;
            int j;
            yacmemptr s=obuf->data;
            yacmemptr d=buffer->data;
            
            if(ch>o_channels)
            {
               //  		  CLOG("create channels\n");
               // create new channels
               switch(stype)
               {
               case Sample8:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<o_channels; j++)
                        *d.s8++=*s.s8++;
                     for(; j<ch; j++)
                        *d.s8++=0;
                  }
                  ret=1;
                  break;
               case Sample16:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<o_channels; j++)
                        *d.s16++=*s.s16++;
                     for(; j<ch; j++)
                        *d.s16++=0;
                  }
                  ret=1;
                  break;
               case Sample24:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<o_channels; j++)
                        *d.s32++=*s.s32++;
                     for(; j<ch; j++)
                        *d.s32++=0;
                  }
                  ret=1;
                  break;
               case Sample32:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<o_channels; j++)
                        *d.s32++=*s.s32++;
                     for(; j<ch; j++)
                        *d.s32++=0;
                  }
                  ret=1;
                  break;
               case SampleFloat:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<o_channels; j++)
                        *d.f32++=*s.f32++;
                     for(; j<ch; j++)
                        *d.f32++=0;
                  }
                  ret=1;
                  break;
               }
            }
            if(o_channels>ch)
            {
               //  		  CLOG("delete channels\n");
               // delete channels		    
               switch(stype)
               {
               case Sample8:
                  break;
               case Sample16:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<ch; j++)
                        *d.s16++=*s.s16++;
                     s.s16+=o_channels-j;
                  }
                  ret=1;
                  break;
               case Sample24:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<ch; j++)
                        *d.s32++=*s.s32++;
                     s.s32+=o_channels-j;
                  }
                  ret=1;
                  break;
               case Sample32:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<ch; j++)
                        *d.s32++=*s.s32++;
                     s.s32+=o_channels-j;
                  }
                  ret=1;
                  break;
               case SampleFloat:
                  for(i=0; i<length; i++)
                  {
                     for(j=0; j<ch; j++)
                        *d.f32++=*s.f32++;
                     s.f32+=o_channels-j;
                  }
                  ret=1;
                  break;
               }
            }
        }
        else
        {
           own_buffer=o_ownbuffer;
           buffer=obuf;
        }
    }
    
    }
    return ret;
}

int _AudioSample::getTDMax(int sw) {
   int maxi=-1;
   
   if(buffer)
      if(type==TD_RAW||type==TD_VIRTUAL)
         if(buffer->data.any)
            if(sw<buffer->s.y)
            {
               int i=0;
               mem max_v;
               yacmemptr a=buffer->getData(0, sw);
               int n= length;
               switch(stype)
               {
               case Sample8:
                  max_v.s8=0;
                  for(; i<n ; i++)
                  {
                     if(*a.s8>max_v.s8)
                     {
                        max_v.s8=*a.s8;
                        maxi=i;
                     }
                     a.s8++;
                  }
                  break;
               case Sample16:
                  max_v.s16=0;
                  for(; i<n ; i++)
                  {
                     if(*a.s16>max_v.s16)
                     {
                        max_v.s16=*a.s16;
                        maxi=i;
                     }
                     a.s16++;
                  }
                  break;
               case Sample24:
                  max_v.s32=0;
                  for(; i<n ; i++)
                  {
                     if(*a.s32>max_v.s32)
                     {
                        max_v.s32=*a.s32;
                        maxi=i;
                     }
                     a.s32++;
                  }
                  break;
               case Sample32:
                  max_v.s32=0;
                  for(; i<n ; i++)
                  {
                     if(*a.s32>max_v.s32)
                     {
                        max_v.s32=*a.s32;
                        maxi=i;
                     }
                     a.s32++;
                  }
                  break;
               case SampleFloat:
                  max_v.f32=0;
                  for(; i<n ; i++)
                  {
                     if(*a.f32>max_v.f32)
                     {
                        max_v.f32=*a.f32;
                        maxi=i;
                     }
                     a.f32++;
                  }
                  break;
               }
            }
            else
            {
               yac_host->printf("[---] AudioSample::getTDMax: Illegal subwave (%i>%i).\n", (sw+1), buffer->s.y);
            }
            else
            {
               yac_host->printf("[---] AudioSample::getTDMax: Empty buffer!\n");
            }
            else
            {
               yac_host->printf("[---] AudioSample::getTDMax: Not a time domain buffer.\n");
            }
            else
            {
               yac_host->printf("[---] AudioSample::getTDMax: No buffer!\n");
            }
            return maxi;
}

void _AudioSample::normalize() {
   //    CLOG("normalize: " << channels << " channels.\n");
   if(buffer)
      if(buffer->data.any)
      {
         int sw;
         for(sw=0; sw<buffer->s.y; sw++)
         {
            int maxi=getTDMax(sw);
            if(maxi!=-1)
            {
               yacmemptr a=buffer->getData(0,sw);
               mem max_v;
               int i;
               switch(stype)
               {
               case Sample8:
                  max_v.s8=a.s8[maxi];
                  if(max_v.s8<0)
                     max_v.s16=(128*127)/(-max_v.s8);
                  else
                     max_v.s16=(128*127)/(  max_v.s8==0 ? 1 :  max_v.s8);
                  for(i=0; i<length*channels; i++)
                  {
                     *a.s8=(signed char)((*a.s8 * max_v.s16)>>7);
                     a.s8++;
                  }
                  break;
               case Sample16:
                  max_v.s16=a.s16[maxi];
                  if(max_v.s16<0)
                     max_v.s32=(32768*32767)/(-max_v.s16);
                  else
                     max_v.s32=(32768*32767)/(  max_v.s16==0 ? 1 :  max_v.s16);
                  for(i=0; i<length*channels; i++)
                  {
                     *a.s16=(signed short)((*a.s16 * max_v.s32)>>15);
                     a.s16++;
                  }
                  break;
               case Sample32:
                  max_v.s32=a.s32[maxi];
                  if(max_v.s32<0)
                     max_v.f32=(sF32)((2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*(2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0))/((sF32)-max_v.s32));
                  else
                     max_v.f32=(sF32)((2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*(2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0-1))/( max_v.s32==0 ? 1.0 : ((sF32)max_v.s32) ));
                  
                  for(i=0; i<length*channels; i++)
                  {
                     *a.s32=(signed long)((*a.s32 * max_v.f32)/(2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0));
                     a.s32++;
                  }
                  break;
               case SampleFloat:
                  max_v.f32=a.f32[maxi];
                  //  		    CLOG("normalize: max_v.f32=" << max_v.f32  <<
                  //  			 " maxi=" << maxi << endl);
                  if(max_v.f32<0)
                     max_v.f32=(sF32)(1.0/(-max_v.f32));
                  else
                     max_v.f32=(sF32)(1.0/(max_v.f32==0 ? 1.0 : max_v.f32));
                  //  		    CLOG("normalize: weight.f32=" << max_v.f32 << endl);		    
                  for(i=0; i<length*channels; i++)
                  {
                     *a.f32= *a.f32*max_v.f32;
                     a.f32++;
                  }
               }
            }
         }
      }
      else
      {
         yac_host->printf("[---] AudioSample::normalize: Empty buffer!\n");
      }
      else
      {
         yac_host->printf("[---] AudioSample::normalize: No buffer!\n");
      }
}


void YAC_VCALL _AudioSample::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_cmd==YAC_OP_ASSIGN)
   {
      if(YAC_CHK(_o, clid_AudioSample))
      {
         _AudioSample *o=(_AudioSample *)_o;
         setBuffer(o->buffer);
         stype=o->stype;
         channels=o->channels;
         frequency=o->frequency;
         offset=o->offset;
         length=o->length;
         repeat_offset=o->repeat_offset;
         repeat_length=o->repeat_length;
         fft_m=o->fft_m;
      }
   }
}

sSI _AudioSample::_allocBuffer(sSI _l, sF32 _f, sSI _ch, sSI _sw) {
   return allocBuffer(Sample16, TD_RAW, _l, _f, _ch, _sw, 7);
}

void _AudioSample::_freeBuffer(void) {
   deleteBuffer();
}

void _AudioSample::_visitBuffer(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_AudioSample))
   {
      _AudioSample *o=(_AudioSample*)_o;
      setBuffer(o->buffer);
      // xxx adjust length, repeatx??
   }
}

void _AudioSample::_normalize(void) {
   normalize();
}

sSI _AudioSample::_loadAIFF(YAC_String *_name) {
   return loadAIFFFile(0, _name);
}

sSI _AudioSample::_loadLocalAIFF(YAC_String *_name) {
   return loadAIFFFile(1, _name);
}

sSI _AudioSample::_getChannels(void) {
   return channels;
}

sSI _AudioSample::_getOffset(void) {
   return offset;
}

void _AudioSample::_setOffset(sSI _o) {
   offset=_o;
}

sSI _AudioSample::_getLength(void) { 
   return length;
}

void _AudioSample::_setLength(sSI _l) {
   length=_l;
}

sSI _AudioSample::_getRepeatOffset(void) { 
   return repeat_offset;
}

void _AudioSample::_setRepeatOffset(sSI _ro) {
   repeat_offset=_ro;
}

sSI _AudioSample::_getRepeatLength(void) { 
   return repeat_length;
}

void _AudioSample::_setRepeatLength(sSI _l) {
   repeat_length=_l;
}

sSI _AudioSample::_copyBufferFrom(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_AudioSample))
   {
      _AudioSample *o=(_AudioSample*)_o;
      if(o->buffer)
         return allocBuffer(o->stype, TD_RAW, o->buffer->s.x, o->frequency, o->channels, o->buffer->s.y, 7);
   }
   return 0;
}

void _AudioSample::_setFrequency(sF32 _f) {
   if(_f>0.0f && _f<=96000.0f)
      frequency=_f;
}

sF32 _AudioSample::_getFrequency(void) {
   return frequency;
}

void _AudioSample::_setBaseFrequency(sF32 _f) {
   if(_f>0.0f && _f<=32000.0f)
      base_frequency=_f;
}

sF32 _AudioSample::_getBaseFrequency(void) {
   return base_frequency;
}

