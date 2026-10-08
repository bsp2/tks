/// UDPPacket.h
///
/// (c) 2002-2008 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_UDPPACKET_H__
#define __TKSDL_UDPPACKET_H__

YG("network");


/* @class UDPPacket,Object
Represents a user datagram protocol packet
*/
YC class _UDPPacket : public YAC_Object {
public:
	UDPpacket   sdlnet_packet;
	YAC_Buffer *netbuf;
public:
	_UDPPacket(void);
	~_UDPPacket();

	YAC(_UDPPacket);


   /* @method setChannel,int ch
   */
	YM void        _setChannel (sSI _nr);


   /* @method getChannel:int
   */
	YM sSI         _getChannel (void);


   /* @method setData,Buffer netbuf
   */
	YM void        _setData    (YAC_Object *_netbuf);


   /* @method getData:Buffer
   */
	YM YAC_Object *_getData    (void);


   /* @method getLength:int
   */
	YM sSI         _getLength  (void);


   /* @method setLength,int len
   */
	YM void        _setLength  (sSI);


   /* @method getStatus:int
   */
	YM sSI         _getStatus  (void);


   /* @method setAddress,IPAddress ip
   */
	YM void        _setAddress (_IPAddress *_ipadr);


   /* @method getAddress:IPAddress
   */
	YM void        _getAddress (YAC_Value *_r);
};


#endif // __TKSDL_UDPPACKET_H__

