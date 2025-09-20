/// FPS.h
///
/// (c) 2001-2008 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef __TKSDL_FPS_H__
#define __TKSDL_FPS_H__

YG("hal1");


/* @class FPS,Object
 Utility class for measuring/controlling frames per second
*/
YCS class _FPS : public YAC_Object {
public:
	YAC(_FPS);

	 // ------------ exported methods ------------

   /* @method getCurrent:float
      Get current FPS (in Hz, extrapolated from last frame)
   */
	YM sF32 _getCurrent           (void);


   /* @method getAverage:float
      Get average FPS rate (Hz)
   */
	YM sF32 _getAverage           (void);


   /* @method reset
      Reset FPS counters.
   */
	YM void _reset                (void);

   
   /* @method setMax,float max
      Set maximum FPS rate (Hz).
      This affects the getPrecision() method
   */
	YM void _setMax               (sF32 _fps);


   /* @method getMax:float
      Query maximum FPS rate
   */
	YM sF32 _getMax               (void);


   /* @method getReal:float
      Return real interpolated frames per second
   */
	YM sF32 _getReal              (void);


   /* @method getPrecision:float
      Return current precision.

      This can be used to determine the deviation of the current FPS rate from the desired one (tickInterval).
      A value > 1 means that the rendering is slower than the desired rate, a value < 1 means that it is faster.
   */
	YM sF32 _getPrecision         (void);

   
   /* @method getTotalTime:int
      Get total time (millisec)
   */
	YM sSI  _getTotalTime         (void);


   /* @method getNumFramesRendered
      Query total number of frames rendered
   */
	YM sSI  _getNumFramesRendered (void);

   
   /* @method setTickInterval,float interval
      Set desired tick interval in millisec.

      This affects the return value of the getPrecision() method. It also determines how often the tick() signal callback is invoked.
   */
	YM void _setTickInterval      (sF32 _ival);


   /* @method getTickInterval:float
      Query current tick interval
   */
	YM sF32 _getTickInterval      (void);


   /* @method setTickBuffer,int size
      Set the maximum number of ticks to catch up to if the rendering is too slow.

      Mostly useful for constant timeframe physics.
   */
	YM void _setTickBuffer        (sSI _ival);

   
   /* @method getTickBuffer:int
      Query current maximum number of ticks to catch up
   */
	YM sSI  _getTickBuffer        (void);


   /* @method setLimit,int limit
      Set FPS limit (in Hz)

      This can be used to prevent the application from redrawing too often and thus can help to save CPU/GPU resources.

      @arg limit 0=unlimited, frame limit (Hz) otherwise
   */
	YM void _setLimit             (sUI);


   /* @method getLimit:int
      Query current FPS limit
   */
	YM sUI  _getLimit             (void);
};


#endif // __TKSDL_FPS_H__

