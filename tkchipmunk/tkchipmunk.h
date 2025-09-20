/// tkchipmunk.h
///
/// (c) 2007 Scott Lembcke <lemb0029(at)morris(dot)umn(dot)edu or slembcke(at)gmail(dot)com>
/// (c) 2008-2021 Bastian Spiegel <bs@tkscript.de>
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
/// created: 06May2008
/// changed: 07May2008, 08May2008, 09May2008, 12Jun2018, 13Jun2018, 14Jun2018, 19Jul2018
///          19Sep2021, 22Sep2021, 24Sep2021
///
///
///

#ifndef __TKCHIPMUNK_H__
#define __TKCHIPMUNK_H__

YG("cp");

Dyac_std_exid_decl;

class CpSpace;
class CpVect;
class CpBB;
class CpShape;
class CpBody;
class CpConstraint;
class CpArbiter;

// The YAC_NEW macro expects exported classnames to start with '_'
#define _CpArbiter CpArbiter
#define _CpSpace CpSpace
#define _CpVect CpVect
#define _CpBB CpBB
#define _CpTransform CpTransform
#define _CpShape CpShape
#define _CpCircleShape CpCircleShape
#define _CpSegmentShape CpSegmentShape
#define _CpPolyShape CpPolyShape
#define _CpBody CpBody
#define _CpConstraint CpConstraint
#define _CpJoint CpJoint
#define _CpPinJoint CpPinJoint
#define _CpSlideJoint CpSlideJoint
#define _CpPivotJoint CpPivotJoint
#define _CpGrooveJoint CpGrooveJoint


// ---------------------------------------------------------------------------- CpVect
YC class CpVect : public YAC_Object {
  public:
   cpVect cp_vect;

  public:
   CpVect(void);
   ~CpVect();
   
   YAC_POOLED(CpVect, YAC_POOL_PRIORITY_MEDIUM);

   sUI   YAC_VCALL yacArrayGetNumElements    (void);
   sUI   YAC_VCALL yacArrayGetMaxElements    (void);
   void  YAC_VCALL yacArraySet               (YAC_ContextHandle, sUI, YAC_Value *);
   void  YAC_VCALL yacArrayGet               (YAC_ContextHandle, sUI, YAC_Value *);

   void  YAC_VCALL yacOperatorAssign (YAC_Object *_o);
   sBool YAC_VCALL yacToString       (YAC_String *_r) const;

   YM sF32 _getX (void);
   YM void _setX (sF32 _x);

   YM sF32 _getY (void);
   YM void _setY (sF32 _y);

   YM void _getString (YAC_Value *_r);

   YM void _init (sF32 _x, sF32 _y);

   // Add two vectors
   YM void _add_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _add_YAC_RVAL (CpVect *_v, YAC_Value *_r);
   YM void _add_YAC_RSELF (CpVect *_v);

   // Subtract two vectors
   YM void _sub_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _sub_YAC_RVAL (CpVect *_v, YAC_Value *_r);
   YM void _sub_YAC_RSELF (CpVect *_v);

   // Negate a vector.
   YM void _neg_YAC_RARG (CpVect *_r);
   YM void _neg_YAC_RVAL (YAC_Value *_r);
   YM void _neg_YAC_RSELF (void);

   // Scalar multiplication.
   YM void _mult_YAC_RARG (sF32 _s, CpVect *_r);
   YM void _mult_YAC_RVAL (sF32 _s, YAC_Value *_r);
   YM void _mult_YAC_RSELF (sF32 _s);

   // Vector dot product.
   YM sF32 _dot (CpVect *_v);

   // 2D vector cross product analog.
   // The cross product of 2D vectors results in a 3D vector with only a z component.
   // This function returns the magnitude of the z value.
   YM sF32 _cross (CpVect *_v);

   // Returns a perpendicular vector. (90 degree rotation)
   YM void _perp_YAC_RARG (CpVect *_r);
   YM void _perp_YAC_RVAL (YAC_Value *_r);
   YM void _perp_YAC_RSELF (void);

   // Returns a perpendicular vector. (-90 degree rotation)
   YM void _rperp_YAC_RARG (CpVect *_r);
   YM void _rperp_YAC_RVAL (YAC_Value *_r);
   YM void _rperp_YAC_RSELF (void);

   // Returns the vector projection of this onto 'v'.
   YM void _project_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _project_YAC_RVAL (CpVect *_v, YAC_Value *_r);
   YM void _project_YAC_RSELF (CpVect *_v);

   /// Returns the unit length vector for the given angle (in radians).
   YM void _forAngle_YAC_RARG (sF32 _a, CpVect *_r);
   YM void _forAngle_YAC_RVAL (sF32 _a, YAC_Value *_r);
   YM void _forAngle_YAC_RSELF (sF32 _a);

   // Returns the angular direction v is pointing in (in radians).
   YM sF32 _toAngle (void);
   YM sF32 _getAngle (void);

   // Uses complex number multiplication to rotate v1 by v2. Scaling will occur if v1 is not a unit vector.
   YM void _rotate_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _rotate_YAC_RVAL (CpVect *_v, YAC_Value *_r);
   YM void _rotate_YAC_RSELF (CpVect *_v);

   // Inverse of rotate().
   YM void _unrotate_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _unrotate_YAC_RVAL (CpVect *_v, YAC_Value *_r);
   YM void _unrotate_YAC_RSELF (CpVect *_v);

   // Returns the length of v.
   YM sF32 _getLength (void);

   // Returns the squared length of v. Faster than getLength() when you only need to compare lengths.
   YM sF32 _getLengthSq (void);

   // Linearly interpolate between this and 'v'.
   YM void _lerp_YAC_RARG (CpVect *_o, sF32 _t, CpVect *_r);
   YM void _lerp_YAC_RVAL (CpVect *_o, sF32 _t, YAC_Value *_r);
   YM void _lerp_YAC_RSELF (CpVect *_o, sF32 _t);

   // Returns a normalized copy of v.
   YM void _normalize_YAC_RARG (CpVect *_r);
   YM void _normalize_YAC_RVAL (YAC_Value *_r);
   YM void _normalize_YAC_RSELF (void);

   // Spherical linearly interpolate between this and 'v'.
   YM void _slerp_YAC_RARG (CpVect *_o, sF32 _t, CpVect *_r);
   YM void _slerp_YAC_RVAL (CpVect *_o, sF32 _t, YAC_Value *_r);
   YM void _slerp_YAC_RSELF (CpVect *_o, sF32 _t);

   // Spherical linearly interpolate between this towards 'v' by no more than angle a radians
   YM void _slerpconst_YAC_RARG (CpVect *_o, sF32 _a, CpVect *_r);
   YM void _slerpconst_YAC_RVAL (CpVect *_o, sF32 _a, YAC_Value *_r);
   YM void _slerpconst_YAC_RSELF (CpVect *_o, sF32 _a);

   // Clamp to length len.
   YM void _clamp_YAC_RARG (sF32 _l, CpVect *_r);
   YM void _clamp_YAC_RVAL (sF32 _l, YAC_Value *_r);
   YM void _clamp_YAC_RSELF (sF32 _l);

   // Linearly interpolate between this towards 'v' by distance d.
   YM void _lerpconst_YAC_RARG (CpVect *_o, sF32 _d, CpVect *_r);
   YM void _lerpconst_YAC_RVAL (CpVect *_o, sF32 _d, YAC_Value *_r);
   YM void _lerpconst_YAC_RSELF (CpVect *_o, sF32 _d);

   // Returns the distance between this and 'v'.
   YM sF32 _dist (CpVect *_v);

   // Returns the squared distance between this and 'v'. Faster than dist() when you only need to compare distances.
   YM sF32 _distSq (CpVect *_v);

   // Returns true if the distance between this and 'v' is less than dist.
   YM sBool _near (CpVect *_v, sF32 _dist);

   // Add randomization to vector by +-'amt'
   YM void _rand_YAC_RARG (sF32 _amt, CpVect *_r);
   YM void _rand_YAC_RVAL (sF32 _amt, YAC_Value *_r);
   YM void _rand_YAC_RSELF (sF32 _amt);

   // Add randomization to vector by +-'amt' and normalize
   YM void _randNormalize_YAC_RARG (sF32 _amt, CpVect *_r);
   YM void _randNormalize_YAC_RVAL (sF32 _amt, YAC_Value *_r);
   YM void _randNormalize_YAC_RSELF (sF32 _amt);
};


// ---------------------------------------------------------------------------- CpBB
YC class CpBB : public YAC_Object {
  public:
   cpBB cp_bb;

  public:
   CpBB(void);
   ~CpBB();
   
   YAC_POOLED(CpBB, YAC_POOL_PRIORITY_MEDIUM);

   void  YAC_VCALL yacOperatorAssign (YAC_Object *_o);
   sBool YAC_VCALL yacToString       (YAC_String *_r) const;

   // Convenience constructor for cpBB structs.
   YM void _New  (sF32 _left, sF32 _bottom, sF32 _right, sF32 _top, YAC_Value *_r);

   // Constructs a cpBB centered on a point with the given extents (half sizes).
   YM void _NewForExtents (CpVect *_v, sF32 _hw, sF32 _hh, YAC_Value *_r);

   // Constructs a cpBB for a circle with the given position and radius.
   YM void _NewForCircle (CpVect *_v, sF32 _radius, YAC_Value *_r);

   YM sF32 _getL (void);
   YM void _setL (sF32 _l);

   YM sF32 _getB (void);
   YM void _setB (sF32 _b);

   YM sF32 _getR (void);
   YM void _setR (sF32 _r);

   YM sF32 _getT (void);
   YM void _setT (sF32 _t);

   YM void _getString (YAC_Value *_r);

   YM void _init (sF32 _l, sF32 _b, sF32 _r, sF32 _t);

   // Returns true if this and 'b' intersect.
   YM sBool _intersects (CpBB *_b);

   // Returns true 'b' lies completely within this.
   YM sBool _contains (CpBB *_b);

   // Returns true if this contains 'v'.
   YM sBool _containsVect (CpVect *_v);

   // Returns a bounding box that holds both bounding boxes.
   YM void _merge (CpBB *_b, YAC_Value *_r);

   // Returns a bounding box that holds both this and 'v'.
   YM void _expand_YAC_RARG (CpVect *_v, CpBB *_r);
   YM void _expand_YAC_RVAL (CpVect *_v, YAC_Value *_r);
   YM void _expand_YAC_RSELF (CpVect *_v);

   // Returns the center vector of the bounding box.
   YM void _getCenter_YAC_RARG (CpVect *_r);
   YM void _getCenter_YAC_RVAL (YAC_Value *_r);

   // Returns the area of the bounding box.
   YM sF32 _getArea (void);

   // Merges this and @c b and returns the area of the merged bounding box.
   YM sF32 _getMergedArea (CpBB *_b);

   // Returns the fraction along the segment query the cpBB is hit. Returns INFINITY if it doesn't hit.
   YM sF32 _segmentQuery (CpVect *_a, CpVect *_b);

   // Return true if the bounding box intersects the line segment with ends @c a and @c b.
   YM sBool _intersectsSegment (CpVect *_a, CpVect *_b);

   // Clamp a vector to the bounding box (and return new vector).
   YM void _clampVect_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _clampVect_YAC_RVAL (CpVect *_v, YAC_Value *_r);

   // Wrap a vector to a bounding box (and return new vector).
   YM void _wrapVect_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _wrapVect_YAC_RVAL (CpVect *_v, YAC_Value *_r);

   // Returns a bounding box offseted by @c v (and return new bb).
   YM void _offset_YAC_RARG (CpVect *_v, CpBB *_r);
   YM void _offset_YAC_RVAL (CpVect *_v, YAC_Value *_r);

   // Returns a bounding box offseted by @c -v (and return new bb).
   YM void _roffset_YAC_RARG (CpVect *_v, CpBB *_r);
   YM void _roffset_YAC_RVAL (CpVect *_v, YAC_Value *_r);

};


// ---------------------------------------------------------------------------- CpTransform
YC class CpTransform : public YAC_Object {
  public:
   cpTransform cp_transform;

  public:
   CpTransform(void);
   ~CpTransform();
   
   YAC_POOLED(CpTransform, YAC_POOL_PRIORITY_MEDIUM);

   void  YAC_VCALL yacOperatorAssign(YAC_Object *_o);
   void  YAC_VCALL yacArrayGet      (void *_context, sUI _index, YAC_Value *_value);
   sBool YAC_VCALL yacToString      (YAC_String *_r) const;

   YM sF32 _getA (void);
   YM void _setA (sF32 _a);

   YM sF32 _getB (void);
   YM void _setB (sF32 _b);

   YM sF32 _getC (void);
   YM void _setC (sF32 _c);

   YM sF32 _getD (void);
   YM void _setD (sF32 _d);

   YM sF32 _getTx (void);
   YM void _setTx (sF32 _tx);

   YM sF32 _getTy (void);
   YM void _setTy (sF32 _ty);

   YM void _getString (YAC_Value *_r);

   YM void _init (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _tx, sF32 _ty);
   YM void _initIdentity (void);
};


// ---------------------------------------------------------------------------- CpSpace
struct CpSpaceCollisionHandler {
   CpSpace *parent_space;
   cpCollisionHandler *cp_collision_handler;
   sUI collision_type_a;  // 0=any
   sUI collision_type_b;  // 0=any
   YAC_Object *fun_begin;
   YAC_Object *fun_pre_solve;
   YAC_Object *fun_post_solve;
   YAC_Object *fun_separate;
   YAC_Object *user_data;
};

YC class CpArbiter : public YAC_Object {
   // (note) object is re-used during collision detection (current_arbiter is set dynamically)
  public:
   CpSpace *parent_space;
   cpArbiter *current_arbiter;

  public:
   CpArbiter(void);
   ~CpArbiter();

   YAC(CpArbiter);

   CpVect *cpv_surface_velocity;
   CpVect *cpv_total_impulse;
   CpVect *cpv_normal;
   CpVect *cpv_point_a;
   CpVect *cpv_point_b;

  public:

   // Return the colliding shapes involved for this arbiter.
   // The order of their cpSpace.collision_type values will match
   // the order set when the collision handler was registered.
   YM YAC_Object *_getShapeA (void);
   YM YAC_Object *_getShapeB (void);

   // Return the colliding bodies involved for this arbiter.
   // The order of the cpSpace.collision_type the bodies are associated with values will match
   // the order set when the collision handler was registered.
   YM YAC_Object *_getBodyA (void);
   YM YAC_Object *_getBodyB (void);

   // Get the restitution (elasticity) that will be applied to the pair of colliding objects.
   YM sF32 _getRestitution (void);

   // Override the restitution (elasticity) that will be applied to the pair of colliding objects.
   YM void _setRestitution (sF32 _restitution);

   // Get the friction coefficient that will be applied to the pair of colliding objects.
   YM float _getFriction (void);

   // Override the friction coefficient that will be applied to the pair of colliding objects.
   YM void _setFriction (sF32 _friction);

   // Get the relative surface velocity of the two shapes in contact.
   YM YAC_Object *_getSurfaceVelocity (void);

   // Override the relative surface velocity of the two shapes in contact.
   // By default this is calculated to be the difference of the two surface velocities clamped to the tangent plane.
   YM void _setSurfaceVelocity (YAC_Object *_vr);

   // Calculate the total impulse including the friction that was applied by this arbiter.
   // This function should only be called from a post-solve, post-step or cpBodyEachArbiter callback.
   YM YAC_Object *_getTotalImpulse (void);

   // Calculate the amount of energy lost in a collision including static, but not dynamic friction.
   // This function should only be called from a post-solve, post-step or cpBodyEachArbiter callback.
   YM sF32 _getTotalKE (void);

   // Mark a collision pair to be ignored until the two objects separate.
   // Pre-solve and post-solve callbacks will not be called, but the separate callback will be called.
   YM void _ignore (void);

   // Returns true if this is the first step a pair of objects started colliding.
   YM sBool _isFirstContact (void);

   // Returns true if the separate callback is due to a shape being removed from the space.
   YM sBool _isRemoval (void);

   // Get the number of contact points for this arbiter.
   YM sSI _getCount (void);

   // Get the normal of the collision.
   YM YAC_Object *_getNormal (void);

   // Get the position of the @c first contact point on the surface of the first shape.
   YM YAC_Object *_getPointA (void);

   // Get the position of the @c ith contact point on the surface of the first shape.
   YM YAC_Object *_getPointAByIndex (sSI _i);

   // Get the position of the @c first contact point on the surface of the second shape.
   YM YAC_Object *_getPointB (void);

   // Get the position of the @c ith contact point on the surface of the second shape.
   YM YAC_Object *_getPointBByIndex (sSI _i);

   // Get the depth of the @c first contact point.
   YM sF32 _getDepth (void);

   // Get the depth of the @c ith contact point.
   YM sF32 _getDepthByIndex (sSI _i);
};

YC class CpSpace : public YAC_Object {
#define CPSPACE_MAX_COLLISION_HANDLERS YCI 256
   //
   // spaces: 
   //   Spaces are the basic simulation unit in Chipmunk. 
   //   You add bodies, shapes and constraints to a space, and then update the space as a whole.
   //
  public:
   cpSpace *cp_space;

   CpVect *vec_gravity;

   YAC_PointerArray *shapes;
   YAC_PointerArray *bodies;
   YAC_PointerArray *constraints;

   CpSpaceCollisionHandler collision_handlers[CPSPACE_MAX_COLLISION_HANDLERS];

   CpArbiter *arbiter;
   YAC_PointerArray *shape_remove_queue;
   YAC_PointerArray *body_remove_queue;

   // temporary while running bbQuery()
   YAC_Object *bb_query_fun;
   YAC_Object *bb_query_user_data;

  public:
   CpSpace(void);
   ~CpSpace();

   YAC(CpSpace);

   YM void _New(YAC_Value *_r);

   YM void _free(void);
   YM void _init(void);

   // The amount of gravity applied to the system.
   YM YAC_Object *_getGravity(void);
   YM void        _setGravity(CpVect *_gravity);
   YM void        _setGravity2f(sF32 _x, sF32 _y);

   // The amount of viscous damping applied to the system.
   YM sF32 _getDamping(void);
   YM void _setDamping(sF32 _damping);

   // The number of iterations to use when solving constraints (collisions and joints). 
   // Defaults to 10.
   YM sSI  _getIterations(void);
   YM void _setIterations(sSI _iterations);

   /// Speed threshold for a body to be considered idle.
   /// The default value of 0 means to let the space guess a good threshold based on gravity.
   YM sF32 _getIdleSpeedThreshold (void);
   YM void _setIdleSpeedThreshold (sF32 _threshold);

   // Time a group of bodies must remain idle in order to fall asleep.
   // Enabling sleeping also implicitly enables the the contact graph.
   // The default value of INFINITY disables the sleeping algorithm.
   YM sF32 _getSleepTimeThreshold (void);
   YM void _setSleepTimeThreshold (sF32 _sleepTimeThreshold);

   // Amount of encouraged penetration between colliding shapes.
   // Used to reduce oscillating contacts and keep the collision cache warm.
   // Defaults to 0.1. If you have poor simulation quality,
   // increase this number as much as possible without allowing visible amounts of overlap.
   YM sF32 _getCollisionSlop (void);
   YM void _setCollisionSlop (sF32 _collisionSlop);

   // Determines how fast overlapping shapes are pushed apart.
   // Expressed as a fraction of the error remaining after each second.
   // Defaults to pow(1.0 - 0.1, 60.0) meaning that Chipmunk fixes 10% of overlap each frame at 60Hz.
   YM sF32 _getCollisionBias (void);
   YM void _setCollisionBias (sF32 _collisionBias);

   // Number of frames that contact information should persist.
   // Defaults to 3. There is probably never a reason to change this value.
   YM sSI  _getCollisionPersistence (void);
   YM void _setCollisionPersistence (sSI _collisionPersistence);

   // These functions add and remove shapes, bodies and joints from space. 
   // Shapes added as static are assumed not to move. 
   // Static shapes should be be attached to a rigid body with an infinite mass and moment of inertia. 
   // Also, don’t add the rigid body used to the space, as that will cause it to fall under the effects of gravity.
   YM void _addShape       (YAC_Object *_shape);
   YM void _addBody        (YAC_Object *_body);
   YM void _addConstraint  (YAC_Object *_constraint);
   // tkchipmunk4 backwards compatibility (redirects to addConstraint)
   YM void _addJoint       (YAC_Object *_joint);

   YM void _removeShape       (CpShape *_shape);
   YM void _queueRemoveShapeAndBody (CpShape *_shape);  // to be used in collision callback, remove after step()

   YM void _removeBody        (CpBody *_body);
   YM void _removeConstraint  (CpConstraint *_constraint);

   YM void _freeChildren   (void);

   YM YAC_Object *_getShapes      (void);
   YM YAC_Object *_getBodies      (void);
   YM YAC_Object *_getConstraints (void);

   // Install collision handlers. Collision type 0 matches all types.
   //  - use typeA=0 and typeB=0 to configure default handler
   //  - use typeA>0 and typeB=0 to configure wildcard handler
   //  - use typeA>0 and typeB>0 to configure handler for specific collision type combination
   //  - 'userData' is an arbitrary Object that is passed to the collision function callbacks
   //  - callbacks:
   //     onCollBegin    (CpArbiter _arb, CpSpace _space, Object _userData) : boolean
   //     onCollPreSolve (CpArbiter _arb, CpSpace _space, Object _userData) : boolean
   //     onCollPostSolve(CpArbiter _arb, CpSpace _space, Object _userData)
   //     onCollSeparate (CpArbiter _arb, CpSpace _space, Object _userData)
   YM sBool _addCollisionHandlers (sUI _collisionTypeA, sUI _collisionTypeB, YAC_Object *_funBeginOrNull, YAC_Object *_funPreSolveOrNull, YAC_Object *_funPostSolveOrNull, YAC_Object *_funSeparateOrNull, YAC_Object *_userData);

   YM void _bbQuery (CpBB *_bb, YAC_Object *_funMatch, YAC_Object *_userData);

   // Query the space at a point and return the nearest shape found. Returns NULL if no shapes were found.
   YM CpShape *_pointQueryNearest (CpVect *_point, sF32 _maxDistance);

   // Switch the space to use a spatial hash instead of the bounding box tree.
   YM void _useSpatialHash (sF32 _dim, sSI _count);
    
   // Update the collision detection info for the static shapes in the space.
   YM void _reindexStatic (void);

   // Update the space for the given time step. 
   // Using a fixed time step is highly recommended. 
   // Doing so will increase the efficiency of the contact persistence, requiring an order of magnitude fewer iterations 
   // to resolve the collisions in the usual case.
   YM void _step (sF32 _dt);

};


// ---------------------------------------------------------------------------- CpBody
YC class CpBody : public YAC_Object {
   //
   // rigid bodies: 
   //   A rigid body holds the physical properties of an object. (mass, position, rotation, velocity, etc.) 
   //   It does not have a shape by itself. 
   //   If you’ve done physics with particles before, rigid bodies differ mostly in that they are able to rotate.
   //


// Exported constants:
//#define INFINITY YCF INFINITY

	/// A dynamic body is one that is affected by gravity, forces, and collisions.
	/// This is the default body type.
	//#define CP_BODY_TYPE_DYNAMIC YCI CP_BODY_TYPE_DYNAMIC

	/// A kinematic body is an infinite mass, user controlled body that is not affected by gravity, forces or collisions.
	/// Instead the body only moves based on it's velocity.
	/// Dynamic bodies collide normally with kinematic bodies, though the kinematic body will be unaffected.
	/// Collisions between two kinematic bodies, or a kinematic body and a static body produce collision callbacks, but no collision response.
	//#define CP_BODY_TYPE_KINEMATIC YCI CP_BODY_TYPE_KINEMATIC

	/// A static body is a body that never (or rarely) moves. If you move a static body, you must call one of the cpSpaceReindex*() functions.
	/// Chipmunk uses this information to optimize the collision detection.
	/// Static bodies do not produce collision callbacks when colliding with other static bodies.
	//#define CP_BODY_TYPE_STATIC YCI CP_BODY_TYPE_STATIC

  public:
   cpBody *cp_body; // (note) userData is set to "this"
   sBool   b_delete_body;  // 0=static body
   
   CpVect *vec_p; // lazy alloc
   CpVect *vec_cog;
   CpVect *vec_v;
   CpVect *vec_w;
   CpVect *vec_f;
   CpVect *vec_rot;

   YAC_String *id;

  public:
   CpBody(void);
   ~CpBody();

   YAC(CpBody);

  public:
   void freeBody (void);

  public:

   YM void _init (sF32 _mass, sF32 _inertia);
   YM void _New  (sF32 _mass, sF32 _inertia, YAC_Value *_r);

   // Wake up a sleeping or idle body.
   YM void _activate (void);

   // Wake up any sleeping or idle bodies touching a static body.
   YM void _activateStatic (CpShape *_shape);

   // Force a body to fall asleep immediately.
   YM void _sleep (void);

   // Force a body to fall asleep immediately along with other bodies in a group.
   YM void _sleepWithGroup (CpBody *_group);

   // Returns true if the body is sleeping.
   YM sBool _isSleeping (void);

   // Get the type of the body.
   YM sSI _getType (void);

   // Set the type of the body.
   YM void _setType (sSI _bodyType);

   // Get the position of a body.
   YM YAC_Object *_getPosition (void);
   YM YAC_Object *_getP (void);

   // Set the position of a body.
   YM void        _setPosition (CpVect *_p);
   YM void        _setP (CpVect *_p);

   // Get the offset of the center of gravity in body local coordinates.
   YM YAC_Object *_getCenterOfGravity (void);
   YM YAC_Object *_getCog             (void);

   // Set the offset of the center of gravity in body local coordinates.
   YM void        _setCenterOfGravity (CpVect *_p);
   YM void        _setCog             (CpVect *_p);

   // Get the velocity of the body.
   YM YAC_Object *_getVelocity (void);
   YM YAC_Object *_getV        (void);

   // Set the velocity of the body.
   YM void        _setVelocity (CpVect *_v);
   YM void        _setV        (CpVect *_v);

   // Get the angular velocity of the body.
   YM sF32        _getAngularVelocity (void);
   YM sF32        _getW (void);

   // Set the angular velocity of the body.
   YM void        _setAngularVelocity (sF32 _w);
   YM void        _setW               (sF32 _w);

   // Get the force applied to the body for the next time step.
   YM YAC_Object *_getForce    (void);
   YM YAC_Object *_getF        (void);

   // Set the force applied to the body for the next time step.
   YM void        _setForce    (CpVect *_v);
   YM void        _setF        (CpVect *_v);

   // Get the rotation vector of the body. (The x basis vector of it's transform.)
   YM YAC_Object *_getRotation (void); // via cpBodyGetRotation()

   YM YAC_Object *_getRot      (void); // via transform field
   YM void        _setRot      (CpVect *_v);

   // Get the torque applied to the body for the next time step.
   YM sF32        _getTorque   (void);
   YM sF32        _getT        (void);

   // Set the torque applied to the body for the next time step.
   YM void        _setTorque   (sF32 _t);
   YM void        _setT        (sF32 _t);

   // Get the mass of the body.
   YM sF32        _getMass     (void);
   YM sF32        _getM        (void);

   // Set the mass of the body.
   YM void        _setMass     (sF32 _m);
   YM void        _setM        (sF32 _m);

   // Get the moment of inertia of the body.
   YM sF32        _getMoment   (void);
   YM sF32        _getI        (void);

   // Set the moment of inertia of the body.
   YM void        _setMoment   (sF32 _inertia);
   YM void        _setI        (sF32 _inertia);

   // Get the angle of the body.
   YM sF32        _getAngle    (void);
   YM sF32        _getA        (void);

   // Set the angle of the body.
   YM void        _setAngle    (sF32 _angle);
   YM void        _setA        (sF32 _angle);

   // Convert body relative/local coordinates to absolute/world coordinates.
   YM void _localToWorld_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _localToWorld_YAC_RVAL (CpVect *_v, YAC_Value *_r);

   // Convert body absolute/world coordinates to  relative/local coordinates.
   YM void _worldToLocal_YAC_RARG (CpVect *_v, CpVect *_r);
   YM void _worldToLocal_YAC_RVAL (CpVect *_v, YAC_Value *_r);

   // Apply an impulse to a body. Both the impulse and point are expressed in world coordinates.
   YM void _applyImpulseAtWorldPoint (CpVect *_impulse, CpVect *_point);
   // tkchipmunk4 backward compatibility:
   YM void _applyImpulse             (CpVect *_impulse, CpVect *_point);

   // Apply an impulse to a body. Both the impulse and point are expressed in body local coordinates.
   YM void _applyImpulseAtLocalPoint (CpVect *_impulse, CpVect *_point);

   // Apply a force to a body. Both the force and point are expressed in world coordinates.
   YM void _applyForceAtWorldPoint (CpVect *_force, CpVect *_point);
   // tkchipmunk4 backward compatibility:
   YM void _applyForce             (CpVect *_force, CpVect *_point);

   // Apply a force to a body. Both the force and point are expressed in body local coordinates.
   YM void _applyForceAtLocalPoint (CpVect *_force, CpVect *_point);

   // Get the velocity on a body (in world units) at a point on the body in world coordinates.
   YM void getVelocityAtWorldPoint (CpVect *_point, YAC_Value *_r);

   // Get the velocity on a body (in world units) at a point on the body in local coordinates.
   YM void getVelocityAtLocalPoint (CpVect *_point, YAC_Value *_r);

   // Updates the velocity of the body using Euler integration. 
   // You don’t need to call this unless you are managing the object manually instead of adding it to a cpSpace.
   YM void _updateVelocity (CpVect *_gravity, sF32 _damping, sF32 _dt);

   // Updates the position of the body using Euler integration. 
   // Like cpBodyUpdateVelocity() you shouldn’t normally need to call this yourself.
   YM void _updatePosition (sF32 _dt);

   // Get the amount of kinetic energy contained by the body.
   //  (note) 0.5*mass*velocity*velocity
   //  (note) hit damage is proportional to energy loss due to collisions
   YM sF32 _getKineticEnergy (void);

   YM void        _setId (YAC_Object *_s);
   YM YAC_Object *_getId (void);
};


// ---------------------------------------------------------------------------- CpShape
YCR class CpShape : public YAC_Object {
   //
   // collision shapes: 
   //   By attaching shapes to bodies, you can define the a body’s shape. 
   //   You can attach many shapes to a single body to define a complex shape, or none if it doesn’t require a shape.
   //

// Export shape type constants:
//#define CP_CIRCLE_SHAPE  YCI CP_CIRCLE_SHAPE
//#define CP_SEGMENT_SHAPE YCI CP_SEGMENT_SHAPE
//#define CP_POLY_SHAPE    YCI CP_POLY_SHAPE

  public:
   cpShape *cp_shape;  // (note) userData is set to "this"
   
   CpBody           *body;          // The rigid body the shape is attached to.
   CpVect           *vec_surface_v; // Surface velocity (lazy alloc)
   CpVect           *vec_cog;       // Center of gravity (lazy alloc)

   sSI render_style;
   YAC_String *id;
   YAC_Value user_data;

   sF32 display_rotation;

  public:
   CpShape(void);
   ~CpShape();

   YAC(CpShape);

   YM virtual void _free       (void);

   YM YAC_Object *_getBody (void);
   YM sSI         _getType (void);

   // Shapes tagged as sensors (cpShape.sensor == true) never generate collisions that get processed, so collisions between sensors shapes and other shapes will never call the postSolve() callback. They still generate begin(), and separate() callbacks, and the preSolve() callback is also called every frame even though there is no collision response
   YM sSI  _getSensor (void);
   YM void _setSensor (sBool _bEnable);

   // Get the mass of the shape if you are having Chipmunk calculate mass properties for you.
   YM sF32 _getMass (void);

   // Set the mass of this shape to have Chipmunk calculate mass properties for you.
   YM void _setMass(sF32 _mass);

   // Get the density of the shape if you are having Chipmunk calculate mass properties for you.
   YM sF32 _getDensity (void);

   // Set the density  of this shape to have Chipmunk calculate mass properties for you.
   YM void _setDensity (sF32 _density);

   // Get the calculated moment of inertia for this shape.
   YM sF32 _getMoment (void);

   // Get the calculated area of this shape.
   YM sF32 _getArea (void);

   // Get the centroid of this shape.
   YAC_Object *_getCenterOfGravity (void);
  
   // Shapes in the same non-zero group do not generate collisions. 
   // Useful when creating an object out of many shapes that you don’t want to self collide. 
   // Defaults to 0;
   YM sSI         _getFilterGroup (void);
   YM sSI         _getGroup (void);  // tkchipmunk4 backwards compatibility
   YM void        _setFilterGroup (sSI _group);
   YM void        _setGroup (sSI _group); // tkchipmunk4 backwards compatibility

	/// A bitmask of user definable categories that this object belongs to.
	/// The category/mask combinations of both objects in a collision must agree for a collision to occur.
   YM sSI         _getFilterCategories(void);
   YM void        _setFilterCategories(sSI _categories);

   // Shapes only collide if they are in the same bit-planes. i.e. (a->layers & b->layers) != 0 
   // By default, a shape occupies all 32 bit-planes.
   YM sSI         _getFilterMask(void);
   YM void        _setFilterMask(sSI _layers);

   //A user definable field, see the collision pair function section below for more information.
   YM sSI         _getCollisionType(void);
   YM void        _setCollisionType(sSI _collisionType);

   // The surface velocity of the object. 
   // Useful for creating conveyor belts or players that move around. 
   // This value is only used when calculating friction, not the collision.
   YM YAC_Object *_getSurfaceVelocity(void);
   YM void        _setSurfaceVelocity(CpVect *_v);
   YM void        _setSurfaceVelocity2f(sF32 _x, sF32 _y);

   // Elasticity of the shape. 
   // A value of 0.0 gives no bounce, while a value of 1.0 will give a "perfect" bounce. 
   // However due to inaccuracies in the simulation using 1.0 or greater is not recommended however
   YM sF32        _getElasticity (void);
   YM sF32        _getE          (void);
   YM void        _setElasticity (sF32 _e);
   YM void        _setE          (sF32 _e);

   // Friction coefficient. 
   // Chipmunk uses the Coulomb friction model, a value of 0.0 is frictionless. 
   // see <http://www.roymech.co.uk/Useful_Tables/Tribology/co_of_frict.htm> for a table of friction coefficients.
   YM sF32        _getFriction (void); // get friction coefficient, 0=frictionless, 
   YM sF32        _getU (void);
   YM void        _setFriction (sF32 _friction);
   YM void        _setU        (sF32 _friction);

   // Custom userdata:
   YM void        _setRenderStyle (sSI _type);
   YM sSI         _getRenderStyle (void);

   YM void        _setId (YAC_Object *_s);
   YM YAC_Object *_getId (void);

   YM void        _setUserData (YAC_Object *_v);
   YM void        _getUserData (YAC_Value *_r);

   YM sF32        _getDisplayRotation (void);
   YM void        _setDisplayRotation (sF32 _rot);
};


// ---------------------------------------------------------------------------- CpCircleShape
YC class CpCircleShape : public CpShape {
  public:
   CpVect *vec_c; // Center (lazy alloc)

  public:
   YAC(CpCircleShape);

   CpCircleShape(void);
   ~CpCircleShape();

   void _free(void);

   YM void _initCircle (CpBody *_body, sF32 _radius, CpVect *_offset);

   // body is the body to attach the circle to, offset is the offset from the body’s center of gravity in body local coordinates.
   YM void _New(CpBody *_body, sF32 _radius, CpVect *_offset, YAC_Value *_r);

   // Center. (body space coordinates)
   YM YAC_Object *_getC (void);
   YM void        _setC (CpVect *_c);

   // Radius
   YM sF32 _getR(void);
   YM void _setR(sF32 _r);
};


// ---------------------------------------------------------------------------- CpSegmentShape
YC class CpSegmentShape : public CpShape {
  public:
   CpVect *vec_a; // Endpoint a (lazy alloc)
   CpVect *vec_b; // Endpoint b (lazy alloc)
   CpVect *vec_n; // Normal n (lazy alloc)

  public:
   YAC(CpSegmentShape);

   CpSegmentShape(void);
   ~CpSegmentShape();

   void _free(void);

   YM void _initSegment(CpBody *_body, CpVect *_a, CpVect *_b, sF32 _radius);

   // body is the body to attach the segment to, a and b are the endpoints, and radius is the thickness of the segment.
   YM void _New(CpBody *_body, CpVect *_a, CpVect *_b, sF32 _radius, YAC_Value *_r); 

   // Endpoints and normal of the segment. (body space coordinates)
   YM YAC_Object *_getA (void);
   YM void        _setA (CpVect *_a);
   YM YAC_Object *_getB (void);
   YM void        _setB (CpVect *_b);
   YM YAC_Object *_getN (void);
   YM void        _setN (CpVect *_n);

   // Radius of the segment. (Thickness)
   YM sF32 _getR(void);
   YM void _setR(sF32 _r);
};


// ---------------------------------------------------------------------------- CpPolyShape
YC class CpPolyShape : public CpShape {
  public:
   YAC_PointerArray *vertex_array;  // Array of CpVect objects (created by _initPoly())

  public:
   YAC(CpPolyShape);

   CpPolyShape(void);
   ~CpPolyShape();

   void _free(void);

   YM YAC_Object *_getVerts(void);

   // (note) 'transform' was usually a cpvzero in chipmunk4, must use cpTransformIdentity in chipmunk7
   YM sBool _initPoly   (CpBody *_body, YAC_Object *_vertArray, YAC_Object *_transformOrVect, sF32 _radius);

   // body is the body to attach the poly to, verts is an array of cpVect’s defining a convex hull with a counterclockwise
   // winding, offset is the offset from the body’s center of gravity in body local coordinates.
   YM void _New(CpBody *_body, YAC_Object *_vertArray, YAC_Object *_transformOrVect, sF32 _radius, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpConstraint
YCR class CpConstraint : public YAC_Object {
   //
   // joints: 
   //   You can attach joints between two bodies to constrain their behavior.
   //

  public:
   cpConstraint *cp_constraint;
   CpBody *body_a;   // ref
   CpBody *body_b;;  // ref
   CpVect *anchor_a;
   CpVect *anchor_b;

  public:
   CpConstraint(void);
   ~CpConstraint();

   YAC(CpConstraint);
   
   YM virtual void _free (void);

   void initConstraint (CpBody *_a, CpBody *_b, CpVect *_anchorAOrNull, CpVect *_anchorBOrNull);

   YM CpBody *getBodyA (void);
   YM CpBody *getBodyB (void);
   YM CpVect *getAnchorA (void);
   YM CpVect *getAnchorB (void);
};


// ---------------------------------------------------------------------------- CpJoint
// for backwards compatibility with tkchipmunk4:
YCR class CpJoint : public CpConstraint {

  public:
   CpJoint(void);
   ~CpJoint();

   YAC(CpJoint);
  
};


// ---------------------------------------------------------------------------- CpPinJoint
YC class CpPinJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpPinJoint(void);
   ~CpPinJoint();

   YAC(CpPinJoint);
   
   void _free (void);

   YM void _initPin (CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2);

   // a and b are the two bodies to connect, and anchr1 and anchr2 are the anchor points on those bodies.
   YM void _New(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpSlideJoint
YC class CpSlideJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpSlideJoint(void);
   ~CpSlideJoint();

   YAC(CpSlideJoint);
   
   void _free (void);

   YM void _initSlide  (CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _min, sF32 _max);

   // a and b are the two bodies to connect, anchr1 and anchr2 are the anchor points on those bodies, and 
   // min and max define the allowed distances of the anchor points.
   YM void _New(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _min, sF32 _max, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpPivotJoint
YC class CpPivotJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpPivotJoint(void);
   ~CpPivotJoint();

   YAC(CpPivotJoint);
   
   void _free (void);

   YM void _initPivot  (CpBody *_a, CpBody *_b, CpVect *_pivot);
   YM void _initPivot2 (CpBody *_a, CpBody *_b, CpVect *_anchorA, CpVect *_anchorB);

   // a and b are the two bodies to connect, and pivot is the point in world coordinates of the pivot. 
   // Because the pivot location is given in world coordinates, you must have the bodies moved into the 
   // correct positions already.
   YM void _New(CpBody *_a, CpBody *_b, CpVect *_pivot, YAC_Value *_r);

   // Allocate and initialize a pivot joint with specific anchors.
   YM void _New2(CpBody *_a, CpBody *_b, CpVect *_anchorA, CpVect *_anchorB, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpGrooveJoint
YC class CpGrooveJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpGrooveJoint(void);
   ~CpGrooveJoint();

   YAC(CpGrooveJoint);
   
   void _free (void);

   YM void _initGroove (CpBody *_a, CpBody *_b, CpVect *_grooveA, CpVect *_grooveB, CpVect *_anchr2);

   // The groove goes from groov_a to groove_b on body a_, and the pivot is attached to _anchr2 on body b.
   // All coordinates are body local.
   YM void _New(CpBody *_a, CpBody *_b, CpVect *_grooveA, CpVect *_grooveB, CpVect *_anchr2, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpDampedSpring
YC class CpDampedSpring : public CpJoint/*CpConstraint*/ {

  public:
   CpDampedSpring(void);
   ~CpDampedSpring();

   YAC(CpDampedSpring);
   
   void _free (void);

   YM void _initSpring (CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _restLen, sF32 _stiffness, sF32 _damping);

   // a and b are the two bodies to connect, and anchr1 and anchr2 are the anchor points on those bodies.
   YM void _New(CpBody *_a, CpBody *_b, CpVect *_anchr1, CpVect *_anchr2, sF32 _restLen, sF32 _stiffness, sF32 _damping, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpGearJoint
YC class CpGearJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpGearJoint(void);
   ~CpGearJoint();

   YAC(CpGearJoint);
   
   void _free (void);

   YM void _initGear (CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratio);

   YM void _New(CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratio, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpRotaryLimitJoint
YC class CpRotaryLimitJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpRotaryLimitJoint(void);
   ~CpRotaryLimitJoint();

   YAC(CpRotaryLimitJoint);
   
   void _free (void);

   YM void _initRotaryLimit (CpBody *_a, CpBody *_b, sF32 _min, sF32 _max);

   YM void _New(CpBody *_a, CpBody *_b, sF32 _min, sF32 _max, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpRatchetJoint
YC class CpRatchetJoint : public CpJoint/*CpConstraint*/ {

  public:
   CpRatchetJoint(void);
   ~CpRatchetJoint();

   YAC(CpRatchetJoint);
   
   void _free (void);

   YM void _initRatchet (CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratchet);

   YM void _New(CpBody *_a, CpBody *_b, sF32 _phase, sF32 _ratchet, YAC_Value *_r);
};


// ---------------------------------------------------------------------------- CpVect
YF void YAC_CALL _cpv(sF32 _x, sF32 _y, YAC_Value *_r);
YF void YAC_CALL _cpvpin(sF32 _x, sF32 _y, YAC_Value *_r);

// Add or subtract two vectors.
YF void YAC_CALL _cpvadd_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r);
YF void YAC_CALL _cpvadd_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r);
YF void YAC_CALL _cpvsub_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r);
YF void YAC_CALL _cpvsub_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r);

// Scalar multiplication.
YF void YAC_CALL _cpvmult_YAC_RVAL(CpVect *_v1, sF32 _s, YAC_Value *_r);
YF void YAC_CALL _cpvmult_YAC_RARG(CpVect *_v1, sF32 _s, CpVect *_r);

// Returns the vector projection of v1 onto v2.
YF void YAC_CALL _cpvproject_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r);
YF void YAC_CALL _cpvproject_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r);

// Uses complex multiplication to rotate (and scale) v1 by v2.
YF void YAC_CALL _cpvrotate_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r);
YF void YAC_CALL _cpvrotate_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r);

// Inverse of cpvrotate().
YF void YAC_CALL _cpvunrotate_YAC_RVAL(CpVect *_v1, CpVect *_v2, YAC_Value *_r);
YF void YAC_CALL _cpvunrotate_YAC_RARG(CpVect *_v1, CpVect *_v2, CpVect *_r);

// Negate a vector.
YF void YAC_CALL _cpvneg_YAC_RVAL(CpVect *_v1, YAC_Value *_r);
YF void YAC_CALL _cpvneg_YAC_RARG(CpVect *_v1, CpVect *_r);

// Returns the perpendicular vector. (90 degree rotation)
YF void YAC_CALL _cpvperp_YAC_RVAL(CpVect *_v1, YAC_Value *_r);
YF void YAC_CALL _cpvperp_YAC_RARG(CpVect *_v1, CpVect *_r);

// Returns a normalized copy of v.
YF void YAC_CALL _cpvnormalize_YAC_RVAL(CpVect *_v1, YAC_Value *_r);
YF void YAC_CALL _cpvnormalize_YAC_RARG(CpVect *_v1, CpVect *_r);

// Returns the unit length vector for the given angle (in radians).
YF void YAC_CALL _cpvforangle_YAC_RVAL(sF32 _a, YAC_Value *_r);
YF void YAC_CALL _cpvforangle_YAC_RARG(sF32 _a, CpVect *_r);

// Returns the angular direction v is pointing in (in radians).
YF sF32 YAC_CALL _cpvtoangle(CpVect *_v1);

// Vector dot product.
YF sF32 YAC_CALL _cpvdot(CpVect *_v1, CpVect *_v2);

// 2D vector cross product analog. 
// The cross product of 2D vectors exists only in the z component, so only that value is returned.
YF sF32 YAC_CALL _cpvcross(CpVect *_v1, CpVect *_v2);

// Returns the length of v.
YF sF32 YAC_CALL _cpvlength(CpVect *_v1);

// Returns the squared length of v. 
// Faster than cpvlength() when you only need to compare lengths.
YF sF32 YAC_CALL _cpvlengthsq(CpVect *_v1);

// Calculate the moment of inertia for a circle. Arguments are similar to cpCircleShapeInit(). m_ is the mass, 
// _r1 and r2 define an inner and outer radius, and offset is the offset of the center from the center of gravity 
// of the rigid body.
YF sF32 YAC_CALL _cpMomentForCircle(sF32 _m, sF32 _r1, sF32 _r2, CpVect *_offset);

// Calculate the moment of inertia for a poly. Arguments are similar to cpPolyShapeInit() m_ is the mass, 
// _numVerts is the number of vertexes in verts, and offset is the offset of the poly coordinates from the 
// center of gravity of the rigid body.
YF sF32 YAC_CALL _cpMomentForPoly(sF32 _m, YAC_Object *_vertArray, CpVect *_offset, sF32 _radius);

YF void YAC_CALL _cpSpaceGetStaticBody(CpSpace *_space, YAC_Value *_r);


#endif // __TKCHIPMUNK_H__
