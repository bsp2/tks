/// tri_hspan.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///
///
#ifndef TKS_TRI_HSPAN_H__
#define TKS_TRI_HSPAN_H__

// (note) a simple, non-antialised hspan-based triangle rasterizer
// (note) originally written for the Amiga 500+1200

// #define HSPAN_COUNT_PX  defined

// (note) no discernable difference between 4,5 or 6 bits (in M68000 HSPAN_DIV_TBL code path)
#define FR_BITS 4
#define FR_HALF ((1 << (FR_BITS-1)))
#define FR_ONE  (1 << FR_BITS)

// (note) don't change. fixed point coordinates use 5 fractional bits.
#define FX_COORD_SCL_HSPAN 16.0f

#ifndef HSPAN_DIV_TBL
#define HSPAN_DIV_TBL 1
#endif // HSPAN_DIV_TBL

#ifndef HSPAN_TOPLEFT_FILLRULE
#define HSPAN_TOPLEFT_FILLRULE 1
#endif // HSPAN_TOPLEFT_FILLRULE

// 1=framebuffer boundary check
#ifndef HSPAN_SAFE
#define HSPAN_SAFE 0
#endif // HSPAN_SAFE

// (note) should be left undefined. no precision advantages.
/* #define HSPAN_FLOAT_STEPX defined */

#undef Dstatic
#ifdef AMIGA
// vbcc+gcc combo linker issue: can't use static vars or functions in gcc compiled code (unless they are inlined)
#define Dstatic 
#else
#define Dstatic static
#endif // AMIGA


#ifndef Dprintf
#define Dprintf if(1);else printf
#endif // Dprintf

#ifndef Dprintfmissing
#define Dprintfmissing if(1);else printf
#endif // Dprintfmissing

#ifndef Dprintfimpl
#define Dprintfimpl    if(1);else printf
#endif // Dprintfimpl

#ifdef AMIGA
// 3 interleaved bitplanes
#define Ddraw_hspan draw_hspan
#define HSPAN_FBPTR _bpl
#define HSPAN_FBPTR_ARG sU8 *HSPAN_FBPTR
#define HSPAN_COLOR _palIdx
#define HSPAN_MASK _palIdxMask
#define HSPAN_FBSZ
#define HSPAN_FBSZ_ARGS
#else
// ARGB32 framebuffer
#define Ddraw_hspan draw_hspan_simple 
#define HSPAN_FBPTR _fb
#define HSPAN_FBPTR_ARG sU32 *HSPAN_FBPTR 
#define HSPAN_COLOR _c32
#define HSPAN_MASK _c32Mask
#define HSPAN_FBSZ _fbW, _fbH,
#define HSPAN_FBSZ_ARGS sU32 _fbW, sU32 _fbH,
#endif // AMIGA

#if 0
// (todo) remove
#define XL_BIAS_FR     32
#define XR_BIAS_FR     64
#define XL_BIAS16   32768
#define XR_BIAS16   65536
#else
#define XL_BIAS_FR      0
#define XR_BIAS_FR      0
#define XL_BIAS16       0
#define XR_BIAS16       0
#endif

// (todo) remove
#define X_BIAS_FR 0   // 1


#if 0
#if 1 && defined(AMIGA)
// https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html
static inline short divsw(signed int a, short b) {
   // (note) 68000 divs.w: 158 cycles (Dn,Dn)
   // divide a by b
   asm("divs %[div],%[io]\n\t"
       : [io]  "+d" (a)
       : [div] "d" (b)
       : "cc"
       );
   return (short)a;  // set remainder to 0
}
#else
static inline short divsw(signed int a, short b) {
#ifndef AMIGA_LLVM
   if( (a / b) > 32767 )
      printf("xxx divsw overflow: a=%d b=%d => a/b=%d\n", a, b, (a/b));
#endif // AMIGA_LLVM

   return a / b;
}
#endif // AMIGA
#endif

/* (unused)
SMult32S:
UMult32S:
    move.l    D2,-(SP)    ; 
    move.l    D0,-(SP)    ; A
    mulu.w    D1,D0        ; D0=Al*Bl
    move.l    D1,D2        ; B
    mulu.w    (SP)+,D1    ; D1=Ah*Bl
    swap    D2        ; D2=Bh
    mulu.w    (SP)+,D2    ; D2=Al*Bh
    add.w    D2,D1        ;
    swap    D1        ;
    move.l    (SP)+,D2    ;
    clr.w    D1        ;
    add.l    D1,D0        ;
    rts
*/

#if 0
static inline sU32 mulul(sU32 a, sU32 b) {
   return (sU32)(a * b);
}
#endif // 0

#ifdef HSPAN_DIV_TBL

#ifdef AMIGA
static inline sU32 mulu_w(sU16 b, sU16 c) {
   sU32 a;
#if 0
   asm("muluw %2,%0":"=d"(a): "0"(c),"d"(b): "cc");
#else
   asm("muluw %2,%0"
       :"=d"(a)
       : "0"(c),"d"(b)
       : "cc"
       );
#endif
   return a;
}
#else
static inline sU32 mulu_w(sU16 b, sU16 c) {
   return b * c;
}
#endif // AMIGA

#ifdef AMIGA
Dstatic const sU16 loc_div_rows_tbl[240+1] = {
   0u, 1u, 32768u, 21845u, 16384u, 13107u, 10922u, 9362u, 8192u, 7281u, 6553u, 5957u, 5461u, 5041u, 4681u, 4369u, 4096u, 3855u, 3640u, 3449u, 3276u, 3120u, 2978u, 2849u, 2730u, 2621u, 2520u, 2427u, 2340u, 2259u, 2184u, 2114u, 2048u, 1985u, 1927u, 1872u, 1820u, 1771u, 1724u, 1680u, 1638u, 1598u, 1560u, 1524u, 1489u, 1456u, 1424u, 1394u, 1365u, 1337u, 1310u, 1285u, 1260u, 1236u, 1213u, 1191u, 1170u, 1149u, 1129u, 1110u, 1092u, 1074u, 1057u, 1040u, 1024u, 1008u, 992u, 978u, 963u, 949u, 936u, 923u, 910u, 897u, 885u, 873u, 862u, 851u, 840u, 829u, 819u, 809u, 799u, 789u, 780u, 771u, 762u, 753u, 744u, 736u, 728u, 720u, 712u, 704u, 697u, 689u, 682u, 675u, 668u, 661u, 655u, 648u, 642u, 636u, 630u, 624u, 618u, 612u, 606u, 601u, 595u, 590u, 585u, 579u, 574u, 569u, 564u, 560u, 555u, 550u, 546u, 541u, 537u, 532u, 528u, 524u, 520u, 516u, 512u, 508u, 504u, 500u, 496u, 492u, 489u, 485u, 481u, 478u, 474u, 471u, 468u, 464u, 461u, 458u, 455u, 451u, 448u, 445u, 442u, 439u, 436u, 434u, 431u, 428u, 425u, 422u, 420u, 417u, 414u, 412u, 409u, 407u, 404u, 402u, 399u, 397u, 394u, 392u, 390u, 387u, 385u, 383u, 381u, 378u, 376u, 374u, 372u, 370u, 368u, 366u, 364u, 362u, 360u, 358u, 356u, 354u, 352u, 350u, 348u, 346u, 344u, 343u, 341u, 339u, 337u, 336u, 334u, 332u, 330u, 329u, 327u, 326u, 324u, 322u, 321u, 319u, 318u, 316u, 315u, 313u, 312u, 310u, 309u, 307u, 306u, 304u, 303u, 302u, 300u, 299u, 297u, 296u, 295u, 293u, 292u, 291u, 289u, 288u, 287u, 286u, 284u, 283u, 282u, 281u, 280u, 278u, 277u, 276u, 275u, 274u, 273u
};

#else
static const sU16 loc_div_rows_tbl[1200+1] = {
   0u, 1u, 32768u, 21845u, 16384u, 13107u, 10922u, 9362u, 8192u, 7281u, 6553u, 5957u, 5461u, 5041u, 4681u, 4369u, 4096u, 3855u, 3640u, 3449u, 3276u, 3120u, 2978u, 2849u, 2730u, 2621u, 2520u, 2427u, 2340u, 2259u, 2184u, 2114u, 2048u, 1985u, 1927u, 1872u, 1820u, 1771u, 1724u, 1680u, 1638u, 1598u, 1560u, 1524u, 1489u, 1456u, 1424u, 1394u, 1365u, 1337u, 1310u, 1285u, 1260u, 1236u, 1213u, 1191u, 1170u, 1149u, 1129u, 1110u, 1092u, 1074u, 1057u, 1040u, 1024u, 1008u, 992u, 978u, 963u, 949u, 936u, 923u, 910u, 897u, 885u, 873u, 862u, 851u, 840u, 829u, 819u, 809u, 799u, 789u, 780u, 771u, 762u, 753u, 744u, 736u, 728u, 720u, 712u, 704u, 697u, 689u, 682u, 675u, 668u, 661u, 655u, 648u, 642u, 636u, 630u, 624u, 618u, 612u, 606u, 601u, 595u, 590u, 585u, 579u, 574u, 569u, 564u, 560u, 555u, 550u, 546u, 541u, 537u, 532u, 528u, 524u, 520u, 516u, 512u, 508u, 504u, 500u, 496u, 492u, 489u, 485u, 481u, 478u, 474u, 471u, 468u, 464u, 461u, 458u, 455u, 451u, 448u, 445u, 442u, 439u, 436u, 434u, 431u, 428u, 425u, 422u, 420u, 417u, 414u, 412u, 409u, 407u, 404u, 402u, 399u, 397u, 394u, 392u, 390u, 387u, 385u, 383u, 381u, 378u, 376u, 374u, 372u, 370u, 368u, 366u, 364u, 362u, 360u, 358u, 356u, 354u, 352u, 350u, 348u, 346u, 344u, 343u, 341u, 339u, 337u, 336u, 334u, 332u, 330u, 329u, 327u, 326u, 324u, 322u, 321u, 319u, 318u, 316u, 315u, 313u, 312u, 310u, 309u, 307u, 306u, 304u, 303u, 302u, 300u, 299u, 297u, 296u, 295u, 293u, 292u, 291u, 289u, 288u, 287u, 286u, 284u, 283u, 282u, 281u, 280u, 278u, 277u, 276u, 275u, 274u, 273u, 271u, 270u, 269u, 268u, 267u, 266u, 265u, 264u, 263u, 262u, 261u, 260u, 259u, 258u, 257u, 256u, 255u, 254u, 253u, 252u, 251u, 250u, 249u, 248u, 247u, 246u, 245u, 244u, 243u, 242u, 241u, 240u, 240u, 239u, 238u, 237u, 236u, 235u, 234u, 234u, 233u, 232u, 231u, 230u, 229u, 229u, 228u, 227u, 226u, 225u, 225u, 224u, 223u, 222u, 222u, 221u, 220u, 219u, 219u, 218u, 217u, 217u, 216u, 215u, 214u, 214u, 213u, 212u, 212u, 211u, 210u, 210u, 209u, 208u, 208u, 207u, 206u, 206u, 205u, 204u, 204u, 203u, 202u, 202u, 201u, 201u, 200u, 199u, 199u, 198u, 197u, 197u, 196u, 196u, 195u, 195u, 194u, 193u, 193u, 192u, 192u, 191u, 191u, 190u, 189u, 189u, 188u, 188u, 187u, 187u, 186u, 186u, 185u, 185u, 184u, 184u, 183u, 183u, 182u, 182u, 181u, 181u, 180u, 180u, 179u, 179u, 178u, 178u, 177u, 177u, 176u, 176u, 175u, 175u, 174u, 174u, 173u, 173u, 172u, 172u, 172u, 171u, 171u, 170u, 170u, 169u, 169u, 168u, 168u, 168u, 167u, 167u, 166u, 166u, 165u, 165u, 165u, 164u, 164u, 163u, 163u, 163u, 162u, 162u, 161u, 161u, 161u, 160u, 160u, 159u, 159u, 159u, 158u, 158u, 157u, 157u, 157u, 156u, 156u, 156u, 155u, 155u, 154u, 154u, 154u, 153u, 153u, 153u, 152u, 152u, 152u, 151u, 151u, 151u, 150u, 150u, 149u, 149u, 149u, 148u, 148u, 148u, 147u, 147u, 147u, 146u, 146u, 146u, 145u, 145u, 145u, 144u, 144u, 144u, 144u, 143u, 143u, 143u, 142u, 142u, 142u, 141u, 141u, 141u, 140u, 140u, 140u, 140u, 139u, 139u, 139u, 138u, 138u, 138u, 137u, 137u, 137u, 137u, 136u, 136u, 136u, 135u, 135u, 135u, 135u, 134u, 134u, 134u, 134u, 133u, 133u, 133u, 132u, 132u, 132u, 132u, 131u, 131u, 131u, 131u, 130u, 130u, 130u, 130u, 129u, 129u, 129u, 129u, 128u, 128u, 128u, 128u, 127u, 127u, 127u, 127u, 126u, 126u, 126u, 126u, 125u, 125u, 125u, 125u, 124u, 124u, 124u, 124u, 123u, 123u, 123u, 123u, 122u, 122u, 122u, 122u, 122u, 121u, 121u, 121u, 121u, 120u, 120u, 120u, 120u, 120u, 119u, 119u, 119u, 119u, 118u, 118u, 118u, 118u, 118u, 117u, 117u, 117u, 117u, 117u, 116u, 116u, 116u, 116u, 115u, 115u, 115u, 115u, 115u, 114u, 114u, 114u, 114u, 114u, 113u, 113u, 113u, 113u, 113u, 112u, 112u, 112u, 112u, 112u, 112u, 111u, 111u, 111u, 111u, 111u, 110u, 110u, 110u, 110u, 110u, 109u, 109u, 109u, 109u, 109u, 109u, 108u, 108u, 108u, 108u, 108u, 107u, 107u, 107u, 107u, 107u, 107u, 106u, 106u, 106u, 106u, 106u, 106u, 105u, 105u, 105u, 105u, 105u, 105u, 104u, 104u, 104u, 104u, 104u, 104u, 103u, 103u, 103u, 103u, 103u, 103u, 102u, 102u, 102u, 102u, 102u, 102u, 101u, 101u, 101u, 101u, 101u, 101u, 100u, 100u, 100u, 100u, 100u, 100u, 100u, 99u, 99u, 99u, 99u, 99u, 99u, 98u, 98u, 98u, 98u, 98u, 98u, 98u, 97u, 97u, 97u, 97u, 97u, 97u, 97u, 96u, 96u, 96u, 96u, 96u, 96u, 96u, 95u, 95u, 95u, 95u, 95u, 95u, 95u, 94u, 94u, 94u, 94u, 94u, 94u, 94u, 94u, 93u, 93u, 93u, 93u, 93u, 93u, 93u, 92u, 92u, 92u, 92u, 92u, 92u, 92u, 92u, 91u, 91u, 91u, 91u, 91u, 91u, 91u, 91u, 90u, 90u, 90u, 90u, 90u, 90u, 90u, 90u, 89u, 89u, 89u, 89u, 89u, 89u, 89u, 89u, 88u, 88u, 88u, 88u, 88u, 88u, 88u, 88u, 87u, 87u, 87u, 87u, 87u, 87u, 87u, 87u, 87u, 86u, 86u, 86u, 86u, 86u, 86u, 86u, 86u, 86u, 85u, 85u, 85u, 85u, 85u, 85u, 85u, 85u, 85u, 84u, 84u, 84u, 84u, 84u, 84u, 84u, 84u, 84u, 83u, 83u, 83u, 83u, 83u, 83u, 83u, 83u, 83u, 82u, 82u, 82u, 82u, 82u, 82u, 82u, 82u, 82u, 82u, 81u, 81u, 81u, 81u, 81u, 81u, 81u, 81u, 81u, 81u, 80u, 80u, 80u, 80u, 80u, 80u, 80u, 80u, 80u, 80u, 79u, 79u, 79u, 79u, 79u, 79u, 79u, 79u, 79u, 79u, 78u, 78u, 78u, 78u, 78u, 78u, 78u, 78u, 78u, 78u, 78u, 77u, 77u, 77u, 77u, 77u, 77u, 77u, 77u, 77u, 77u, 77u, 76u, 76u, 76u, 76u, 76u, 76u, 76u, 76u, 76u, 76u, 76u, 75u, 75u, 75u, 75u, 75u, 75u, 75u, 75u, 75u, 75u, 75u, 74u, 74u, 74u, 74u, 74u, 74u, 74u, 74u, 74u, 74u, 74u, 74u, 73u, 73u, 73u, 73u, 73u, 73u, 73u, 73u, 73u, 73u, 73u, 73u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 72u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 71u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 70u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 69u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 68u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 67u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 66u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 65u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 64u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 63u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 62u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 61u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 60u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 59u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 58u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 56u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 55u, 54u, 54u, 54u, 54u, 54u, 54u, 54u, 54u, 54u
};
#endif // AMIGA
#endif // HSPAN_DIV_TBL

static inline signed int div_rows(signed int _a, short _b) {
   // (note) 68020 divs.w : 56 cycles <https://www.nxp.com/docs/en/data-sheet/MC68020UM.pdf>
   // (note) 68020 divsl.l: 90 cycles <https://www.nxp.com/docs/en/data-sheet/MC68020UM.pdf>
   // (note) 68060 muls.l : 43 cycles <https://www.nxp.com/docs/en/data-sheet/MC68020UM.pdf>

   // (note) precision good enough for 640x240 res
   // (note) uses 16*16=32 int multiplication (mulu.w  on 68000)

   // (note) 'a' range: 0..(640*64)
   // (note) 'b' range: 0..240

   if(_b <= 0)
      return 0;
   if(1 == _b)
      return (_a << 10);

#ifdef HSPAN_DIV_TBL
   // (note) precision good enough for 640x240 res (?)
   // (note) uses 16*16=32 int multiplication (mulu.w  on 68000)
   typedef union u_us {
      sU32 u;
      sS32 s;
   } us_t;
   us_t r;
   /* printf("xxx a=%d b=%d\n", _a, _b); */
   //f.u = (sU32)(65536u / _b);
   const sU16 f = loc_div_rows_tbl[_b];
   if(_a < 0)
   {
      r.u = (mulu_w(((sU16)-_a)/*>>10*/, f) >> 6);
      return -r.s;
   }
   else
   {
      r.u = (mulu_w(((sU16)_a)/*>>10*/, f) >> 6);
      return r.s;
   }
   // (640*65536)/200 = 209715
   // rsh((640*65536) * round((65536*64)/200),14)
   // rsh(rsh((640*65536),7) * round((16384*1)/200),7) = 209920
   // rsh(rsh((640*65536),9) * round(65536/200),7) = 209920
   // rsh(rsh((640*65536),10) * round(65536/200),6) = 209920

   // rsh(rsh((640*65536),8) * round((32768*1)/200),7) = 209920
   //     rsh((640*65536),8) * round((32768*1)/2) = 2684354560 = #A000 0000  (best range for a={0..640*65536} b={2..240}
   //   ==> still precision issues with grey circle in 'cake' mesh

   // rsh(rsh((640*65536),11) * round(65536/200),5) = 209920

   // (640*65536)/5 = 8388608
   // rsh(rsh((640*65536),4) * round((65536*1)/5),9) = 8387584
   // rsh(rsh((640*65536),9) * round(65536/5),7) = 8388480

   // (640*65536)/2 = 20971520
   // rsh(rsh((640*65536),9) * round(65536/2),7) = 20971520

   // (640*65536)/3 = 13981013
   // rsh(rsh((640*65536),9) * round(65536/3),7) = 13980800
#elif 0
   sU16 f = (sS32)(65536.0f / _b);
   return ( (_a << 10) * f ) >> 16;
#elif 0
   /* _a = 320; */
   /* _b = 2; */
   sS32 idiv = ( (_a << 10) / _b);
   sS32 fdiv = (sS32)(1024.0f * (((sF32)(_a)) / (sS32)_b));
   sU32 f = (sU32)( ((sF32)(1 << 16)) / _b);
   sS32 r;
   if(_a < 0)
      r = -(( ((sU64)((-_a) << 10)) * f ) >> 16);
   else
      r = ( ((sU64)(_a << 10)) * f ) >> 16;
   /* if(fdiv != idiv) */
      printf("xxx idiv=%d fdiv=%d r=%d   f=%u\n", idiv, fdiv, r, f);
   return idiv;
#else
   return ( (_a << 10) / _b);
#endif // HSPAN_DIV_TBL
}

#if 0
static sS32 debug_div_rows(sS32 _a, sS32 _b) {
   if(0 == _b)
      return 0;
   else if(1 == _b)
      return (_a << 10);
   sS32 fdiv = (sS32)(1024.0f * (((sF32)(_a * 1.0f)) / _b));

   sS32 idiv = ( (_a << 10) / _b);
   /* sS32 fdiv = (sS32)(1024.0f * (((sF32)(_a)) / (sS32)_b)); */
   sU32 f = (sU32)( ((sF32)(1 << 20)) / _b);
   sS32 r;
   if(_a < 0)
      r = -(( ((sU64)((-_a) << 10)) * f ) >> 20);
   else
      r = ( ((sU64)(_a << 10)) * f ) >> 20;
   if(fdiv != idiv)
      printf("xxx a=%d(0x%08x b=%d(0x%08x) idiv=%d fdiv=%d r=%d   f=%u\n", _a, _a, _b, _b, idiv, fdiv, r, f);

   return fdiv;
}
#endif // 0

#ifdef HSPAN_COUNT_PX
sU32 hspan_count_px = 0u;
#endif // HSPAN_COUNT_PX

#ifndef AMIGA
#if HSPAN_SAFE
static int hspan_oob_count = 0;
#endif // HSPAN_SAFE
static void draw_hspan_simple(sU32 *_fb, const sU32 _fbW, const sU32 _fbH,
                              const sU32 x, const sU32 y, const sU32 w,
                              const sU32 _c32, const sU32 _c32Mask
                              ) {
   // debug
   Dprintf("    draw_hspan_simple: x=%u y=%u w=%u c32=#0x%08x mask=#0x%08x\n", x, y, w, _c32, _c32Mask);
#if 0
   if( (x + w) >= _fbW)
   {
      printf("xxx draw_hspan_simple: clip x=%u w=%u\n", x, w);
      return;
   }
   if( (y >= FB_H) )
   {
      printf("xxx draw_hspan_simple: clip y=%u\n", y);
      return;
   }
#endif
   sU32 *fb = _fb + y * _fbW + x;
#if HSPAN_SAFE
   if( ((fb - _fb) + w) > (_fbW * _fbH) )
   {
      if(0 == hspan_oob_count++) // avoid excessive log output
         printf("[---] draw_hspan_simple: out of bounds (x=%u y=%u w=%u) (fbSz=(%u,%u))\n", x, y, w, _fbW, _fbH);
      return;
   }
#endif // HSPAN_SAFE
   if(_c32Mask)
   {
      for(sU32 i = 0u; i < w; i++)
         if(fb[i] == _c32Mask)
            fb[i] = _c32;
   }
   else
   {
      for(sU32 i = 0u; i < w; i++)
            fb[i] = _c32;
   }
#ifdef HSPAN_COUNT_PX
   hspan_count_px += w;
#endif // HSPAN_COUNT_PX
}
#endif // AMIGA

#ifdef USE_BE2
static inline void draw_tri_hspan_be2(HSPAN_FBPTR_ARG, HSPAN_FBSZ_ARGS
                                      sU32 iy,
                                      sS32 topNumRows,
                                      sS32 bottomNumRows,
                                      sS32 edge12x,
                                      sS32 edge13x,
                                      sS32 edge23x,
                                      sS32 stepX12,
                                      sS32 stepX13,
                                      sS32 stepX23,
                                      const sU32 HSPAN_COLOR,
                                      const sU32 HSPAN_MASK
                                      ) {
   /* printf("xxx draw_tri_hspan_be2: iy=%u topNumRows=%u bottomNumRows=%u palIdx=%u\n", iy, topNumRows, bottomNumRows, _palIdx); */

   for(sSI rowIdx = 0; rowIdx < topNumRows; rowIdx++)
   {
      const sS32 xl = edge12x;
      const sS32 xr = edge13x;

      edge12x += stepX12;
      edge13x += stepX13;

      if(xr > xl)
      {
         sU32 xli = ((xl + (xl & 0x8000)) >> 16);
         sU32 xri = ((xr + (xr & 0x8000)) >> 16);
         const sU16 w = (sU16)(xri - xli);
         /* Dprintf("xxx y=%u xl=%f(0x%08x) xr=%f(0x%08x) e12x=0x%08x e13x=0x%08x  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, edge12x, edge13x, xli, xri, w); */
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xli, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      else
      {
         sU32 xri = ((xr + (xr & 0x8000)) >> 16);
         sU32 xli = ((xl + (xl & 0x8000)) >> 16);
         const sU16 w = (sU16)(xli - xri);
         /* Dprintf("xxx y=%u xl=%f(0x%08x) xr=%f(0x%08x) e12x=0x%08x e13x=0x%08x  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, edge12x, edge13x, xri, xli, w); */
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xri, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      iy++;
   }

   /* Dprintf("xxx ******\n"); */

   for(sSI rowIdx = 0; rowIdx < bottomNumRows; rowIdx++)
   {
      sS32 xl = edge23x;
      sS32 xr = edge13x;

      edge23x += stepX23;
      edge13x += stepX13;

      // top/left fill rule: draw first pixel only if leftX < 0.5, otherwise proceed to next pixel
      // top/left fill rule: draw last pixel only if rightX >= 0.5, otherwise proceed to next pixel

      if(xr > xl)
      {
         sU32 xli = ((xl & 0x8000) < 0x8000) ? (xl >> 16) : ((xl>>16)+1);
         sU32 xri = ((xr + (xr & 0x8000)) >> 16);
         const sU16 w = (sU16)(xri - xli);
         /* Dprintf("xxx y=%u  xl=%f(0x%08x)  xr=%f(0x%08x)  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, xli, xri, w); */
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xli, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      else
      {
         sU32 xri = ((xr & 0x8000) < 0x8000) ? (xr >> 16) : ((xr>>16)+1);
         sU32 xli = ((xl + (xl & 0x8000)) >> 16);
         const sU16 w = (sU16)(xli - xri);
         /* Dprintf("xxx y=%u  xl=%f(0x%08x)  xr=%f(0x%08x)  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, xri, xli, w); */
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xri, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      iy++;
   }
}
#endif // USE_BE2


Dstatic void draw_tri_hspan_int(HSPAN_FBPTR_ARG, HSPAN_FBSZ_ARGS
                                const sU32 *v1, const sU32 *v2, const sU32 *v3,
                                const sU32 HSPAN_COLOR,
                                const sU32 HSPAN_MASK
                                ) {
   // if(! ( (115u == (v1[1] >> 4)) || (115u == (v2[1] >> 4)) || (115u == (v3[1] >> 4)) ))
   //    return;
   // v1.y < v2.y < v3.y
   Dprintf("xxx -------- draw_tri_hspan_int: v1=(%f;%f) v2=(%f;%f) v3=(%f;%f)\n", v1[0]/FX_COORD_SCL_HSPAN, v1[1]/FX_COORD_SCL_HSPAN, v2[0]/FX_COORD_SCL_HSPAN, v2[1]/FX_COORD_SCL_HSPAN, v3[0]/FX_COORD_SCL_HSPAN, v3[1]/FX_COORD_SCL_HSPAN);

#define ybias 0  // 1
   const sS32 topRow    = (v1[1] + ybias) >> FR_BITS;
   const sS32 centerRow = (v2[1] + ybias) >> FR_BITS;
   const sS32 bottomRow = (v3[1] + ybias) >> FR_BITS;
#undef ybias

   Dprintf("xxx 1 topRow=%d centerRow=%d bottomRow=%d\n", topRow, centerRow, bottomRow);

   sS32 topNumRows    = centerRow - topRow;
   sS32 totalNumRows  = bottomRow - topRow;
   sS32 bottomNumRows = bottomRow - centerRow;

   const sS16 dx12 = ((sS16)v2[0] - (sS16)v1[0]);
   const sS16 dx13 = ((sS16)v3[0] - (sS16)v1[0]);
   const sS16 dx23 = ((sS16)v3[0] - (sS16)v2[0]);

#ifndef HSPAN_FLOAT_STEPX
   const sS32 stepX12 = div_rows(dx12 << (16-FR_BITS-10), (sS16)topNumRows);
   const sS32 stepX13 = div_rows(dx13 << (16-FR_BITS-10), (sS16)totalNumRows);
   const sS32 stepX23 = div_rows(dx23 << (16-FR_BITS-10), (sS16)bottomNumRows);
#elif 0
   // debug
   const sS32 stepX12 = debug_div_rows(dx12 << (16-FR_BITS-10), topNumRows);
   const sS32 stepX13 = debug_div_rows(dx13 << (16-FR_BITS-10), totalNumRows);
   const sS32 stepX23 = debug_div_rows(dx23 << (16-FR_BITS-10), bottomNumRows);
#else
   // debug
   const sS32 stepX12 = (sS32)(65536.0f * (((sF32)(dx12 / FX_COORD_SCL_HSPAN)) / topNumRows));
   const sS32 stepX13 = (sS32)(65536.0f * (((sF32)(dx13 / FX_COORD_SCL_HSPAN)) / totalNumRows));
   const sS32 stepX23 = (sS32)(65536.0f * (((sF32)(dx23 / FX_COORD_SCL_HSPAN)) / bottomNumRows));
#endif // HSPAN_FLOAT_STEPX

   Dprintf("xxx stepX12=%f(0x%08x)\n", (stepX12/65536.0f), stepX12);
   Dprintf("xxx stepX13=%f(0x%08x)\n", (stepX13/65536.0f), stepX13);
   Dprintf("xxx stepX23=%f(0x%08x)\n", (stepX23/65536.0f), stepX23);

   topNumRows++;
   totalNumRows++;

#if 1
   if(topRow == centerRow && topRow == bottomRow)
   {
#undef Dmin
#undef Dmax
#define Dmin(A,B) (((A)<(B))?(A):(B))
#define Dmax(A,B) (((A)>(B))?(A):(B))
      const sU16 v1x = Dmin(Dmin(v1[0], v2[0]),v3[0]);
      const sU16 v2x = Dmax(Dmax(v1[0], v2[0]),v3[0]);
#undef Dmin
#undef Dmax

      Dprintf("xxx top==center==bottom (%u)\n", topRow);

      if(v2x > v1x)
      {
         sU32 xli = ((v1x + X_BIAS_FR + (v1x & FR_HALF)) >> FR_BITS);
         sU32 xri = ((v2x + X_BIAS_FR + (v2x & FR_HALF)) >> FR_BITS);
         /* const sU16 w = (v2x + XR_BIAS_FR - v1x) >> 4; */
         const sU16 w = (sU16)(xri - xli);

         Dprintf("xxx same y=%u xl=%f(0x%08x) xr=%f(0x%08x) xli=%u xri=%u => w=%u\n", topRow, v1x/FX_COORD_SCL_HSPAN, v1x, v2x/FX_COORD_SCL_HSPAN, v2x, xli, xri, w);
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xli, topRow, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
         return;
      }
      else if(v1x > v2x)
      {
         /* const sU16 w = (v1x + XR_BIAS_FR - v2x) >> 4; */
         sU32 xri = ((v2x + X_BIAS_FR + (v2x & FR_HALF)) >> FR_BITS);
         sU32 xli = ((v1x + X_BIAS_FR + (v1x & FR_HALF)) >> FR_BITS);
         /* const sU16 w = (v2x + XR_BIAS_FR - v1x) >> 4; */
         const sU16 w = (sU16)(xli - xri);
         Dprintf("xxx same y=%u xl=%f(0x%08x) xr=%f(0x%08x) xli=%u xri=%u => w=%u\n", topRow, v1[0]/FX_COORD_SCL_HSPAN, v1[0], v2[0]/FX_COORD_SCL_HSPAN, v2[0], xli, xri, w);
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xri, topRow, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
         return;
      }
   }
#endif // 1

   sUI iy = topRow;

   sS32 edge12x = (v1[0] + X_BIAS_FR) << (16-FR_BITS);
   sS32 edge13x = edge12x;

   if(0)
   {
      sS16 dyStart = (v1[1]-(iy << FR_BITS));
      sS32 advStartY = (stepX13 * dyStart) >> FR_BITS;
      printf("xxx  edge13x dyStart=%d(%f) advStart=%d(%f)\n", dyStart, dyStart/FX_COORD_SCL_HSPAN, advStartY, advStartY/65536.0f);
      edge13x += advStartY;
   }

   if(topRow == centerRow)
   {
      // Corner case: 1 pixel top row (e.g. axis aligned rectangles)
      edge12x = (v2[0] + X_BIAS_FR) << (16-FR_BITS);

#if HSPAN_TOPLEFT_FILLRULE
      // top/left fill rule: draw top row only when frac(v2.y) < 0.5
      Dprintf("xxx topRow == centerRow : frac(v2.y)>=0.5:%d  v2.y=0x%04x\n", (0u != (v2[1] & FR_HALF)), v2[1]);
      if(0u != (v2[1] & FR_HALF))
      {
         iy++;
         edge12x += stepX12;
         edge13x += stepX13;
         topNumRows--;
      }
#endif // HSPAN_TOPLEFT_FILLRULE

   }

#if HSPAN_TOPLEFT_FILLRULE
   if(centerRow == bottomRow)
   {
      // top/left fill rule: draw bottom row only when frac(v3.y) >= 0.5
      //  (note) bottomNumRows is 0
      if(0u == (v3[1] & FR_HALF))
         topNumRows--;
   }
#endif // HSPAN_TOPLEFT_FILLRULE

   Dprintf("xxx 2 topRow=%d centerRow=%d bottomRow=%d   numTopRows=%d numBottomRows=%d\n", topRow, centerRow, bottomRow, topNumRows, bottomNumRows);

   /* // while(iy < centerRow) */
   for(sSI rowIdx = 0; rowIdx < topNumRows; rowIdx++)
   {
      const sS32 xl = edge12x;
      const sS32 xr = edge13x;

      edge12x += stepX12;
      edge13x += stepX13;

      if(xr > xl)
      {
         sU32 xli = ((xl + (xl & 0x8000)) >> 16);
         sU32 xri = ((xr + (xr & 0x8000)) >> 16);
         /* const sU16 w = (xr + XR_BIAS16 - xl) >> 16; */
         const sU16 w = (sU16)(xri - xli);
         Dprintf("xxx y=%u xl=%f(0x%08x) xr=%f(0x%08x) e12x=0x%08x e13x=0x%08x  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, edge12x, edge13x, xli, xri, w);
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xli, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      else
      {
         sU32 xri = ((xr + (xr & 0x8000)) >> 16);
         sU32 xli = ((xl + (xl & 0x8000)) >> 16);
         /* sU32 xri = (xr >> 16); */
         /* /\* sU32 xli = (xl >> 16); *\/ */
         /* const sU16 w = (xl + XR_BIAS16 - xr) >> 16; */
         const sU16 w = (sU16)(xli - xri);
         Dprintf("xxx y=%u xl=%f(0x%08x) xr=%f(0x%08x) e12x=0x%08x e13x=0x%08x  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, edge12x, edge13x, xri, xli, w);
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xri, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      iy++;
   }

#if HSPAN_TOPLEFT_FILLRULE
   if(centerRow == bottomRow)
      return;

   // top/left fill rule: draw bottom row only when frac(v3.y) >= 0.5
   if(0u == (v3[1] & FR_HALF))
   /* /\* if( (v3[1] & FR_HALF) <= FR_HALF ) *\/ */
      bottomNumRows--;
#endif // HSPAN_TOPLEFT_FILLRULE

   Dprintf("xxx ******\n");
   sS32 edge23x = (v2[0] + X_BIAS_FR) << (16-FR_BITS);
   edge23x += stepX23;

   // while(iy < bottomRow)
   for(sSI rowIdx = 0; rowIdx < bottomNumRows; rowIdx++)
   {
      sS32 xl = edge23x;
      sS32 xr = edge13x;

      edge23x += stepX23;
      edge13x += stepX13;

      // top/left fill rule: draw first pixel only if leftX < 0.5, otherwise proceed to next pixel
      // top/left fill rule: draw last pixel only if rightX >= 0.5, otherwise proceed to next pixel

      if(xr > xl)
      {
         sU32 xli = ((xl & 0x8000) < 0x8000) ? (xl >> 16) : ((xl>>16)+1);
         sU32 xri = ((xr + (xr & 0x8000)) >> 16);
         const sU16 w = (sU16)(xri - xli);
         Dprintf("xxx y=%u  xl=%f(0x%08x)  xr=%f(0x%08x)  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, xli, xri, w);
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xli, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      else
      {
         sU32 xri = ((xr & 0x8000) < 0x8000) ? (xr >> 16) : ((xr>>16)+1);
         sU32 xli = ((xl + (xl & 0x8000)) >> 16);
         const sU16 w = (sU16)(xli - xri);
         Dprintf("xxx y=%u  xl=%f(0x%08x)  xr=%f(0x%08x)  xli=%u xri=%u => w=%u\n", iy, xl/65536.0f, xl, xr/65536.0f, xr, xri, xli, w);
         if(w > 0u)
         {
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        xri, iy, w/*w*/, HSPAN_COLOR, HSPAN_MASK
                        );
         }
      }
      iy++;
   }
}

void draw_tri_hspan(HSPAN_FBPTR_ARG, HSPAN_FBSZ_ARGS
                    const sU32 *va,
                    const sU32 HSPAN_COLOR,
                    const sU32 HSPAN_MASK
                    ) {
#if HSPAN_SAFE
   hspan_oob_count = 0;
#endif // HSPAN_SAFE

   // (note) only the code paths actually used in the test objects are implemented
   if(va[3] == va[5])
   {
      // v2.y == v3.y
      if(va[1] < va[3])
      {
         // => order: v1,v2==v3
         Dprintfimpl("xxx order: v1,v2==v3\n");
         draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                            va+0, va+2, va+4,
                            HSPAN_COLOR, HSPAN_MASK
                            );
      }
      else
      {
         // => order: v2==v3,v1
         Dprintfimpl("xxx order: v2==v3,v1\n");
         draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                            va+2, va+4, va+0,
                            HSPAN_COLOR, HSPAN_MASK
                            );
      }
   }
   else if(va[3] < va[5])
   {
      // v2.y < v3.y
      if(va[1] < va[3])
      {
         // v1.y < v2.y
         // => order: v1,v2,v3
         Dprintfimpl("xxx order: v1,v2,v3\n");
         draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                            va+0, va+2, va+4,
                            HSPAN_COLOR, HSPAN_MASK
                            );
      }
      else if(va[1] == va[3])
      {
         // v1.y == v2.y
         if(va[1] < va[5])
         {
            // => order: v1==v2,v3
            Dprintfimpl("xxx order: v1==v2,v3\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+0, va+2, va+4,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
         else
         {
            // v2.y < v1.y
            // => order: v3,v1==v2
            Dprintfmissing("xxx order: v3,v1==v2\n");
         }
      }
      else
      {
         // v2.y < v1.y
         if(va[1] < va[5])
         {
            // => order: v2,v1,v3
            Dprintfimpl("xxx order: v2,v1,v3\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+2, va+0, va+4,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
         else if(va[1] == va[5])
         {
            // => order: v2,v1==v3
            Dprintfimpl("xxx order: v2,v1==v3\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+2, va+0, va+4,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
         else
         {
            // => order: v2,v3,v1
            Dprintfimpl("xxx order: v2,v3,v1\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+2, va+4, va+0,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
      }
   }
   else
   {
      // v3.y < v2.y
      Dprintf("xxx va[1]=%f va[5]=%f\n", va[1]/FX_COORD_SCL_HSPAN, va[5]/FX_COORD_SCL_HSPAN);
      if(va[1] < va[5])
      {
         // v1.y < v3.y
         // => order: v1,v3,v2
         Dprintfimpl("xxx order: v1,v3,v2\n");
         draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                            va+0, va+4, va+2,
                            HSPAN_COLOR, HSPAN_MASK
                            );
      }
      else if(va[1] == va[5])
      {
         // v1.y == v3.y
         // => order: v1==v3,v2
         Dprintfimpl("xxx order: v1==v3,v2\n");
         draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                            va+0, va+4, va+2,
                            HSPAN_COLOR, HSPAN_MASK
                            );
      }
      else
      {
         // v1.y > v3.y
         if(va[1] == va[3])
         {
            // v1.y == v2.y
            // => order:  v3,v1==v2
            Dprintfimpl("xxx order: v3,v1==v2\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+4, va+0, va+2,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
         else if(va[1] < va[3])
         {
            // v1.y < v2.y
            Dprintfimpl("xxx order: v3,v1,v2\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+4, va+0, va+2,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
         else
         {
            // v1.y > v2.y
            Dprintfimpl("xxx order: v3,v2,v1\n");
            draw_tri_hspan_int(HSPAN_FBPTR, HSPAN_FBSZ
                               va+4, va+2, va+0,
                               HSPAN_COLOR, HSPAN_MASK
                               );
         }
      }
   }
}

#ifdef USE_BE2
void draw_mesh_be2(HSPAN_FBPTR_ARG, HSPAN_FBSZ_ARGS
                   const void *_dat
#ifndef AMIGA
                   , const sU32 *_pal32
#endif // AMIGA
                   ) {
   union {
      const void *any;
      const sU8  *u8;
      const sU16 *u16;
      const sU32 *u32;
      const sS32 *s32;
   } b;
   b.any = _dat;
   const sU8 *datStart = b.u8;

   sU32 totalNumSetups = 0u;
   sU32 pathIdx = 0u;

   for(;;)
   {
      const sU32 palIdx = Dbe2_swap16(*b.u16++);
      /* printf("xxx BE2 palIdx=%u\n", pathIdx); */
      if(0xFFFFu == palIdx)
         break;

#ifndef AMIGA
      const sU32 c32 = _pal32[palIdx & 7u];
#endif // AMIGA

      const sU16 numSetups = Dbe2_swap16(*b.u16++);
      totalNumSetups += numSetups;
     
      /* printf("xxx BE2 draw pathIdx=%u palIdx=%u numSetups=%u\n", pathIdx, palIdx, numSetups); */

      for(sU32 setupIdx = 0u; setupIdx < numSetups; setupIdx++)
      {
         if(*b.u8++)  // mode
         {
            // 1: TRI
            const sU32 iy            = *b.u8++;
            const sS32 topNumRows    = (sS32)*b.u8++;
            const sS32 bottomNumRows = (sS32)*b.u8++;
            const sS32 edge12x       = Dbe2_swap32(*b.s32++);
            const sS32 edge13x       = Dbe2_swap32(*b.s32++);
            const sS32 edge23x       = Dbe2_swap32(*b.s32++);
            const sS32 stepX12       = Dbe2_swap32(*b.s32++);
            const sS32 stepX13       = Dbe2_swap32(*b.s32++);
            const sS32 stepX23       = Dbe2_swap32(*b.s32++);

            Dprintf("xxx BE2   TRI   datOffset=%u pathIdx=%u palIdx=%u setupIdx=%u/%u\n", (sU32)(b.u8 - datStart - 28), pathIdx, palIdx, setupIdx, numSetups);
            Dprintf("                  iy=%u\n", iy);
            Dprintf("                  topNumRows=%u\n", topNumRows);
            Dprintf("                  bottomNumRows=%u\n", bottomNumRows);
            Dprintf("                  edge12x=%f(0x%08x)\n", (edge12x/65536.0f), edge12x);
            Dprintf("                  edge13x=%f(0x%08x)\n", (edge13x/65536.0f), edge12x);
            Dprintf("                  edge23x=%f(0x%08x)\n", (edge23x/65536.0f), edge12x);
            Dprintf("                  stepX12=%f(0x%08x)\n", (stepX12/65536.0f), stepX12);
            Dprintf("                  stepX13=%f(0x%08x)\n", (stepX13/65536.0f), stepX12);
            Dprintf("                  stepX23=%f(0x%08x)\n", (stepX23/65536.0f), stepX12);

#if 1 && !defined(BE2_SKIP_DRAW)
            draw_tri_hspan_be2(HSPAN_FBPTR, HSPAN_FBSZ
                               iy,
                               topNumRows,
                               bottomNumRows,
                               edge12x,
                               edge13x,
                               edge23x,
                               stepX12,
                               stepX13,
                               stepX23,
#ifdef AMIGA
                               palIdx
#else
                               c32
#endif // AMIGA
                               );
#else
            Dprintf("xxx BE2   TRI   datOffset=%u pathIdx=%u palIdx=%u setupIdx=%u/%u\n", (sU32)(b.u8 - datStart - 28), pathIdx, palIdx, setupIdx, numSetups);
#endif // BE2_SKIP_DRAW
         }
         else
         {
            // 0: HSPAN
            const sU32 y = *b.u8++;
            const sU32 x = Dbe2_swap16(*b.u16++);
            const sU32 w = Dbe2_swap16(*b.u16++);

            Dprintf("xxx BE2   HSPAN datOffset=%u pathIdx=%u palIdx=%u setupIdx=%u/%u x=%u y=%u w=%u\n", (sU32)(b.u8 - datStart -6), pathIdx, palIdx, setupIdx, numSetups, x, y, w);
            
#if 1 && !defined(BE2_SKIP_DRAW)
            Ddraw_hspan(HSPAN_FBPTR, HSPAN_FBSZ
                        x, y,
#ifdef AMIGA
                        palIdx,
#else
                        c32,
#endif // AMIGA
                        w/*w*/
                        );
#else
            Dprintf("xxx BE2   HSPAN datOffset=%u pathIdx=%u palIdx=%u setupIdx=%u/%u x=%u y=%u w=%u\n", (sU32)(b.u8 - datStart -6), pathIdx, palIdx, setupIdx, numSetups, x, y, w);
#endif // BE2_SKIP_DRAW
         }

         /* if(0) */
         /* { */
         /*    Dprintf("xxx BE2 pathIdx=%u triIdx=%u v1=(%u; %u) v2=(%u; %u) v3=(%u; %u)\n", pathIdx, triIdx, va[0]>>4, va[1]>>4, va[2]>>4, va[3]>>4, va[4]>>4, va[5]>>4); */
         /* } */
      } // loop numSetups

      pathIdx++;
      // if(pathIdx > 0) // xxxxxxxxxx
      //    break;
   } // for;;

   (void)totalNumSetups;
   (void)pathIdx;

}
#endif // USE_BE2

#undef FR_BITS
#undef FR_HALF
#undef FR_ONE

#endif // TKS_TRI_HSPAN_H__
