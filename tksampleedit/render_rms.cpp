// ----
// ---- file   : render_rms.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2010-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   :
// ----
// ---- created: 11May2010
// ---- changed: 25Feb2011, 07Mar2014, 23Feb2018, 22Dec2023, 13Sep2025, 18Sep2025
// ----
// ----
// ----

#include "tksampleedit.h"

#ifdef USE_TKMINNIE
#include "../minnie/tkminnie/tkminnie_shared.h"
#else
#include "../tkopengl/tkopengl_shared.h"
#endif // USE_TKMINNIE

#define RMS_WINDOW_SIZE 16
//#define RMS_WINDOW_SIZE 8
//#define RMS_WINDOW_SIZE 32

static sF32 rms_weights[RMS_WINDOW_SIZE];


void tksampleedit_rms_init(void) {
   sSI i;
   sF32 a = 0.0f;
   sF32 w = (sF32) (sM_PI * (1.0f/RMS_WINDOW_SIZE));
   sF32 sum = 0.0f;

   for(i=0; i<RMS_WINDOW_SIZE; i++)
   {
      sF32 c = sinf(a);
      rms_weights[i] = c;
      sum += c;
      a += w;
   }

   sF32 nscl = (RMS_WINDOW_SIZE / sum);
   for(i=0; i<RMS_WINDOW_SIZE; i++)
   {
      rms_weights[i] *= nscl;
   }
}


static sF32 read_windowed_rms(sF32 *s, sSI off, sUI numCh, sUI maxSamples) {
   // (todo) use history buffer (sliding window)
   int i;
   float avg = 0.0f;

   for(i=0; i<RMS_WINDOW_SIZE; i++)
   {
      sSI k = (off+i-((RMS_WINDOW_SIZE/2)-1)) * numCh;

      if(k >= 0)
      {
         if(((sUI)k) < maxSamples)
         {
            avg += s[k] * s[k] * rms_weights[i];
         }
      }
   }

   return sqrtf(avg / RMS_WINDOW_SIZE);
}


static void render_sample_rms(sF32 *s, sUI numCh, sUI numSamples, sUI maxSamples, sF32 _w, sF32 _h, sF32 _min, sF32 _max, sF32 _offY) {
   float cx, cy;

   // // yac_host->printf("xxx render_sample_skip_sh #ch=%u #samples=%u w=%f h=%f\n", numCh, numSamples, _w, _h);

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_BeginFilledTriangleStrip(TKMINNIE_MAX_POINTS*2);
#else
   // // tkopengl_shared->_glDisable(GL_CULL_FACE);
   tkopengl_shared->_glBegin(GL_TRIANGLE_STRIP);
#endif // USE_TKMINNIE

   sF32 off = 0.0f;
   sF32 spd = numSamples / _w;
   sSI num = (sSI) spd;

   sF32 scly = (_h / (_max - _min));

   cx = 0.0f;

   if(spd < 1.0f)
   {
      while(cx < _w)
      {
         cy = read_windowed_rms(s, (sSI)off, numCh, maxSamples);

         // printf("xxx cy=%f top=%f bottom=%f\n",
         //        cy,
         //        (_h - ( (cy - _min) * scly )),
         //        (_h - ( (-cy - _min) * scly ))
         //        );

#ifdef USE_TKMINNIE
         tkminnie_shared->sdvg_Vertex2f(cx,
                                        _h - ( (cy - _min) * scly ) + _offY
                                        );
         tkminnie_shared->sdvg_Vertex2f(cx,
                                        _h - ( (-cy - _min) * scly ) + _offY
                                        );
#else
         tkopengl_shared->_glVertex2f(cx,
                                      _h - ( (cy - _min) * scly ) + _offY
                                      );
         tkopengl_shared->_glVertex2f(cx,
                                      _h - ( (-cy - _min) * scly ) + _offY
                                      );
#endif // USE_TKMINNIE

         off += spd;
         cx += 1.0f;
      }
   }
   else
   {
      num++;
      if(num > 32)
         num = 32;

      while(cx < _w)
      {
         sF32 cOff = off - (spd*0.5f);
         sSI k;
         sF32 avg = 0.0f;
         sF32 cw = spd / num;
         for(k = 0; k < num; k++)
         {
            avg += read_windowed_rms(s, (sSI)cOff, numCh, maxSamples);
            cOff += cw;
         }
         avg /= (sF32)num;

#ifdef USE_TKMINNIE
         tkminnie_shared->sdvg_Vertex2f(cx,
                                        _h - ( (avg - _min) * scly ) + _offY
                                        );
         tkminnie_shared->sdvg_Vertex2f(cx,
                                        _h - ( (-avg - _min) * scly ) + _offY
                                        );
#else
         tkopengl_shared->_glVertex2f(cx,
                                      _h - ( (avg - _min) * scly ) + _offY
                                      );
         tkopengl_shared->_glVertex2f(cx,
                                      _h - ( (-avg - _min) * scly ) + _offY
                                      );
#endif // USE_TKMINNIE

         off += spd;
         cx += 1.0f;
      }
   }

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_End();
#else
   tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
}

void YAC_CALL tksampleedit_render_rms(YAC_Object *_data, sUI _numCh,
                                      sUI _sampleOffset, sUI _numSamples,
                                      sF32 _w, sF32 _h, sF32 _min, sF32 _max
                                      )
{
   //
   // Render sample RMS (range) at the given zoom level, using OpenGL.
   //

   ////yac_host->printf("xxx tksampleedit_render_rms\n");

   if(YAC_Is_FloatArray(_data))
   {
      YAC_CAST_ARG(YAC_FloatArray, a, _data);

      if( (_numCh > 0) && (_numCh <= 2/*max interleaved channels*/) )
      {
         sUI maxSamples = a->num_elements; // do not divide by numCh!

         sUI numSamplesAvail = maxSamples / _numCh;

         if(_sampleOffset < numSamplesAvail)
         {
            if(_numSamples <= numSamplesAvail)
            {
               if( (_sampleOffset + _numSamples) <= numSamplesAvail)
               {
                  if( (_sampleOffset + _numSamples) >= _sampleOffset) // 32bit overflow
                  {
                     // Render samples
                     {
                        sF32 h = _h / _numCh;

#ifdef USE_TKMINNIE
                        tkminnie_shared_resolve();
#else
                        tkopengl_shared_resolve();
#endif // USE_TKMINNIE

                        sF32 offY = 0.0f;
                        for(sUI ch = 0u; ch < _numCh; ch++)
                        {
                           render_sample_rms(a->elements + (_sampleOffset * _numCh) + ch,
                                             _numCh, _numSamples,
                                             maxSamples - _sampleOffset,
                                             _w, h,
                                             _min, _max,
                                             offY
                                             );
                           offY += h;
                        }
                     }

                  }
                  else
                  {
                     yac_host->printf("[---] tksampleedit_render_rms: (_sampleOffset + _numSamples) < _sampleOffset ((%u+%u)=%u < %u)\n",
                                      _sampleOffset, _numSamples, (_sampleOffset+_numSamples), _sampleOffset);
                  }
               }
               else
               {
                  yac_host->printf("[---] tksampleedit_render_rms: (_sampleOffset + _numSamples) > numSamplesAvail ((%u+%u)=%u >= %u)\n",
                                   _sampleOffset, _numSamples, (_sampleOffset+_numSamples), numSamplesAvail);
               }
            }
            else
            {
               if(yac_host->yacGetDebugLevel() >= 2u)
                  yac_host->printf("[---] tksampleedit_render_rms: _numSamples > numSamplesAvail (%u >= %u)\n", _numSamples, numSamplesAvail);
            }
         }
         else
         {
            yac_host->printf("[---] tksampleedit_render_rms: _sampleOffset >= numSamplesAvail (%u >= %u)\n", _sampleOffset, numSamplesAvail);
         }
      }

   }
}

sF32 YAC_CALL tksampleedit_calc_rms(YAC_Object *_fa, sUI _numCh) {
   if(YAC_Is_FloatArray(_fa))
   {
      YAC_CAST_ARG(YAC_FloatArray, a, _fa);

      if( (_numCh > 0) && (_numCh <= 2/*max interleaved channels*/) )
      {
         sUI numFrames = a->num_elements / _numCh;

         if(numFrames > 0)
         {
            sF32 rms = 0.0f;
            sUI k = 0;
            for(sUI i=0; i<numFrames; i++, k+=_numCh)
            {
               rms += a->elements[k] * a->elements[k];
            }
            rms = sqrtf(rms / numFrames);

            rms += 0.01f;
            rms -= 0.01f;

            return rms;
         }
      }
   }
   return 0.0f;
}
