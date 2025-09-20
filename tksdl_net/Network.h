/// Network.h
///
/// (c) 2002-2008 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_NETWORK_H__
#define __TKSDL_NETWORK_H__

YG("network");


/* @class Network,Object
Static wrapper class for the SDL_net networking functions.
*/
YCS class _Network : public YAC_Object {
public:

   /* @constant INADDR_ANY,int
   */
//#define INADDR_ANY  YCI 0

   
   /* @constant INADDR_NONE,int
   */
//#define INADDR_NONE YCI 0xFFFFFFFF

public:
	SDLNet_SocketSet sdlnet_socketset;
public:
	_Network(void);
	~_Network();

	YAC(_Network);


   /* @method getError:String
   Return last error String.

   This is a wrapper for the SDLNet_GetError() native function.
   */
	YM void _getError       (YAC_Value *_r);


   /* @method resolveHost,String name,int port:IPAddress
   Resolve hostname and port and return IPAddress object

   This is a wrapper function for the SDLNet_ResolveHost() native function.

   @return null if DNS lookup failed, §IPAddress object otherwise
   */
	YM void _resolveHost    (YAC_Object *_name, sSI _port, YAC_Value *_r);


   /* @method resolveIP,IPAddress addr:String
   Reverse DNS lookup, return hostname for IPV4 address

   This is a wrapper function for the SDLNet_ResolveIP() native function.
   */
	YM void _resolveIP      (YAC_Object *_ipadr, YAC_Value *_r);


   /* @method allocSocketSet,int num:boolean
   Create a socket set that will be able to watch up to a given amount of sockets. 

   This is a wrapper function for the SDLNet_AllocSocketSet() native function.

   The resulting SDLNet_SocketSet will be stored internally.
   */
	YM sSI  _allocSocketSet (sSI _num);


   /* @method freeSocketSet
   Free (internal) socket set
   */
	YM void _freeSocketSet  (void);


   /* @method addSocket,Object socket
   Add TCP/UDP socket to (internal) socket set

   This is a wrapper for the SDLNet_TCP_AddSocket() native function.
   */
	YM sSI  _addSocket      (YAC_Object *_socket);


   /* @method addTCPSocket,TCPSocket socket
   Add TCP socket to (internal) socket set

   This is a wrapper for the SDLNet_TCP_AddSocket() native function.
   */
	YM sSI  _addTCPSocket   (YAC_Object *_socket);


   /* @method addUDPSocket,UDPSocket socket
   Add UDP socket to (internal) socket set

   TODO: implementation missing
   */
	YM sSI  _addUDPSocket   (YAC_Object *_socket);


   /* @method removeSocket,Object socket
   Remove TCP or UDP socket from (internal) socket set

   This is a wrapper for the SDLNet_TCP_DelSocket() native function.
   */
	YM sSI  _removeSocket   (YAC_Object *_socket);


   /* @method checkSockets,int timeoutMilliSec
   Check all sockets in the (internal) socket set for activity. 

   This is a wrapper for the SDLNet_CheckSockets() native function.
   */
	YM sSI  _checkSockets   (sSI _timeout);
};

#endif
