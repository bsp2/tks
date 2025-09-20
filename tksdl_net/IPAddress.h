/// IPAddress.h
///
/// (c) 2002-2006 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_IPADDRESS_H__
#define __TKSDL_IPADDRESS_H__


YG("network");

/* @class IPAddress,Object
Represents an IPV4 address
*/
YC class _IPAddress : public YAC_Object {
public:
	IPaddress sdlnet_ipadr;
public:
	_IPAddress(void);
	~_IPAddress();

	YAC(_IPAddress);

	void YAC_VCALL yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *);

   /* @method setHost,int addr
   Set host address

   Automatic byteorder conversion to big endian is performed on little endian platforms.
   */
	YM void _setHost       (sSI _ip4adr);


   /* @method getHost:int
   Query host address

   Automatic byteorder conversion to host byte order is performed.
   */
	YM sSI  _getHost       (void);


   /* @method setPort,int portnr
   Set port number
   */
	YM void _setPort       (sSI _portnr);


   /* @method getPort:int
   Query port number
   */
	YM sSI  _getPort       (void);


   /* @method setHostByName,String name
   Resolve name to IP address and set as host address
   */
	YM sSI  _setHostByName (YAC_String *_name);


   /* @method getHostName:String
   Query host name
   */
	YM void _getHostName   (YAC_Value *_r);


   /* @method init,int addr,int port
   Set host and port

   @see setHost
   @see setPort
   */
	YM void _init          (sSI _adr, sSI _port);


   /* @method getHostString:String
   Return IP address as string
   */
	YM void _getHostString (YAC_Value *_r);
};



#endif
