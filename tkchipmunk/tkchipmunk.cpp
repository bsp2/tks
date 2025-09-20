/// tkchipmunk.cpp
///
/// (c) 2007 Scott Lembcke <lemb0029(at)morris(dot)umn(dot)edu or slembcke(at)gmail(dot)com>
/// (c) 2008-2024 by Bastian Spiegel <bs@tkscript.de>
/// 
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///  
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///  
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///  SOFTWARE.
///   
///  "tkchipmunk" is a TKScript/YAC binding for Scott Lembcke's "chipmunk" 2d game physics library.
///
///  Please see <http://files.slembcke.net/chipmunk/chipmunk-docs.html>
///   
/// created: 06May2008
/// changed: 07May2008, 08May2008, 12Jun2018, 13Jun2018, 14Jun2018, 18Jun2018, 19Jul2018
///          19Sep2021, 22Sep2021, 23Sep2021, 24Sep2021, 21Dec2024
///
///
///

#include <stdio.h>
#include <stdarg.h>

#include <yac.h>

YAC_Host *yac_host;

// Implement "standard" exid_* variables (e.g. exid_InvalidPointer)
Dyac_std_exid_impl;

#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_structs.h>

#include "tkchipmunk.h"

#include "ying_cp.h"
#include "ying_cp.cpp"
#include "ying_cp_CpVect.cpp"
#include "ying_cp_CpBB.cpp"
#include "ying_cp_CpTransform.cpp"
#include "ying_cp_CpBody.cpp"
#include "ying_cp_CpShape.cpp"
#include "ying_cp_CpCircleShape.cpp"
#include "ying_cp_CpSegmentShape.cpp"
#include "ying_cp_CpPolyShape.cpp"
#include "ying_cp_CpConstraint.cpp"
#include "ying_cp_CpJoint.cpp"
#include "ying_cp_CpPinJoint.cpp"
#include "ying_cp_CpSlideJoint.cpp"
#include "ying_cp_CpPivotJoint.cpp"
#include "ying_cp_CpGrooveJoint.cpp"
#include "ying_cp_CpDampedSpring.cpp"
#include "ying_cp_CpGearJoint.cpp"
#include "ying_cp_CpRotaryLimitJoint.cpp"
#include "ying_cp_CpRatchetJoint.cpp"
#include "ying_cp_CpArbiter.cpp"
#include "ying_cp_CpSpace.cpp"

#include "yac_host.cpp"


#define Dsafedelete(a) if(NULL != a) \
                       { \
                           YAC_DELETE(a); \
                           a = NULL; \
                       }


// ---------------------------------------------------------------------------- CpVect
static int oc_cpvect = 0;
CpVect::CpVect(void) {
   oc_cpvect++;
}

CpVect::~CpVect() {
   oc_cpvect--;
}

void YAC_VCALL CpVect::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_VALID(_o))
   {
      if(YAC_BCHK(_o, clid_CpVect))
      {
         CpVect *o = (CpVect*) _o;
         cp_vect.x = o->cp_vect.x;
         cp_vect.y = o->cp_vect.y;
      }
      else
      {
         // Assign generic
         sUI ne = _o->yacArrayGetNumElements();
         if(ne >= 2)
         {
            YAC_Value v;
            // Read 'x'
            _o->yacArrayGet(NULL, 0, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_vect.x = v.value.float_val;
            // Read 'y'
            _o->yacArrayGet(NULL, 0, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_vect.y = v.value.float_val;
         }
      }
   }
}

sUI CpVect::yacArrayGetNumElements(void) {
   return 2u;
}

sUI CpVect::yacArrayGetMaxElements(void) {
   return 2u;
}

void YAC_VCALL CpVect::yacArrayGet(void *_ctx, sUI _index, YAC_Value *_value) {
   if(_index < 2)
   {
      if(0 == _index)
      {
         _value->initFloat(cp_vect.x);
      }
      else
      {
         _value->initFloat(cp_vect.y);
      }
   }
   else
   {
      Dyac_throw(ReadArrayOutOfBounds, "");
   }
}

void CpVect::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 2) 
   {
      sF32 v;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            v = 0.0f;
            break;
         case YAC_TYPE_INT:
            v = (sF32)_val->value.int_val;
            break;
         case YAC_TYPE_FLOAT:
            v = _val->value.float_val;
            break;
         case YAC_TYPE_OBJECT:
            if(YAC_VALID(_val->value.object_val)) 
            {
               if (!_val->value.object_val->yacScanF32(&v))
               {
                  // Scan failed, silently assign 0.0f
                  v = 0.0f;
               }
            } 
            else 
            {
               // Invalid or <null> object, silently assign 0.0f
               v = 0.0f;
            }
            break;
         default:
            // Should never ever happen!
            Dyac_throw(TypeMismatch, "CpVect::yacArraySet: unknown type");
            v = 0.0f;
            break;
      }

      if(0 == _idx)
         cp_vect.x = v;
      else
         cp_vect.y = v;
   }
   else 
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "CpVect::yacArraySet: index out of bounds (%u>1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

sBool YAC_VCALL CpVect::yacToString(YAC_String *_r) const {
   _r->alloc(32);
   _r->printf("(%f; %f)", cp_vect.x, cp_vect.y);
   return 1;
}

sF32 CpVect::_getX(void) {
   return cp_vect.x;
}

void CpVect::_setX(sF32 _x) {
   cp_vect.x = _x;
}

sF32 CpVect::_getY(void) {
   return cp_vect.y;
}

void CpVect::_setY(sF32 _y) {
   cp_vect.y = _y;
}

void CpVect::_init(sF32 _x, sF32 _y) {
   cp_vect.x = _x;
   cp_vect.y = _y;
}

void CpVect::_getString(YAC_Value *_r) {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void CpVect::_add_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpvadd(cp_vect, _v->cp_vect);
}

void CpVect::_add_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvadd(cp_vect, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_add_YAC_RSELF(CpVect *_v) {
   cp_vect = cpvadd(cp_vect, _v->cp_vect);
}

void CpVect::_sub_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpvsub(cp_vect, _v->cp_vect);
}

void CpVect::_sub_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvsub(cp_vect, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_sub_YAC_RSELF(CpVect *_v) {
   cp_vect = cpvsub(cp_vect, _v->cp_vect);
}

void CpVect::_neg_YAC_RARG(CpVect *_r) {
   _r->cp_vect = cpvneg(cp_vect);
}

void CpVect::_neg_YAC_RVAL(YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvneg(cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_neg_YAC_RSELF(void) {
   cp_vect = cpvneg(cp_vect);
}

void CpVect::_mult_YAC_RARG(sF32 _s, CpVect *_r) {
   _r->cp_vect = cpvmult(cp_vect, _s);
}

void CpVect::_mult_YAC_RVAL(sF32 _s, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvmult(cp_vect, _s);
   YAC_RETO(n, 1);
}

void CpVect::_mult_YAC_RSELF(sF32 _s) {
   cp_vect = cpvmult(cp_vect, _s);
}

sF32 CpVect::_dot(CpVect *_v) {
   return cpvdot(cp_vect, _v->cp_vect);
}

sF32 CpVect::_cross(CpVect *_v) {
   return cpvcross(cp_vect, _v->cp_vect);
}

void CpVect::_perp_YAC_RARG(CpVect *_r) {
   _r->cp_vect = cpvperp(cp_vect);
}

void CpVect::_perp_YAC_RVAL(YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvperp(cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_perp_YAC_RSELF(void) {
   cp_vect = cpvperp(cp_vect);
}

void CpVect::_rperp_YAC_RARG(CpVect *_r) {
   _r->cp_vect = cpvrperp(cp_vect);
}

void CpVect::_rperp_YAC_RVAL(YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvrperp(cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_rperp_YAC_RSELF(void) {
   cp_vect = cpvrperp(cp_vect);
}

void CpVect::_project_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpvproject(cp_vect, _v->cp_vect);
}

void CpVect::_project_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvproject(cp_vect, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_project_YAC_RSELF(CpVect *_v) {
   cp_vect = cpvproject(cp_vect, _v->cp_vect);
}

void CpVect::_forAngle_YAC_RARG(sF32 _a, CpVect *_r) {
   _r->cp_vect = cpvforangle(_a);
}

void CpVect::_forAngle_YAC_RVAL(sF32 _a, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvforangle(_a);
   YAC_RETO(n, 1);
}

void CpVect::_forAngle_YAC_RSELF(sF32 _a) {
   cp_vect = cpvforangle(_a);
}

sF32 CpVect::_toAngle (void) {
   return cpvtoangle(cp_vect);
}

sF32 CpVect::_getAngle (void) {
   return cpvtoangle(cp_vect);
}

void CpVect::_rotate_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpvrotate(cp_vect, _v->cp_vect);
}

void CpVect::_rotate_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvrotate(cp_vect, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_rotate_YAC_RSELF(CpVect *_v) {
   cp_vect = cpvrotate(cp_vect, _v->cp_vect);
}

void CpVect::_unrotate_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpvunrotate(cp_vect, _v->cp_vect);
}

void CpVect::_unrotate_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvunrotate(cp_vect, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_unrotate_YAC_RSELF(CpVect *_v) {
   cp_vect = cpvunrotate(cp_vect, _v->cp_vect);
}

sF32 CpVect::_getLength(void) {
   return cpvlength(cp_vect);
}

sF32 CpVect::_getLengthSq(void) {
   return cpvlengthsq(cp_vect);
}

void CpVect::_lerp_YAC_RARG(CpVect *_o, sF32 _t, CpVect *_r) {
   _r->cp_vect = cpvlerp(cp_vect, _o->cp_vect, _t);
}

void CpVect::_lerp_YAC_RVAL(CpVect *_o, sF32 _t, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvlerp(cp_vect, _o->cp_vect, _t);
   YAC_RETO(n, 1);
}

void CpVect::_lerp_YAC_RSELF(CpVect *_o, sF32 _t) {
   cp_vect = cpvlerp(cp_vect, _o->cp_vect, _t);
}

void CpVect::_normalize_YAC_RARG(CpVect *_r) {
   _r->cp_vect = cpvnormalize(cp_vect);
}

void CpVect::_normalize_YAC_RVAL(YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvnormalize(cp_vect);
   YAC_RETO(n, 1);
}

void CpVect::_normalize_YAC_RSELF(void) {
   cp_vect = cpvnormalize(cp_vect);
}

void CpVect::_slerp_YAC_RARG(CpVect *_o, sF32 _t, CpVect *_r) {
   _r->cp_vect = cpvslerp(cp_vect, _o->cp_vect, _t);
}

void CpVect::_slerp_YAC_RVAL(CpVect *_o, sF32 _t, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvslerp(cp_vect, _o->cp_vect, _t);
   YAC_RETO(n, 1);
}

void CpVect::_slerp_YAC_RSELF(CpVect *_o, sF32 _t) {
   cp_vect = cpvslerp(cp_vect, _o->cp_vect, _t);
}

void CpVect::_slerpconst_YAC_RARG(CpVect *_o, sF32 _a, CpVect *_r) {
   _r->cp_vect = cpvslerpconst(cp_vect, _o->cp_vect, _a);
}

void CpVect::_slerpconst_YAC_RVAL(CpVect *_o, sF32 _a, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvslerpconst(cp_vect, _o->cp_vect, _a);
   YAC_RETO(n, 1);
}

void CpVect::_slerpconst_YAC_RSELF(CpVect *_o, sF32 _a) {
   cp_vect = cpvslerpconst(cp_vect, _o->cp_vect, _a);
}

void CpVect::_clamp_YAC_RARG(sF32 _l, CpVect *_r) {
   _r->cp_vect = cpvclamp(cp_vect, _l);
}

void CpVect::_clamp_YAC_RVAL(sF32 _l, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvclamp(cp_vect, _l);
   YAC_RETO(n, 1);
}

void CpVect::_clamp_YAC_RSELF(sF32 _l) {
   cp_vect = cpvclamp(cp_vect, _l);
}

void CpVect::_lerpconst_YAC_RARG(CpVect *_o, sF32 _d, CpVect *_r) {
   _r->cp_vect = cpvlerpconst(cp_vect, _o->cp_vect, _d);
}

void CpVect::_lerpconst_YAC_RVAL(CpVect *_o, sF32 _d, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvlerpconst(cp_vect, _o->cp_vect, _d);
   YAC_RETO(n, 1);
}

void CpVect::_lerpconst_YAC_RSELF(CpVect *_o, sF32 _d) {
   cp_vect = cpvlerpconst(cp_vect, _o->cp_vect, _d);
}

sF32 CpVect::_dist(CpVect *_o) {
   return cpvdist(cp_vect, _o->cp_vect);
}

sF32 CpVect::_distSq(CpVect *_o) {
   return cpvdistsq(cp_vect, _o->cp_vect);
}

sBool CpVect::_near(CpVect *_o, sF32 _dist) {
   return cpvnear(cp_vect, _o->cp_vect, _dist);
}

static sF32 loc_randf(sF32 _max) {
   return (sF32) ((rand()*(0.999999999999f/((sF32)RAND_MAX)))*_max);
}

void CpVect::_rand_YAC_RARG(sF32 _amt, CpVect *_r) {
   _r->cp_vect.x = cp_vect.x + loc_randf(2.0f * _amt) - _amt;
   _r->cp_vect.y = cp_vect.y + loc_randf(2.0f * _amt) - _amt;
}

void CpVect::_rand_YAC_RVAL(sF32 _amt, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect.x = cp_vect.x + loc_randf(2.0f * _amt) - _amt;
   n->cp_vect.y = cp_vect.y + loc_randf(2.0f * _amt) - _amt;
   YAC_RETO(n, 1);
}

void CpVect::_rand_YAC_RSELF(sF32 _amt) {
   cp_vect.x += loc_randf(2.0f * _amt) - _amt;
   cp_vect.y += loc_randf(2.0f * _amt) - _amt;
}


void CpVect::_randNormalize_YAC_RARG(sF32 _amt, CpVect *_r) {
   _r->cp_vect = cpvnormalize(cpv(cp_vect.x + loc_randf(2.0f * _amt) - _amt,
                                  cp_vect.y + loc_randf(2.0f * _amt) - _amt
                                  )
                              );
}

void CpVect::_randNormalize_YAC_RVAL(sF32 _amt, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpvnormalize(cpv(cp_vect.x + loc_randf(2.0f * _amt) - _amt,
                                 cp_vect.y + loc_randf(2.0f * _amt) - _amt
                                 )
                             );
   YAC_RETO(n, 1);
}

void CpVect::_randNormalize_YAC_RSELF(sF32 _amt) {
   cp_vect = cpvnormalize(cpv(cp_vect.x + loc_randf(2.0f * _amt) - _amt,
                              cp_vect.y + loc_randf(2.0f * _amt) - _amt
                              )
                          );
}


// ---------------------------------------------------------------------------- CpBB
static int oc_cpbb = 0;
CpBB::CpBB(void) {
   oc_cpbb++;
   cp_bb.l = cp_bb.b = cp_bb.r = cp_bb.t = 0.0f;
}

CpBB::~CpBB() {
   oc_cpbb--;
}

void YAC_VCALL CpBB::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_VALID(_o))
   {
      if(YAC_BCHK(_o, clid_CpBB))
      {
         CpBB *o = (CpBB*) _o;
         cp_bb.l = o->cp_bb.l;
         cp_bb.b = o->cp_bb.b;
         cp_bb.r = o->cp_bb.r;
         cp_bb.t = o->cp_bb.t;
      }
   }
}

sBool YAC_VCALL CpBB::yacToString(YAC_String *_r) const {
   _r->alloc(64);
   _r->printf("(l=%f; b=%f; r=%f; t=%f)", cp_bb.l, cp_bb.b, cp_bb.r, cp_bb.t);
   return 1;
}

void CpBB::_getString(YAC_Value *_r) {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void CpBB::_New(sF32 _left, sF32 _bottom, sF32 _right, sF32 _top, YAC_Value *_r) {
   CpBB *b = YAC_NEW_POOLED(CpBB);
   b->_init(_left, _bottom, _right, _top);
   YAC_RETO(b, 1);
}

void CpBB::_NewForExtents(CpVect *_v, sF32 _hw, sF32 _hh, YAC_Value *_r) {
   CpBB *b = YAC_NEW_POOLED(CpBB);
   b->cp_bb = cpBBNewForExtents(_v->cp_vect, _hw, _hh);
   YAC_RETO(b, 1);
}

void CpBB::_NewForCircle(CpVect *_v, sF32 _radius, YAC_Value *_r) {
   CpBB *b = YAC_NEW_POOLED(CpBB);
   b->cp_bb = cpBBNewForCircle(_v->cp_vect, _radius);
   YAC_RETO(b, 1);
}

sF32 CpBB::_getL(void) {
   return cp_bb.l;
}

void CpBB::_setL(sF32 _l) {
   cp_bb.l = _l;
}

sF32 CpBB::_getB(void) {
   return cp_bb.b;
}

void CpBB::_setB(sF32 _b) {
   cp_bb.b = _b;
}

sF32 CpBB::_getR(void) {
   return cp_bb.r;
}

void CpBB::_setR(sF32 _r) {
   cp_bb.r = _r;
}

sF32 CpBB::_getT(void) {
   return cp_bb.t;
}

void CpBB::_setT(sF32 _t) {
   cp_bb.t = _t;
}

void CpBB::_init(sF32 _l, sF32 _b, sF32 _r, sF32 _t) {
   cp_bb.l = _l;
   cp_bb.b = _b;
   cp_bb.r = _r;
   cp_bb.t = _t;
}

sBool CpBB::_intersects(CpBB *_b) {
   return cpBBIntersects(cp_bb, _b->cp_bb);
}

sBool CpBB::_contains(CpBB *_b) {
   return cpBBContainsBB(cp_bb, _b->cp_bb);
}

sBool CpBB::_containsVect(CpVect *_v) {
   return cpBBContainsVect(cp_bb, _v->cp_vect);
}

void CpBB::_merge(CpBB *_b, YAC_Value *_r) {
   CpBB *n = YAC_NEW_POOLED(CpBB);
   n->cp_bb = cpBBMerge(cp_bb, _b->cp_bb);
   YAC_RETO(n, 1);
}

void CpBB::_expand_YAC_RARG(CpVect *_v, CpBB *_r) {
   _r->cp_bb = cpBBExpand(cp_bb, _v->cp_vect);
}

void CpBB::_expand_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpBB *n = YAC_NEW_POOLED(CpBB);
   n->cp_bb = cpBBExpand(cp_bb, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpBB::_expand_YAC_RSELF(CpVect *_v) {
   cp_bb = cpBBExpand(cp_bb, _v->cp_vect);
}

void CpBB::_getCenter_YAC_RARG(CpVect *_r) {
   _r->cp_vect = cpBBCenter(cp_bb);
}

void CpBB::_getCenter_YAC_RVAL(YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpBBCenter(cp_bb);
   YAC_RETO(n, 1);
}

sF32 CpBB::_getArea(void) {
   return cpBBArea(cp_bb);
}

sF32 CpBB::_getMergedArea(CpBB *_b) {
   return cpBBMergedArea(cp_bb, _b->cp_bb);
}

sF32 CpBB::_segmentQuery(CpVect *_a, CpVect *_b) {
   return cpBBSegmentQuery(cp_bb, _a->cp_vect, _b->cp_vect);
}

sBool CpBB::_intersectsSegment(CpVect *_a, CpVect *_b) {
   return cpBBIntersectsSegment(cp_bb, _a->cp_vect, _b->cp_vect);
}

void CpBB::_clampVect_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpBBClampVect(cp_bb, _v->cp_vect);
}

void CpBB::_clampVect_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpBBClampVect(cp_bb, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpBB::_wrapVect_YAC_RARG(CpVect *_v, CpVect *_r) {
   _r->cp_vect = cpBBWrapVect(cp_bb, _v->cp_vect);
}

void CpBB::_wrapVect_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *n = YAC_NEW_POOLED(CpVect);
   n->cp_vect = cpBBWrapVect(cp_bb, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpBB::_offset_YAC_RARG(CpVect *_v, CpBB *_r) {
   _r->cp_bb = cpBBOffset(cp_bb, _v->cp_vect);
}

void CpBB::_offset_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpBB *n = YAC_NEW_POOLED(CpBB);
   n->cp_bb = cpBBOffset(cp_bb, _v->cp_vect);
   YAC_RETO(n, 1);
}

void CpBB::_roffset_YAC_RARG(CpVect *_v, CpBB *_r) {
   _r->cp_bb = cpBBOffset(cp_bb, cpv(-_v->cp_vect.x, -_v->cp_vect.y));
}

void CpBB::_roffset_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpBB *n = YAC_NEW_POOLED(CpBB);
   n->cp_bb = cpBBOffset(cp_bb, cpv(-_v->cp_vect.x, -_v->cp_vect.y));
   YAC_RETO(n, 1);
}


// ---------------------------------------------------------------------------- CpTransform
static int oc_cptransform = 0;
CpTransform::CpTransform(void) {
   oc_cptransform++;
}

CpTransform::~CpTransform() {
   oc_cptransform--;
}

void YAC_VCALL CpTransform::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_VALID(_o))
   {
      if(YAC_BCHK(_o, clid_CpTransform))
      {
         CpTransform *o = (CpTransform*) _o;
         cp_transform.a  = o->cp_transform.a;
         cp_transform.b  = o->cp_transform.b;
         cp_transform.c  = o->cp_transform.c;
         cp_transform.d  = o->cp_transform.d;
         cp_transform.tx = o->cp_transform.tx;
         cp_transform.ty = o->cp_transform.ty;
      }
      else
      {
         // Assign generic
         sUI ne = _o->yacArrayGetNumElements();
         if(ne >= 6)
         {
            YAC_Value v;

            // Read 'a'
            _o->yacArrayGet(NULL, 0, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_transform.a = v.value.float_val;

            // Read 'b'
            _o->yacArrayGet(NULL, 1, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_transform.b = v.value.float_val;

            // Read 'c'
            _o->yacArrayGet(NULL, 2, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_transform.c = v.value.float_val;

            // Read 'd'
            _o->yacArrayGet(NULL, 3, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_transform.d = v.value.float_val;

            // Read 'tx'
            _o->yacArrayGet(NULL, 4, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_transform.tx = v.value.float_val;

            // Read 'ty'
            _o->yacArrayGet(NULL, 5, &v);
            v.typecast(YAC_TYPE_FLOAT);
            cp_transform.ty = v.value.float_val;
         }
      }
   }
}

void YAC_VCALL CpTransform::yacArrayGet(void *_ctx, sUI _index, YAC_Value *_value) {
   if(_index < 6)
   {
      switch(_index)
      {
         default:
         case 0:
            _value->initFloat(cp_transform.a);
            break;

         case 1:
            _value->initFloat(cp_transform.b);
            break;

         case 2:
            _value->initFloat(cp_transform.c);
            break;

         case 3:
            _value->initFloat(cp_transform.d);
            break;

         case 4:
            _value->initFloat(cp_transform.tx);
            break;

         case 5:
            _value->initFloat(cp_transform.ty);
            break;
      }
   }
   else
   {
      Dyac_throw(ReadArrayOutOfBounds, "");
   }
}

sBool YAC_VCALL CpTransform::yacToString(YAC_String *_r) const {
   _r->alloc(128);
   _r->printf("(a=%f; b=%f; c=%f; d=%f; tx=%f; ty=%f)", cp_transform.a, cp_transform.b, cp_transform.c, cp_transform.d, cp_transform.tx, cp_transform.ty);
   return 1;
}

sF32 CpTransform::_getA(void) {
   return cp_transform.a;
}

void CpTransform::_setA(sF32 _a) {
   cp_transform.a = _a;
}

sF32 CpTransform::_getB(void) {
   return cp_transform.b;
}

void CpTransform::_setB(sF32 _b) {
   cp_transform.b = _b;
}

sF32 CpTransform::_getC(void) {
   return cp_transform.c;
}

void CpTransform::_setC(sF32 _c) {
   cp_transform.c = _c;
}

sF32 CpTransform::_getD(void) {
   return cp_transform.d;
}

void CpTransform::_setD(sF32 _d) {
   cp_transform.d = _d;
}

sF32 CpTransform::_getTx(void) {
   return cp_transform.tx;
}

void CpTransform::_setTx(sF32 _tx) {
   cp_transform.tx = _tx;
}

sF32 CpTransform::_getTy(void) {
   return cp_transform.ty;
}

void CpTransform::_setTy(sF32 _ty) {
   cp_transform.ty = _ty;
}

void CpTransform::_init(sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _tx, sF32 _ty) {
   cp_transform.a  = _a;
   cp_transform.b  = _b;
   cp_transform.c  = _c;
   cp_transform.d  = _d;
   cp_transform.tx = _tx;
   cp_transform.ty = _ty;
}

void CpTransform::_initIdentity(void) {
   cp_transform.a  = 1.0f;
   cp_transform.b  = 0.0f;
   cp_transform.c  = 0.0f;
   cp_transform.d  = 1.0f;
   cp_transform.tx = 0.0f;
   cp_transform.ty = 0.0f;
}

void CpTransform::_getString(YAC_Value *_r) {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}


// ---------------------------------------------------------------------------- CpArbiter
CpArbiter::CpArbiter(void) {
   parent_space    = NULL;
   current_arbiter = NULL;

   cpv_surface_velocity = NULL;
   cpv_total_impulse    = NULL;
   cpv_normal           = NULL;
   cpv_point_a          = NULL;
   cpv_point_b          = NULL;
}

CpArbiter::~CpArbiter() {
   YAC_DELETE_SAFE(cpv_surface_velocity);
   YAC_DELETE_SAFE(cpv_total_impulse);
   YAC_DELETE_SAFE(cpv_normal);
   YAC_DELETE_SAFE(cpv_point_a);
   YAC_DELETE_SAFE(cpv_point_b);
}

YAC_Object *CpArbiter::_getShapeA(void) {
   cpShape *a;
   cpShape *b;
   cpArbiterGetShapes(current_arbiter, &a, &b);
   return (CpShape*)a->userData;
}

YAC_Object *CpArbiter::_getShapeB(void) {
   cpShape *a;
   cpShape *b;
   cpArbiterGetShapes(current_arbiter, &a, &b);
   return (CpShape*)b->userData;
}

YAC_Object *CpArbiter::_getBodyA(void) {
   cpBody *a;
   cpBody *b;
   cpArbiterGetBodies(current_arbiter, &a, &b);
   return (CpBody*)a->userData;
}

YAC_Object *CpArbiter::_getBodyB(void) {
   cpBody *a;
   cpBody *b;
   cpArbiterGetBodies(current_arbiter, &a, &b);
   return (CpBody*)b->userData;
}

float CpArbiter::_getRestitution(void) {
   return cpArbiterGetRestitution(current_arbiter);
}

void CpArbiter::_setRestitution(sF32 _restitution) {
   cpArbiterSetRestitution(current_arbiter, _restitution);
}

sF32 CpArbiter::_getFriction(void) {
   return cpArbiterGetFriction(current_arbiter);
}

void CpArbiter::_setFriction(float _friction) {
   cpArbiterSetFriction(current_arbiter, _friction);
}

YAC_Object *CpArbiter::_getSurfaceVelocity(void) {
   if(NULL == cpv_surface_velocity)
   {
      cpv_surface_velocity = YAC_NEW(CpVect);
   }
   cpv_surface_velocity->cp_vect = cpArbiterGetSurfaceVelocity(current_arbiter);
   return cpv_surface_velocity;

}

void CpArbiter::_setSurfaceVelocity(YAC_Object *_vr) {
   if(YAC_BCHK(_vr, clid_CpVect))
   {
      YAC_CAST_ARG(CpVect, vr, _vr);
      cpArbiterSetSurfaceVelocity(current_arbiter, vr->cp_vect);
   }
}

YAC_Object *CpArbiter::_getTotalImpulse(void) {
   if(NULL == cpv_total_impulse)
   {
      cpv_total_impulse = YAC_NEW(CpVect);
   }
   cpv_total_impulse->cp_vect = cpArbiterTotalImpulse(current_arbiter);
   return cpv_total_impulse;
}

float CpArbiter::_getTotalKE(void) {
   return cpArbiterTotalKE(current_arbiter);
}

void CpArbiter::_ignore(void) {
   cpArbiterIgnore(current_arbiter);
}

sBool CpArbiter::_isFirstContact(void) {
   return cpArbiterIsFirstContact(current_arbiter);
}

sBool CpArbiter::_isRemoval(void) {
   return cpArbiterIsRemoval(current_arbiter);
}

sSI CpArbiter::_getCount(void) {
   return cpArbiterGetCount(current_arbiter);
}

YAC_Object *CpArbiter::_getNormal(void) {
   if(NULL == cpv_normal)
   {
      cpv_normal = YAC_NEW(CpVect);
   }
   cpv_normal->cp_vect = cpArbiterGetNormal(current_arbiter);
   return cpv_normal;
}

YAC_Object *CpArbiter::_getPointA(void) {
   if(NULL == cpv_point_a)
   {
      cpv_point_a = YAC_NEW(CpVect);
   }
   cpv_point_a->cp_vect = cpArbiterGetPointA(current_arbiter, 0);
   return cpv_point_a;
}

YAC_Object *CpArbiter::_getPointAByIndex(sSI _i) {
   if(NULL == cpv_point_a)
   {
      cpv_point_a = YAC_NEW(CpVect);
   }
   cpv_point_a->cp_vect = cpArbiterGetPointA(current_arbiter, _i);
   return cpv_point_a;
}

YAC_Object *CpArbiter::_getPointB(void) {
   if(NULL == cpv_point_b)
   {
      cpv_point_b = YAC_NEW(CpVect);
   }
   cpv_point_b->cp_vect = cpArbiterGetPointB(current_arbiter, 0);
   return cpv_point_b;
}

YAC_Object *CpArbiter::_getPointBByIndex(sSI _i) {
   if(NULL == cpv_point_b)
   {
      cpv_point_b = YAC_NEW(CpVect);
   }
   cpv_point_b->cp_vect = cpArbiterGetPointB(current_arbiter, _i);
   return cpv_point_b;
}

sF32 CpArbiter::_getDepth(void) {
   return cpArbiterGetDepth(current_arbiter, 0);
}

sF32 CpArbiter::_getDepthByIndex(sSI _i) {
   return cpArbiterGetDepth(current_arbiter, _i);
}


// ---------------------------------------------------------------------------- CpSpace

// static void DebugPA(YAC_Object *_pa) {
//    if(NULL != _pa)
//    {
//       YAC_PointerArray *pa = (YAC_PointerArray*) _pa;

//       sUI i;
//       for(i=0; i<pa->max_elements; i++)
//       {
//          YAC_Object *o = pa->elements[i].value.object_val;
//          if(NULL != o)
//          {
//             printf("xxx delete pa[%d] validation_tag=0x%08x.\n", i, o->validation_tag);
//          }
//       }
      
//    }
// }


static int oc_cpspace = 0;
CpSpace::CpSpace(void) {
   cp_space      = NULL;
   vec_gravity   = NULL;
   shapes        = NULL;
   bodies        = NULL;
   constraints   = NULL;
   oc_cpspace++;
   memset((void*)collision_handlers, 0, sizeof(collision_handlers));
   arbiter = NULL;
   shape_remove_queue = NULL;
   body_remove_queue = NULL;
}

CpSpace::~CpSpace() {
   _free();
   oc_cpspace--;

   YAC_DELETE_SAFE(arbiter);
}

void CpSpace::_free(void) {
   if(NULL != cp_space)
   {
      cpSpaceFree(cp_space);
      cp_space = NULL;
   }

//    DebugPA(shapes);
//    DebugPA(bodies);
//    DebugPA(constraints);

   Dsafedelete(vec_gravity);
   Dsafedelete(shapes);
   Dsafedelete(bodies);
   Dsafedelete(constraints);
   Dsafedelete(shape_remove_queue);
   Dsafedelete(body_remove_queue);
}

void CpSpace::_init(void) {
   _free();
   cp_space = cpSpaceNew();
   shapes      = YAC_New_PointerArray();
   bodies      = YAC_New_PointerArray();
   constraints = YAC_New_PointerArray();
}

void CpSpace::_New(YAC_Value *_r) {
   CpSpace *s = YAC_NEW(CpSpace);
   s->_init();
   YAC_RETO(s, 1);
}

YAC_Object *CpSpace::_getGravity(void) {
   if(NULL == vec_gravity)
   {
      vec_gravity = YAC_NEW(CpVect);
   }
   vec_gravity->cp_vect = cp_space->gravity;
   return vec_gravity;
}

void CpSpace::_setGravity(CpVect *_gravity) {
   cp_space->gravity = _gravity->cp_vect;
}

void CpSpace::_setGravity2f(sF32 _x, sF32 _y) {
   cp_space->gravity.x = _x;
   cp_space->gravity.y = _y;
}

sF32 CpSpace::_getDamping(void) {
   return cp_space->damping;
}

void CpSpace::_setDamping(sF32 _damping) {
   cp_space->damping = _damping;
}

sSI CpSpace::_getIterations(void) {
   return cp_space->iterations;
}

void CpSpace::_setIterations(sSI _iterations) {
   cp_space->iterations = _iterations;
}

sF32 CpSpace::_getIdleSpeedThreshold(void) {
   return cpSpaceGetIdleSpeedThreshold(cp_space);
}

void CpSpace::_setIdleSpeedThreshold(sF32 _threshold) {
   cpSpaceSetIdleSpeedThreshold(cp_space, _threshold);
}

sF32 CpSpace::_getSleepTimeThreshold(void) {
   return cpSpaceGetSleepTimeThreshold(cp_space);
}

void CpSpace::_setSleepTimeThreshold(sF32 _sleepTimeThreshold) {
   cpSpaceSetSleepTimeThreshold(cp_space, _sleepTimeThreshold);
}

sF32 CpSpace::_getCollisionSlop(void) {
   return cpSpaceGetCollisionSlop(cp_space);
}

void CpSpace::_setCollisionSlop(sF32 _collisionSlop) {
   cpSpaceSetCollisionSlop(cp_space, _collisionSlop);
}

sF32 CpSpace::_getCollisionBias(void) {
   return cpSpaceGetCollisionBias(cp_space);
}

void CpSpace::_setCollisionBias(sF32 _collisionBias) {
   cpSpaceSetCollisionBias(cp_space, _collisionBias);
}

sSI CpSpace::_getCollisionPersistence(void) {
   return (sSI)cpSpaceGetCollisionPersistence(cp_space);
}

void CpSpace::_setCollisionPersistence(sSI _collisionPersistence) {
   cpSpaceSetCollisionPersistence(cp_space, (cpTimestamp)_collisionPersistence);
}

static sBool GetWrappedObject(YAC_Object *_o, sUI _clid, YAC_Object **_retObject, sBool *_retBDelete) {
   if(YAC_VALID(_o))
   {
      YAC_Object *o;
      if(YAC_BCHK(_o, YAC_CLID_VALUE))
      {
         YAC_ValueObject *vo = (YAC_ValueObject*) _o;
         if(YAC_TYPE_OBJECT <= vo->type)
         {
            // Unlink object from value container 
            *_retBDelete = vo->deleteme;
            vo->deleteme = 0;
            o = vo->value.object_val;
            if(!(YAC_VALID(o)))
            {
               Dyac_throw_def(InvalidPointer, "invalid value-wrapped Object pointer");
               return 0;
            }
         }
         else
         {
            Dyac_throw_def(TypeMismatch, "expected Object type argument.");
            return 0;
         }
      }
      else
      {
         o = _o;
         *_retBDelete = 0;
      }
      if(YAC_BCHK(o, _clid))
      {
         // ok, class type matches
         *_retObject = o;
         return 1;
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "object has wrong class ID");
         return 0;
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "invalid Object pointer");
      return 0;
   }
}

void CpSpace::_addShape(YAC_Object *_shape) {
   sBool bDelete;
   YAC_Object *o;
   if(GetWrappedObject(_shape, clid_CpShape, &o, &bDelete))
   {
      CpShape *shape = (CpShape*) o;
      cpSpaceAddShape(cp_space, shape->cp_shape);
      shapes->add(shape, bDelete);
   }
}

void CpSpace::_addBody(YAC_Object *_body) {
   sBool bDelete;
   YAC_Object *o;

   if(GetWrappedObject(_body, clid_CpBody, &o, &bDelete))
   {
      CpBody *body = (CpBody*) o;
      cpSpaceAddBody(cp_space, body->cp_body);
      bodies->add(body, bDelete);
   }
}

void CpSpace::_addConstraint(YAC_Object *_constraint) {
   sBool bDelete;
   YAC_Object *o;
   if(GetWrappedObject(_constraint, clid_CpConstraint, &o, &bDelete))
   {
      CpConstraint *constraint = (CpConstraint*) o;
      cpSpaceAddConstraint(cp_space, constraint->cp_constraint);
      constraints->add(constraint, bDelete);
   }
}

void CpSpace::_addJoint(YAC_Object *_joint) {
   _addConstraint(_joint);
}

void CpSpace::_removeShape(CpShape *_shape) {
   if(YAC_VALID(_shape))
   {
      cpSpaceRemoveShape(cp_space, _shape->cp_shape);
      shapes->removeIndex(shapes->indexOfPointer(_shape, 0));
   }
}

void CpSpace::_queueRemoveShapeAndBody(CpShape *_shape) {
   if(NULL == shape_remove_queue)
      shape_remove_queue = YAC_New_PointerArray();

   shape_remove_queue->add(_shape, YAC_FALSE/*bDelete*/);

   if(NULL == body_remove_queue)
      body_remove_queue = YAC_New_PointerArray();
   body_remove_queue->add(_shape->body, YAC_FALSE/*bDelete*/);
}

void CpSpace::_removeBody(CpBody *_body) {
   if(YAC_VALID(_body))
   {
      cpSpaceRemoveBody(cp_space, _body->cp_body);
      bodies->removeIndex(bodies->indexOfPointer(_body, 0));
   }
}

void CpSpace::_removeConstraint(CpConstraint *_constraint) {
   if(YAC_VALID(_constraint))
   {
      cpSpaceRemoveConstraint(cp_space, _constraint->cp_constraint);
      constraints->removeIndex(constraints->indexOfPointer(_constraint, 0));
   }
}

void CpSpace::_freeChildren(void) {

   // Remove (active) shapes
   while(0 != shapes->num_elements)
   {
      _removeShape( (CpShape*) shapes->elements[--shapes->num_elements].value.object_val );
   }

   // Remove constraints
   while(0 != constraints->num_elements)
   {
      _removeConstraint( (CpConstraint*) constraints->elements[--constraints->num_elements].value.object_val );
   }

   // Remove bodies
   while(0 != bodies->num_elements)
   {
      _removeBody( (CpBody*) bodies->elements[--bodies->num_elements].value.object_val );
   }
}

YAC_Object *CpSpace::_getShapes(void) {
   return shapes;
}

YAC_Object *CpSpace::_getBodies(void) {
   return bodies;
}

YAC_Object *CpSpace::_getConstraints(void) {
   return constraints;
}

cpBool tkchipmunk_coll_begin(cpArbiter *arb, cpSpace *space, cpDataPointer userData) {
   CpSpaceCollisionHandler *ch = (CpSpaceCollisionHandler *)userData;

   // yac_host->printf("xxx tkchipmunk_coll_begin: fun=%p\n", ch->fun_begin);

   if(YAC_VALID(ch->fun_begin))
   {
      // yac_host->printf("xxx 2 tkchipmunk_coll_begin\n");

      CpArbiter *yArb = ch->parent_space->arbiter;
      yArb->current_arbiter = arb;

      YAC_Value args[3];
      args[0].initObject(yArb, 0);
      args[1].initObject(ch->parent_space, 0);
      args[2].initObject(ch->user_data, 0);

      YAC_Value r;
      yac_host->yacEvalFunctionReturn(yac_host->yacContextGetDefault(), ch->fun_begin, 3, args, &r);

      r.typecast(YAC_TYPE_INT);
      return r.value.int_val;
   }
   else
   {
      return 1;
   }
}

cpBool tkchipmunk_coll_pre_solve(cpArbiter *arb, cpSpace *space, cpDataPointer userData) {
   CpSpaceCollisionHandler *ch = (CpSpaceCollisionHandler *)userData;

   if(YAC_VALID(ch->fun_pre_solve))
   {
      CpArbiter *yArb = ch->parent_space->arbiter;
      yArb->current_arbiter = arb;

      YAC_Value args[3];
      args[0].initObject(yArb, 0);
      args[1].initObject(ch->parent_space, 0);
      args[2].initObject(ch->user_data, 0);

      YAC_Value r;
      yac_host->yacEvalFunctionReturn(yac_host->yacContextGetDefault(), ch->fun_pre_solve, 3, args, &r);

      r.typecast(YAC_TYPE_INT);
      return r.value.int_val;
   }
   else
   {
      return 1;
   }
}

void tkchipmunk_coll_post_solve(cpArbiter *arb, cpSpace *space, cpDataPointer userData) {
   CpSpaceCollisionHandler *ch = (CpSpaceCollisionHandler *)userData;

   if(YAC_VALID(ch->fun_post_solve))
   {
      CpArbiter *yArb = ch->parent_space->arbiter;
      yArb->current_arbiter = arb;

      YAC_Value args[3];
      args[0].initObject(yArb, 0);
      args[1].initObject(ch->parent_space, 0);
      args[2].initObject(ch->user_data, 0);

      yac_host->yacEvalFunction(yac_host->yacContextGetDefault(), ch->fun_post_solve, 3, args);
   }
}

void tkchipmunk_coll_separate(cpArbiter *arb, cpSpace *space, cpDataPointer userData) {
   CpSpaceCollisionHandler *ch = (CpSpaceCollisionHandler *)userData;

   if(YAC_VALID(ch->fun_separate))
   {
      CpArbiter *yArb = ch->parent_space->arbiter;
      yArb->current_arbiter = arb;

      YAC_Value args[3];
      args[0].initObject(yArb, 0);
      args[1].initObject(ch->parent_space, 0);
      args[2].initObject(ch->user_data, 0);

      yac_host->yacEvalFunction(yac_host->yacContextGetDefault(), ch->fun_separate, 3, args);
   }
}

sBool CpSpace::_addCollisionHandlers(sUI _collisionTypeA,
                                     sUI _collisionTypeB,
                                     YAC_Object *_funBeginOrNull,
                                     YAC_Object *_funPreSolveOrNull,
                                     YAC_Object *_funPostSolveOrNull,
                                     YAC_Object *_funSeparateOrNull,
                                     YAC_Object *_userData
                                     ) {

   if(NULL == arbiter)
   {
      arbiter = YAC_NEW(CpArbiter);
      arbiter->parent_space = this;
   }

   sUI i = 0u;
   CpSpaceCollisionHandler *ch = NULL;
   for(; i < CPSPACE_MAX_COLLISION_HANDLERS; i++)
   {
      ch = &collision_handlers[i];

      if(NULL != ch->cp_collision_handler)
      {
         if(ch->collision_type_a == _collisionTypeA)
         {
            if(ch->collision_type_b == _collisionTypeB)
            {
               // Re-use existing handler
               break;
            }
         }
      }
      else
      {
         break;
      }
   }

   if(i < CPSPACE_MAX_COLLISION_HANDLERS)
   {
      if(NULL == ch->cp_collision_handler)
      {
         // Init new collision handler
         ch->parent_space = this;

         if(0u == _collisionTypeB)
         {
            if(0u == _collisionTypeA)
            {
               ch->cp_collision_handler = cpSpaceAddDefaultCollisionHandler(cp_space);
            }
            else
            {
               ch->cp_collision_handler = cpSpaceAddWildcardHandler(cp_space, _collisionTypeA);
            }
         }
         else
         {
            ch->cp_collision_handler = cpSpaceAddCollisionHandler(cp_space, _collisionTypeA, _collisionTypeB);
         }

         ch->collision_type_a = _collisionTypeA;
         ch->collision_type_b = _collisionTypeB;
         
         // Init chipmunk cpCollisionHandler callbacks and userData
         ch->cp_collision_handler->beginFunc     = &tkchipmunk_coll_begin;
         ch->cp_collision_handler->preSolveFunc  = &tkchipmunk_coll_pre_solve;
         ch->cp_collision_handler->postSolveFunc = &tkchipmunk_coll_post_solve;
         ch->cp_collision_handler->separateFunc  = &tkchipmunk_coll_separate;
         ch->cp_collision_handler->userData      = (void*)ch;
      }

      // Update TKS callbacks and user_data
      ch->fun_begin      = _funBeginOrNull;
      ch->fun_pre_solve  = _funPreSolveOrNull;
      ch->fun_post_solve = _funPostSolveOrNull;
      ch->fun_separate   = _funSeparateOrNull;
      ch->user_data      = _userData;

      return YAC_TRUE;
   }
   else
   {
      yac_host->printf("[---] CpSpace::addCollisionHandlers: CPSPACE_MAX_COLLISION_HANDLERS(%u) exceeded\n", CPSPACE_MAX_COLLISION_HANDLERS);
   }
   return YAC_FALSE;
}

static void loc_cpspace_bb_query_func(cpShape *_shape, void *_userDataCpSpace) {
   CpSpace *space = (CpSpace*)_userDataCpSpace;

   YAC_Value args[2];
   args[0].initObject((CpShape*)_shape->userData, 0);
   args[1].initObject(space->bb_query_user_data, 0);

   yac_host->yacEvalFunction(yac_host->yacContextGetDefault(), space->bb_query_fun, 2, args);
}

CpShape *CpSpace::_pointQueryNearest(CpVect *_point, sF32 _maxDistance) {
   
   cpShapeFilter flt;
   flt.group = 0;
   flt.categories = ~0u;
   flt.mask       = ~0u;

   cpPointQueryInfo queryInfo;
   cpShape *s = cpSpacePointQueryNearest(cp_space, _point->cp_vect, _maxDistance, flt, &queryInfo);

   if(NULL != s)
   {
      return (CpShape*)s->userData;
   }

   return NULL;
}

void CpSpace::_bbQuery(CpBB *_bb, YAC_Object *_funMatch, YAC_Object *_userData) {
   if(YAC_VALID(_funMatch))
   {
      if(YAC_BCHK(_bb, clid_CpBB))
      {
         YAC_CAST_ARG(CpBB, bb, _bb);

         cpShapeFilter flt;
         flt.group = 0;
         flt.categories = ~0u;
         flt.mask       = ~0u;

         bb_query_fun       = _funMatch;
         bb_query_user_data = _userData;
         
         cpSpaceBBQuery(cp_space, bb->cp_bb, flt, &loc_cpspace_bb_query_func, (void*)this/*userData*/);
      }
   }
}

void CpSpace::_useSpatialHash(sF32 _dim, sSI _count) {
   cpSpaceUseSpatialHash(cp_space, _dim, _count);
}

void CpSpace::_reindexStatic(void) {
   cpSpaceReindexStatic(cp_space);
}

void CpSpace::_step(sF32 _dt) {
   cpSpaceStep(cp_space, _dt);

   if(NULL != shape_remove_queue)
   {
      while(0 != shape_remove_queue->num_elements)
      {
         _removeShape( (CpShape*) shape_remove_queue->elements[--shape_remove_queue->num_elements].value.object_val );
      }
   }

   if(NULL != shape_remove_queue)
   {
      while(0 != body_remove_queue->num_elements)
      {
         _removeBody( (CpBody*) body_remove_queue->elements[--body_remove_queue->num_elements].value.object_val );
      }
   }
   
}



// ---------------------------------------------------------------------------- CpBody
static int oc_cpbody = 0;
CpBody::CpBody(void) {
   cp_body = cpBodyAlloc();
   cp_body->userData = (void*)this;
   b_delete_body = YAC_TRUE;
   vec_p   = NULL;
   vec_cog = NULL;
   vec_v   = NULL;
   vec_f   = NULL;
   vec_rot = NULL;
   id      = NULL;
   oc_cpbody++;
}

CpBody::~CpBody() {
   Dsafedelete(vec_p);
   Dsafedelete(vec_cog);
   Dsafedelete(vec_v);
   Dsafedelete(vec_f);
   Dsafedelete(vec_rot);
   Dsafedelete(id);

   freeBody();

   oc_cpbody--;
}

void CpBody::freeBody(void) {
   if(b_delete_body)
   {
      if(NULL != cp_body)
      {
         cpBodyFree(cp_body);
         cp_body = NULL;
      }
      b_delete_body = YAC_FALSE;
   }
}

void CpBody::_init(sF32 _mass, sF32 _inertia) {
   // printf("xxx CpBody::_init: cp_body=%p mass=%f inertia=%f\n", cp_body, _mass, _inertia);
   cpBodyInit(cp_body, _mass, _inertia);
}

void CpBody::_New(sF32 _mass, sF32 _inertia, YAC_Value *_r) {
   CpBody *b = YAC_NEW(CpBody);
   b->_init(_mass, _inertia);
   YAC_RETO(b, 1);
}

void CpBody::_activate(void) {
   cpBodyActivate(cp_body);
}

void CpBody::_activateStatic(CpShape *_shape) {
   cpBodyActivateStatic(cp_body, _shape->cp_shape);
}

void CpBody::_sleep(void) {
   cpBodySleep(cp_body);
}

void CpBody::_sleepWithGroup(CpBody *_group) {
   cpBodySleepWithGroup(cp_body, _group->cp_body);
}

sBool CpBody::_isSleeping(void) {
   return cpBodyIsSleeping(cp_body);
}

sSI CpBody::_getType(void) {
   return cpBodyGetType(cp_body);
}

void CpBody::_setType(sSI _bodyType) {
   cpBodySetType(cp_body, (cpBodyType)_bodyType);
}

YAC_Object *CpBody::_getPosition(void) {
   if(NULL == vec_p)
   {
      vec_p = YAC_NEW(CpVect);
   }
   vec_p->cp_vect = cpBodyGetPosition(cp_body);
   return vec_p;
}

YAC_Object *CpBody::_getP(void) {
   return _getPosition();
}

void CpBody::_setPosition(CpVect *_p) {
   cpBodySetPosition(cp_body, _p->cp_vect);
}

void CpBody::_setP(CpVect *_p) {
   _setPosition(_p);
   
}

YAC_Object *CpBody::_getCenterOfGravity(void) {
   if(NULL == vec_cog)
   {
      vec_cog = YAC_NEW(CpVect);
   }
   vec_cog->cp_vect = cpBodyGetCenterOfGravity(cp_body);
   return vec_cog;
}

YAC_Object *CpBody::_getCog(void) {
   return _getCenterOfGravity();
}

void CpBody::_setCenterOfGravity(CpVect *_p) {
   cpBodySetCenterOfGravity(cp_body, _p->cp_vect);
}

void CpBody::_setCog(CpVect *_p) {
   _setCenterOfGravity(_p);
}

YAC_Object *CpBody::_getVelocity(void) {
   if(NULL == vec_v)
   {
      vec_v = YAC_NEW(CpVect);
   }
   vec_v->cp_vect = cpBodyGetVelocity(cp_body);
   return vec_v;
}

YAC_Object *CpBody::_getV(void) {
   return _getVelocity();
}

void CpBody::_setVelocity(CpVect *_v) {
   cpBodySetVelocity(cp_body, _v->cp_vect);
}

void CpBody::_setV(CpVect *_v) {
   _setVelocity(_v);
}

sF32 CpBody::_getAngularVelocity(void) {
   return cpBodyGetAngularVelocity(cp_body);
}

sF32 CpBody::_getW(void) {
   return _getAngularVelocity();
}

void CpBody::_setAngularVelocity(sF32 _w) {
   cpBodySetAngularVelocity(cp_body, _w);
}

void CpBody::_setW(sF32 _w) {
   _setAngularVelocity(_w);
}

YAC_Object *CpBody::_getForce(void) {
   if(NULL == vec_f)
   {
      vec_f = YAC_NEW(CpVect);
   }
   vec_f->cp_vect = cpBodyGetForce(cp_body);
   return vec_f;
}

YAC_Object *CpBody::_getF(void) {
   return _getForce();
}

void CpBody::_setForce(CpVect *_f) {
   cpBodySetForce(cp_body, _f->cp_vect);
}

void CpBody::_setF(CpVect *_f) {
   _setForce(_f);
}

YAC_Object *CpBody::_getRotation(void) {
   if(NULL == vec_rot)
   {
      vec_rot = YAC_NEW(CpVect);
   }
   vec_rot->cp_vect = cpBodyGetRotation(cp_body);
   return vec_rot;
}

YAC_Object *CpBody::_getRot(void) {
   if(NULL == vec_rot)
   {
      vec_rot = YAC_NEW(CpVect);
   }
   vec_rot->cp_vect.x = cp_body->transform.a;
   vec_rot->cp_vect.y = cp_body->transform.b;
   return vec_rot;
}

void CpBody::_setRot(CpVect *_rot) {
   cp_body->transform.a = _rot->cp_vect.x;
   cp_body->transform.b = _rot->cp_vect.y;
}

sF32 CpBody::_getTorque(void) {
   return cpBodyGetTorque(cp_body);
}

sF32 CpBody::_getT(void) {
   return _getTorque();
}

void CpBody::_setTorque(sF32 _t) {
   cpBodySetTorque(cp_body, _t);
}

void CpBody::_setT(sF32 _t) {
   _setTorque(_t);
}

sF32 CpBody::_getMass(void) { 
   return cpBodyGetMass(cp_body);
}

sF32 CpBody::_getM(void) { 
   return _getMass();
}

void CpBody::_setMass(sF32 _m) { 
   cpBodySetMass(cp_body, _m);
}

void CpBody::_setM(sF32 _m) { 
   _setMass(_m);
}

sF32 CpBody::_getMoment(void) { 
   return cpBodyGetMoment(cp_body);
}

sF32 CpBody::_getI(void) { 
   return _getMoment();
}

void CpBody::_setMoment(sF32 _inertia) { 
   cpBodySetMoment(cp_body, _inertia);
}

void CpBody::_setI(sF32 _inertia) { 
   _setMoment(_inertia);
}

sF32 CpBody::_getAngle(void) { 
   return cpBodyGetAngle(cp_body);
}

sF32 CpBody::_getA(void) { 
   return _getAngle();
}

void CpBody::_setAngle(sF32 _angle) { 
   cpBodySetAngle(cp_body, _angle);
}

void CpBody::_setA(sF32 _angle) { 
   _setAngle(_angle);
}

void CpBody::_localToWorld_YAC_RARG(CpVect *_v, CpVect *_r) { 
   _r->cp_vect = cpBodyLocalToWorld(cp_body, _v->cp_vect);
}

void CpBody::_localToWorld_YAC_RVAL(CpVect *_v, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpBodyLocalToWorld(cp_body, _v->cp_vect);
   YAC_RETO(r, 1);
}

void CpBody::_worldToLocal_YAC_RARG(CpVect *_v, CpVect *_r) { 
   _r->cp_vect = cpBodyWorldToLocal(cp_body, _v->cp_vect);
}

void CpBody::_worldToLocal_YAC_RVAL(CpVect *_v, YAC_Value *_r) { 
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpBodyWorldToLocal(cp_body, _v->cp_vect);
   YAC_RETO(r, 1);
}

void CpBody::_applyImpulseAtWorldPoint(CpVect *_impulse, CpVect *_point) {
   cpBodyApplyImpulseAtWorldPoint(cp_body, _impulse->cp_vect, _point->cp_vect);
}

void CpBody::_applyImpulse(CpVect *_impulse, CpVect *_point) {
   // chipmunk4 backwards compatibility
   cpBodyApplyImpulseAtWorldPoint(cp_body, _impulse->cp_vect, cpvadd(cp_body->p, _point->cp_vect/*offset*/));
}

void CpBody::_applyImpulseAtLocalPoint(CpVect *_impulse, CpVect *_point) {
   cpBodyApplyImpulseAtLocalPoint(cp_body, _impulse->cp_vect, _point->cp_vect);
}

void CpBody::_applyForceAtWorldPoint(CpVect *_force, CpVect *_point) {
   cpBodyApplyForceAtWorldPoint(cp_body, _force->cp_vect, _point->cp_vect);
}

void CpBody::_applyForce(CpVect *_force, CpVect *_point) {
   // chipmunk4 backwards compatibility
   cpBodyApplyForceAtWorldPoint(cp_body, _force->cp_vect, cpvadd(cp_body->p, _point->cp_vect/*offset*/));
}

void CpBody::_applyForceAtLocalPoint(CpVect *_force, CpVect *_point) {
   cpBodyApplyForceAtLocalPoint(cp_body, _force->cp_vect, _point->cp_vect);
}

void CpBody::getVelocityAtWorldPoint(CpVect *_point, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpBodyGetVelocityAtWorldPoint(cp_body, _point->cp_vect);
   YAC_RETO(r, 1);
}

void CpBody::getVelocityAtLocalPoint(CpVect *_point, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpBodyGetVelocityAtLocalPoint(cp_body, _point->cp_vect);
   YAC_RETO(r, 1);
}

void CpBody::_updateVelocity(CpVect *_gravity, sF32 _damping, sF32 _dt) { 
   cpBodyUpdateVelocity(cp_body, _gravity->cp_vect, _damping, _dt);
}

void CpBody::_updatePosition(sF32 _dt) { 
   cpBodyUpdatePosition(cp_body, _dt);
}

sF32 CpBody::_getKineticEnergy(void) {
   return cpBodyKineticEnergy(cp_body);
}

void CpBody::_setId(YAC_Object *_s) {
   if(YAC_Is_String(_s))
   {
      if(NULL == id)
      {
         id = YAC_New_String();
         id->yacCopy(_s);
      }
   }
}

YAC_Object *CpBody::_getId(void) {
   return id;
}


// ---------------------------------------------------------------------------- CpShape
static int oc_cpshape = 0;
CpShape::CpShape(void) {
   cp_shape      = NULL;
   body          = NULL;
   vec_surface_v = NULL;
   vec_cog       = NULL;
   oc_cpshape++;
   render_style  = 0;
   id            = NULL;
   user_data.initVoid();
   display_rotation = 0.0f;
}

CpShape::~CpShape() {
   _free();
   oc_cpshape--;
}

void CpShape::_free(void) {
   if(NULL != cp_shape)
   {
      cpShapeFree(cp_shape);
      cp_shape = NULL;
   }
   
   body = NULL;

   Dsafedelete(vec_surface_v);
   Dsafedelete(vec_cog);
   Dsafedelete(id);
   user_data.unset();
}

YAC_Object *CpShape::_getBody(void) {
   return body;
}

sSI CpShape::_getType(void) {
   // // return cp_shape->type;
   return (NULL != cp_shape->klass) ? cp_shape->klass->type : -1;
}

sF32 CpShape::_getMass(void) {
   return cpShapeGetMass(cp_shape);
}

void CpShape::_setMass(sF32 _mass) {
   cpShapeSetMass(cp_shape, _mass);
}

sF32 CpShape::_getDensity(void) {
   return cpShapeGetDensity(cp_shape);
}

void CpShape::_setDensity(sF32 _density) {
   cpShapeSetDensity(cp_shape, _density);
}

sF32 CpShape::_getMoment(void) {
   return cpShapeGetMoment(cp_shape);
}

sF32 CpShape::_getArea(void) {
   return cpShapeGetArea(cp_shape);
}

YAC_Object *CpShape::_getCenterOfGravity(void) {
   if(NULL == vec_cog)
   {
      vec_cog = YAC_NEW(CpVect);
   }
   vec_cog->cp_vect = cpShapeGetCenterOfGravity(cp_shape);
   return vec_cog;
}

sBool CpShape::_getSensor(void) {
   return cp_shape->sensor;
}

void CpShape::_setSensor(sBool _bEnable) {
   cp_shape->sensor = _bEnable;
}

YAC_Object *CpShape::_getSurfaceVelocity(void) {
   if(NULL == vec_surface_v)
   {
      vec_surface_v = YAC_NEW(CpVect);
   }
   vec_surface_v->cp_vect = cpShapeGetSurfaceVelocity(cp_shape);
   return vec_surface_v;
}

void CpShape::_setSurfaceVelocity(CpVect *_v) {
   cpShapeSetSurfaceVelocity(cp_shape, _v->cp_vect);
}

void CpShape::_setSurfaceVelocity2f(sF32 _x, sF32 _y) {
   cp_shape->surfaceV.x = _x;
   cp_shape->surfaceV.y = _y;
}

sF32 CpShape::_getElasticity(void) {
   return cpShapeGetElasticity(cp_shape);
}

sF32 CpShape::_getE(void) {
   return _getElasticity();
}

void CpShape::_setElasticity(sF32 _e) {
   cpShapeSetElasticity(cp_shape, _e);
}

void CpShape::_setE(sF32 _e) {
   _setElasticity(_e);
}

sF32 CpShape::_getFriction(void) {
   return cpShapeGetFriction(cp_shape);
}

sF32 CpShape::_getU(void) {
   return _getFriction();
}

void CpShape::_setFriction(sF32 _friction) {
   cpShapeSetFriction(cp_shape, _friction);
}

void CpShape::_setU(sF32 _friction) {
   _setFriction(_friction);
}

sSI CpShape::_getFilterGroup(void) {
   return (sSI) cp_shape->filter.group;
}

sSI CpShape::_getGroup(void) {
   return _getFilterGroup();
}

void CpShape::_setFilterGroup(sSI _group) {
   cp_shape->filter.group = (unsigned long) _group;
}

void CpShape::_setGroup(sSI _group) {
   _setFilterGroup(_group);
}

sSI CpShape::_getFilterCategories(void) {
   return (sSI) cp_shape->filter.categories;
}

void CpShape::_setFilterCategories(sSI _categories) {
   cp_shape->filter.categories = (unsigned long) _categories;
}

sSI CpShape::_getFilterMask(void) {
   return (sSI) cp_shape->filter.mask; //layers;
}

void CpShape::_setFilterMask(sSI _layers) {
   cp_shape->filter.mask = (unsigned long) _layers;
}

sSI CpShape::_getCollisionType(void) {
   return (sSI) cpShapeGetCollisionType(cp_shape);
}

void CpShape::_setCollisionType(sSI _collisionType) {
   cpShapeSetCollisionType(cp_shape, (unsigned long)_collisionType);
}

void CpShape::_setRenderStyle(sSI _type) {
   render_style = _type;
}

sSI CpShape::_getRenderStyle(void) {
   return render_style;
}

void CpShape::_setId(YAC_Object *_s) {
   if(YAC_Is_String(_s))
   {
      if(NULL == id)
      {
         id = YAC_New_String();
         id->yacCopy(_s);
      }
   }
}

YAC_Object *CpShape::_getId(void) {
   return id;
}

void CpShape::_setUserData(YAC_Object *_v) {
   user_data.unsetFast();
   if(YAC_Is_Value(_v))
   {
      YAC_CAST_ARG(YAC_ValueObject, vo, _v);
      user_data = vo;
   }
   else
   {
      user_data.initObject(_v, 0);
   }
}

void CpShape::_getUserData(YAC_Value *_r) {
   _r->copyRef(&user_data);
}

sF32 CpShape::_getDisplayRotation(void) {
   return display_rotation;
}

void CpShape::_setDisplayRotation(sF32 _rot) {
   display_rotation = _rot;
}



// ---------------------------------------------------------------------------- CpCircleShape
CpCircleShape::CpCircleShape(void) {
   vec_c = NULL;
}

CpCircleShape::~CpCircleShape() {
   _free();
}

void CpCircleShape::_free(void) {
   CpShape::_free();

   Dsafedelete(vec_c);
}

void CpCircleShape::_initCircle(CpBody *_body, sF32 _radius, CpVect *_offset) {
   _free();
   body = _body;
   cp_shape = cpCircleShapeNew(_body->cp_body, _radius, _offset->cp_vect);
   cp_shape->userData = (void*)this;
   // yac_host->printf("xxx initCircle: cp_shape.type=%d\n", cp_shape->type);
}

void CpCircleShape::_New(CpBody *_body, sF32 _radius, CpVect *_offset, YAC_Value *_r) {
   CpCircleShape *s = YAC_NEW(CpCircleShape);
   s->_initCircle(_body, _radius, _offset);
   YAC_RETO(s, 1);
}

YAC_Object *CpCircleShape::_getC(void) {
   if(NULL == vec_c)
   {
      vec_c = YAC_NEW(CpVect);
   }
   vec_c->cp_vect = ((cpCircleShape*)cp_shape)->c;
   return vec_c;
}

void CpCircleShape::_setC(CpVect *_c) {
   ((cpCircleShape*)cp_shape)->c = _c->cp_vect;
}

sF32 CpCircleShape::_getR(void) {
   return ((cpCircleShape*)cp_shape)->r;
}

void CpCircleShape::_setR(sF32 _r) {
   ((cpCircleShape*)cp_shape)->r = _r;
}


// ---------------------------------------------------------------------------- CpSegmentShape
CpSegmentShape::CpSegmentShape(void) {
   vec_a = NULL;
   vec_b = NULL;
   vec_n = NULL;
}

CpSegmentShape::~CpSegmentShape() {
   _free();
}

void CpSegmentShape::_free(void) {
   CpShape::_free();

   Dsafedelete(vec_a);
   Dsafedelete(vec_b);
   Dsafedelete(vec_n);
}

void CpSegmentShape::_initSegment(CpBody *_body, CpVect *_a, CpVect *_b, sF32 _radius) {
   _free();
   body = _body;
   cp_shape = cpSegmentShapeNew(_body->cp_body, _a->cp_vect, _b->cp_vect, _radius);
   cp_shape->userData = (void*)this;
   // yac_host->printf("xxx initSegment: cp_shape.type=%d\n", cp_shape->type);
}

void CpSegmentShape::_New(CpBody *_body, CpVect *_a, CpVect *_b, sF32 _radius, YAC_Value *_r) {
   CpSegmentShape *s = YAC_NEW(CpSegmentShape);
   s->_initSegment(_body, _a, _b, _radius);
   YAC_RETO(s, 1);
}

YAC_Object *CpSegmentShape::_getA(void) {
   if(NULL == vec_a)
   {
      vec_a = YAC_NEW(CpVect);
   }
   vec_a->cp_vect = ((cpSegmentShape*)cp_shape)->a;
   return vec_a;
}

void CpSegmentShape::_setA(CpVect *_a) {
   ((cpSegmentShape*)cp_shape)->a = _a->cp_vect;
}

YAC_Object *CpSegmentShape::_getB(void) {
   if(NULL == vec_b)
   {
      vec_b = YAC_NEW(CpVect);
   }
   vec_b->cp_vect = ((cpSegmentShape*)cp_shape)->b;
   return vec_b;
}

void CpSegmentShape::_setB(CpVect *_b) {
   ((cpSegmentShape*)cp_shape)->b = _b->cp_vect;
}

YAC_Object *CpSegmentShape::_getN(void) {
   if(NULL == vec_n)
   {
      vec_n = YAC_NEW(CpVect);
   }
   vec_n->cp_vect = ((cpSegmentShape*)cp_shape)->n;
   return vec_n;
}

void CpSegmentShape::_setN(CpVect *_n) {
   ((cpSegmentShape*)cp_shape)->n = _n->cp_vect;
}

sF32 CpSegmentShape::_getR(void) {
   return ((cpSegmentShape*)cp_shape)->r;
}

void CpSegmentShape::_setR(sF32 _r) {
   ((cpSegmentShape*)cp_shape)->r = _r;
}


// ---------------------------------------------------------------------------- CpPolyShape
CpPolyShape::CpPolyShape(void) {
   vertex_array  = NULL;
}

CpPolyShape::~CpPolyShape() {
   _free();
}

void CpPolyShape::_free(void) {
   CpShape::_free();

   Dsafedelete(vertex_array);
}

static sBool DecodeVertArray(YAC_Object *_vertArray, sUI *_retNum, cpVect**_retVec, YAC_PointerArray *_pa, cpVect _offset) {
   //
   // Note: caller must delete[] _retVec array 
   //

   *_retNum = 0;
   *_retVec = NULL;

   if(YAC_VALID(_vertArray))
   {
      // Convert _vertArray to array of cpVect structures
      sUI ne = _vertArray->yacArrayGetNumElements();
      if(ne > 0)
      {
         cpVect *v = new cpVect[ne];

         if(NULL != _pa)
         {
            _pa->yacArrayAlloc(ne, 0,0,0);
            _pa->num_elements = ne;
            _pa->max_elements = ne;
         }
         
         sUI i;
         for(i=0; i<ne; i++)
         {
            YAC_Value r;
            _vertArray->yacArrayGet(NULL, i, &r);
            if(YAC_TYPE_OBJECT == r.type)
            {
               // Read x/y values from array-type array element
               YAC_Object *e = r.value.object_val;
               if(YAC_VALID(e))
               {
                  YAC_Value t;
                  e->yacArrayGet(NULL, 0, &t);
                  t.typecast(YAC_TYPE_FLOAT);
                  v[i].x = t.value.float_val;
                  e->yacArrayGet(NULL, 1, &t);
                  t.typecast(YAC_TYPE_FLOAT);
                  v[i].y = t.value.float_val;

                  if(NULL != _pa)
                  {
                     CpVect *n = YAC_NEW(CpVect);
                     n->cp_vect.x = v[i].x + _offset.x;
                     n->cp_vect.y = v[i].y + _offset.y;
                     _pa->elements[i].initObject(n, 1);
                  }
               }
               else
               {
                  delete [] v;
                  Dyac_throw_def(TypeMismatch, "_vertArray element is not a valid object.");
                  return 0;
               }
            }
            else
            {
               r.unset();
               delete [] v;
               Dyac_throw_def(TypeMismatch, "_vertArray element is not an object.");
               return 0;
            }
            r.unsetFast();
         }
         
         // Ok
         *_retNum = ne;
         *_retVec = v;
         return 1;
      }
      else
      {
         Dyac_throw_def(ConstraintViolation, "vertex array numElements == 0.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "invalid _vertArray object.");
   }
   return 0;
}

sBool CpPolyShape::_initPoly(CpBody *_body, YAC_Object *_vertArray, YAC_Object *_transformOrVect, sF32 _radius) {
   _free();

   body = _body;

   cpTransform transform;

   if(YAC_VALID(_transformOrVect))
   {
      if(YAC_BCHK(_transformOrVect, clid_CpTransform))
      {
         // chipmunk7
         YAC_CAST_ARG(CpTransform, yCpTransform, _transformOrVect);
         transform = yCpTransform->cp_transform;
      }
      else if(YAC_BCHK(_transformOrVect, clid_CpVect))
      {
         // was 'offset' cpVect in chipmunk4
         YAC_CAST_ARG(CpVect, yCpVect, _transformOrVect);
         transform = cpTransformTranslate(yCpVect->cp_vect);
      }
      else
      {
         transform = cpTransformIdentity;
      }
   }
   else
   {
      transform = cpTransformIdentity;
   }
   
   sUI ne;
   cpVect *v;
   vertex_array = YAC_New_PointerArray();
   if(DecodeVertArray(_vertArray, &ne, &v, vertex_array, cpv(0, 0)/*offset*/))
   {
      cp_shape = cpPolyShapeNew(_body->cp_body, (sSI)ne, v, transform, _radius);
      cp_shape->userData = (void*)this;

      delete [] v;

      return 1;
   }
   else
   {
      yac_host->printf("[---] CpPolyShape::initPoly: DecodeVertArray failed.\n");
      return 0;
   }
}

void CpPolyShape::_New(CpBody *_body, YAC_Object *_vertArray, YAC_Object *_transformOrVect, sF32 _radius, YAC_Value *_r) {
   CpPolyShape *s = YAC_NEW(CpPolyShape);
   if(s->_initPoly(_body, _vertArray, _transformOrVect, _radius))
   {
      YAC_RETO(s, 1);
   }
   else
   {
      YAC_DELETE(s);
      YAC_RETO(0, 0);
   }
}

YAC_Object *CpPolyShape::_getVerts(void) {
   return vertex_array;
}


// ---------------------------------------------------------------------------- CpConstraint
static int oc_cpconstraint = 0;
CpConstraint::CpConstraint(void) {
   cp_constraint = NULL;
   body_a = NULL;
   body_b = NULL;
   anchor_a = NULL;
   anchor_b = NULL;
   oc_cpconstraint++;
}

CpConstraint::~CpConstraint() {
   _free();
   Dsafedelete(anchor_a);
   Dsafedelete(anchor_b);
   oc_cpconstraint--;
}

void CpConstraint::_free(void) {
   if(NULL != cp_constraint)
   {
      cpConstraintFree(cp_constraint);
      cp_constraint = NULL;
   }
}

void CpConstraint::initConstraint(CpBody *_a, CpBody *_b, CpVect *_anchorAOrNull, CpVect *_anchorBOrNull) {
   body_a = _a;
   body_b = _b;

   if(NULL != _anchorAOrNull)
   {
      if(NULL == anchor_a)
      {
         anchor_a = YAC_NEW(CpVect);
      }
      anchor_a->cp_vect = _anchorAOrNull->cp_vect;
   }

   if(NULL != _anchorBOrNull)
   {
      if(NULL == anchor_b)
      {
         anchor_b = YAC_NEW(CpVect);
      }
      anchor_b->cp_vect = _anchorBOrNull->cp_vect;
   }
}

CpBody *CpConstraint::getBodyA(void) {
   return body_a;
}

CpBody *CpConstraint::getBodyB(void) {
   return body_b;
}

CpVect *CpConstraint::getAnchorA(void) {
   return anchor_a;
}

CpVect *CpConstraint::getAnchorB(void) {
   return anchor_b;
}


// ---------------------------------------------------------------------------- CpJoint
CpJoint::CpJoint(void) {
}

CpJoint::~CpJoint(void) {
}


// ---------------------------------------------------------------------------- CpPinJoint
CpPinJoint::CpPinJoint(void) {
}

CpPinJoint::~CpPinJoint() {
}

void CpPinJoint::_free(void) {
   CpConstraint::_free();
}

void CpPinJoint::_initPin(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2) {
   _free();
   initConstraint(_a, _b, _anchr1, _anchr2);
   // yac_host->printf("xxx initPin a=%p b=%p b.class_ID=%u b.valid=%d\n", _a, _b, _b->class_ID, YAC_VALID(_b));
   // yac_host->printf("xxx initPin anchr1=(%f; %f) anchr2=(%f; %f)\n", _anchr1->cp_vect.x, _anchr1->cp_vect.y, _anchr2->cp_vect.x, _anchr2->cp_vect.y);
   cp_constraint = cpPinJointNew(_a->cp_body, _b->cp_body, _anchr1->cp_vect, _anchr2->cp_vect);
}

void CpPinJoint::_New(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, YAC_Value *_r) {

   // a and b are the two bodies to connect, and anchr1 and anchr2 are the anchor points on those bodies.
   CpPinJoint *j = YAC_NEW(CpPinJoint);
   j->_initPin(_a, _b, _anchr1, _anchr2);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpSlideJoint
CpSlideJoint::CpSlideJoint(void) {
}

CpSlideJoint::~CpSlideJoint() {
   _free();
}

void CpSlideJoint::_free(void) {
   CpConstraint::_free();
}

void CpSlideJoint::_initSlide(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _min, sF32 _max) {
   _free();
   initConstraint(_a, _b, _anchr1, _anchr2);
   cp_constraint = cpSlideJointNew(_a->cp_body, _b->cp_body, _anchr1->cp_vect, _anchr2->cp_vect, _min, _max);
}

void CpSlideJoint::_New(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _min, sF32 _max, YAC_Value *_r) {
   // a and b are the two bodies to connect, anchr1 and anchr2 are the anchor points on those bodies, and 
   // min and max define the allowed distances of the anchor points.
   CpSlideJoint *j = YAC_NEW(CpSlideJoint);
   j->_initSlide(_a, _b, _anchr1, _anchr2, _min, _max);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpPivotJoint
CpPivotJoint::CpPivotJoint(void) {
}

CpPivotJoint::~CpPivotJoint() {
   _free();
}

void CpPivotJoint::_free(void) {
   CpConstraint::_free();
}

void CpPivotJoint::_initPivot(CpBody *_a, CpBody *_b, CpVect *_pivot) {
   _free();
   initConstraint(_a, _b, NULL, NULL);
   cp_constraint = cpPivotJointNew(_a->cp_body, _b->cp_body, _pivot->cp_vect);
}

void CpPivotJoint::_initPivot2(CpBody *_a, CpBody *_b, CpVect *_anchorA, CpVect *_anchorB) {
   _free();
   cp_constraint = cpPivotJointNew2(_a->cp_body, _b->cp_body, _anchorA->cp_vect, _anchorB->cp_vect);
}

void CpPivotJoint::_New(CpBody *_a, CpBody *_b, CpVect *_pivot, YAC_Value *_r) {
   // a and b are the two bodies to connect, and pivot is the point in world coordinates of the pivot. 
   // Because the pivot location is given in world coordinates, you must have the bodies moved into the 
   // correct positions already.
   CpPivotJoint *j = YAC_NEW(CpPivotJoint);
   j->_initPivot(_a, _b, _pivot);
   YAC_RETO(j, 1);
}

void CpPivotJoint::_New2(CpBody *_a, CpBody *_b, CpVect *_anchorA, CpVect *_anchorB, YAC_Value *_r) {
   CpPivotJoint *j = YAC_NEW(CpPivotJoint);
   j->_initPivot2(_a, _b, _anchorA, _anchorB);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpGrooveJoint
CpGrooveJoint::CpGrooveJoint(void) {
}

CpGrooveJoint::~CpGrooveJoint() {
   _free();
}

void CpGrooveJoint::_free(void) {
   CpConstraint::_free();
}

void CpGrooveJoint::_initGroove(CpBody *_a, CpBody *_b, CpVect *_grooveA, CpVect *_grooveB, CpVect *_anchr2) {
   _free();
   initConstraint(_a, _b, _grooveA, _grooveB);
   cp_constraint = cpGrooveJointNew(_a->cp_body, _b->cp_body, _grooveA->cp_vect, _grooveB->cp_vect, _anchr2->cp_vect);
}

void CpGrooveJoint::_New(CpBody *_a, CpBody *_b, CpVect *_grooveA, CpVect *_grooveB, CpVect *_anchr2, YAC_Value *_r) {
   // The groove goes from groov_a to groove_b on body a_, and the pivot is attached to _anchr2 on body b.
   // All coordinates are body local.
   CpGrooveJoint *j = YAC_NEW(CpGrooveJoint);
   j->_initGroove(_a, _b, _grooveA, _grooveB, _anchr2);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpDampedSpring
CpDampedSpring::CpDampedSpring(void) {
}

CpDampedSpring::~CpDampedSpring() {
   _free();
}

void CpDampedSpring::_free(void) {
   CpConstraint::_free();
}

void CpDampedSpring::_initSpring(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _restLen, sF32 _stiffness, sF32 _damping) {
   _free();
   initConstraint(_a, _b, _anchr1, _anchr2);
   cp_constraint = cpDampedSpringNew(_a->cp_body, _b->cp_body, _anchr1->cp_vect, _anchr2->cp_vect, _restLen, _stiffness, _damping);
}

void CpDampedSpring::_New(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _restLen, sF32 _stiffness, sF32 _damping, YAC_Value *_r) {
   CpDampedSpring *j = YAC_NEW(CpDampedSpring);
   j->_initSpring(_a, _b, _anchr1, _anchr2, _restLen, _stiffness, _damping);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpGearJoint
CpGearJoint::CpGearJoint(void) {
}

CpGearJoint::~CpGearJoint() {
   _free();
}

void CpGearJoint::_free(void) {
   CpConstraint::_free();
}

void CpGearJoint::_initGear(CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratio) {
   _free();
   initConstraint(_a, _b, NULL, NULL);
   cp_constraint = cpGearJointNew(_a->cp_body, _b->cp_body, _phase, _ratio);
}

void CpGearJoint::_New(CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratio, YAC_Value *_r) {
   CpGearJoint *j = YAC_NEW(CpGearJoint);
   j->_initGear(_a, _b, _phase, _ratio);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpRotaryLimitJoint
CpRotaryLimitJoint::CpRotaryLimitJoint(void) {
}

CpRotaryLimitJoint::~CpRotaryLimitJoint() {
   _free();
}

void CpRotaryLimitJoint::_free(void) {
   CpConstraint::_free();
}

void CpRotaryLimitJoint::_initRotaryLimit(CpBody *_a, CpBody *_b, sF32 _min, sF32 _max) {
   _free();
   initConstraint(_a, _b, NULL, NULL);
   cp_constraint = cpRotaryLimitJointNew(_a->cp_body, _b->cp_body, _min, _max);
}

void CpRotaryLimitJoint::_New(CpBody *_a, CpBody *_b, sF32 _min, sF32 _max, YAC_Value *_r) {
   CpRotaryLimitJoint *j = YAC_NEW(CpRotaryLimitJoint);
   j->_initRotaryLimit(_a, _b, _min, _max);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- CpRatchetJoint
CpRatchetJoint::CpRatchetJoint(void) {
}

CpRatchetJoint::~CpRatchetJoint() {
   _free();
}

void CpRatchetJoint::_free(void) {
   CpConstraint::_free();
}

void CpRatchetJoint::_initRatchet(CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratchet) {
   _free();
   initConstraint(_a, _b, NULL, NULL);
   cp_constraint = cpRatchetJointNew(_a->cp_body, _b->cp_body, _phase, _ratchet);
}

void CpRatchetJoint::_New(CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratchet, YAC_Value *_r) {
   CpRatchetJoint *j = YAC_NEW(CpRatchetJoint);
   j->_initRatchet(_a, _b, _phase, _ratchet);
   YAC_RETO(j, 1);
}


// ---------------------------------------------------------------------------- fxns
void YAC_CALL _cpv(sF32 _x, sF32 _y, YAC_Value *_r) {
   CpVect *v = YAC_NEW_POOLED(CpVect);
   v->cp_vect.x = _x;
   v->cp_vect.y = _y;
   // yac_host->printf("xxx alloc cpv() v=%p\n", v);
   YAC_RETO(v, 1);
}

void YAC_CALL _cpvpin(sF32 _x, sF32 _y, YAC_Value *_r) {
   CpVect *v = YAC_NEW_POOLED(CpVect);
   v->cp_vect.x = _x;
   v->cp_vect.y = _y;
   YAC_RETO(v, 1);
}

void YAC_CALL _cpvadd_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvadd(_v1->cp_vect, _v2->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvadd_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r) {
   _r->cp_vect = cpvadd(_v1->cp_vect, _v2->cp_vect);
}

void YAC_CALL _cpvsub_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvsub(_v1->cp_vect, _v2->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvsub_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r) {
   _r->cp_vect = cpvsub(_v1->cp_vect, _v2->cp_vect);
}

void YAC_CALL _cpvmult_YAC_RVAL(CpVect *_v1, sF32 _s, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvmult(_v1->cp_vect, _s);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvmult_YAC_RARG(CpVect *_v1, sF32 _s, CpVect *_r) {
   _r->cp_vect = cpvmult(_v1->cp_vect, _s);
}

void YAC_CALL _cpvproject_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvproject(_v1->cp_vect, _v2->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvproject_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r) {
   _r->cp_vect = cpvproject(_v1->cp_vect, _v2->cp_vect);
}

void YAC_CALL _cpvrotate_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvrotate(_v1->cp_vect, _v2->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvrotate_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r) {
   _r->cp_vect = cpvrotate(_v1->cp_vect, _v2->cp_vect);
}

void YAC_CALL _cpvunrotate_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvunrotate(_v1->cp_vect, _v2->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvunrotate_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r) {
   _r->cp_vect = cpvunrotate(_v1->cp_vect, _v2->cp_vect);
}

void YAC_CALL _cpvneg_YAC_RVAL(CpVect *_v1, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvneg(_v1->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvneg_YAC_RARG(CpVect *_v1, CpVect *_r) {
   _r->cp_vect = cpvneg(_v1->cp_vect);
}

void YAC_CALL _cpvperp_YAC_RVAL(CpVect *_v1, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvperp(_v1->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvperp_YAC_RARG(CpVect *_v1, CpVect *_r) {
   _r->cp_vect = cpvperp(_v1->cp_vect);
}

void YAC_CALL _cpvnormalize_YAC_RVAL(CpVect *_v1, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvnormalize(_v1->cp_vect);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvnormalize_YAC_RARG(CpVect *_v1, CpVect *_r) {
   _r->cp_vect = cpvnormalize(_v1->cp_vect);
}

void YAC_CALL _cpvforangle_YAC_RVAL(sF32 _a, YAC_Value *_r) {
   CpVect *r = YAC_NEW_POOLED(CpVect);
   r->cp_vect = cpvforangle(_a);
   YAC_RETO(r, 1);
}

void YAC_CALL _cpvforangle_YAC_RARG(sF32 _a, CpVect *_r) {
   _r->cp_vect = cpvforangle(_a);
}

sF32 YAC_CALL _cpvtoangle(CpVect *_v1) {
   return cpvtoangle(_v1->cp_vect);
}

sF32 YAC_CALL _cpvdot(CpVect *_v1, CpVect *_v2) {
   return cpvdot(_v1->cp_vect, _v2->cp_vect);
}

sF32 YAC_CALL _cpvcross(CpVect *_v1, CpVect *_v2) {
   return cpvcross(_v1->cp_vect, _v2->cp_vect);
}

sF32 YAC_CALL _cpvlength(CpVect *_v1) {
   return cpvlength(_v1->cp_vect);
}

sF32 YAC_CALL _cpvlengthsq(CpVect *_v1) {
   return cpvlengthsq(_v1->cp_vect);
}

sF32 YAC_CALL _cpMomentForCircle(sF32 _m, sF32 _r1, sF32 _r2, CpVect *_offset) {
   return cpMomentForCircle(_m, _r1, _r2, _offset->cp_vect);
}

sF32 YAC_CALL _cpMomentForPoly(sF32 _m, YAC_Object *_vertArray, CpVect *_offset, sF32 _radius) {
   sUI ne;
   cpVect *v;
   if(DecodeVertArray(_vertArray, &ne, &v, NULL, _offset->cp_vect))
   {
      sF32 r = cpMomentForPoly(_m, (sSI)ne, v, _offset->cp_vect, _radius);
      delete [] v;
      return r;
   }
   else
   {
      // Failed to decode vertex array
      return 0.0f;
   }
}

YF void YAC_CALL _cpSpaceGetStaticBody(CpSpace *_space, YAC_Value *_r) {
   CpBody *b = YAC_NEW(CpBody);
   b->freeBody();
   b->cp_body = cpSpaceGetStaticBody(_space->cp_space);
   b->cp_body->userData = (void*)b;
   YAC_RETO(b, 1);
}

void YAC_CALL YAC_Init(YAC_Host *_host) {
   yac_host = _host;

   // // cpInitChipmunk();

   YAC_Init_cp(_host);

   // Setup class inheritance
   yac_host->cpp_typecast_map[clid_CpCircleShape]      [clid_CpShape] = 1;  
   yac_host->cpp_typecast_map[clid_CpSegmentShape]     [clid_CpShape] = 1;  
   yac_host->cpp_typecast_map[clid_CpPolyShape]        [clid_CpShape] = 1;  
                                                       
   yac_host->cpp_typecast_map[clid_CpJoint]            [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpPinJoint]         [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpPinJoint]         [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpSlideJoint]       [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpSlideJoint]       [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpPivotJoint]       [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpPivotJoint]       [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpGrooveJoint]      [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpGrooveJoint]      [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpDampedSpring]     [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpDampedSpring]     [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpGearJoint]        [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpGearJoint]        [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpRotaryLimitJoint] [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpRotaryLimitJoint] [clid_CpConstraint] = 1;
   yac_host->cpp_typecast_map[clid_CpRatchetJoint]     [clid_CpJoint] = 1;
   yac_host->cpp_typecast_map[clid_CpRatchetJoint]     [clid_CpConstraint] = 1;

   // yac_host->printf("xxx clid_CpVect=%d\n", clid_CpVect);
   // yac_host->printf("xxx clid_CpCircleShape=%d\n", clid_CpCircleShape);
   // yac_host->printf("xxx clid_CpSegmentShape=%d\n", clid_CpSegmentShape);

   // Resolve "standard" exception ids
   Dyac_std_exid_resolve;

}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   if(yac_host->yacGetDebugLevel() > 0)
   {
      yac_host->printf("[...] tkchipmunk::YAC_Exit() beg.\n");
   }
   
   YAC_Exit_cp(_host);
   
   if(yac_host->yacGetDebugLevel() > 0)
   {
      yac_host->printf("[...] tkchipmunk::YAC_Exit() end.\n");
   }

   //printf("xxx oc_cpvect=%d\n", oc_cpvect);
   //printf("xxx oc_cpbody=%d\n", oc_cpbody);
   //printf("xxx oc_cpshape=%d\n", oc_cpshape);
   //printf("xxx oc_cpconstraint=%d\n", oc_cpconstraint);

}

