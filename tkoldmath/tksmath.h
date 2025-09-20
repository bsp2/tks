/// tksmath.h
/// author : (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///    published under terms of the GNU general public license (GPL).
///
#ifndef __TKSMATH_H__
#define __TKSMATH_H__

YG("OldMath");


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ Vector                                                         ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YC class _Vector : public sFVector  {
public:
	_Vector(void);
	~_Vector();
	YAC(_Vector);

	YAC_Object *YAC_VCALL yacArrayNew      (void);
	void        YAC_VCALL yacSerialize     (YAC_Object*, sUI);
	sUI         YAC_VCALL yacDeserialize   (YAC_Object*, sUI);
	void        YAC_VCALL yacOperatorClamp (YAC_Object*, YAC_Object*);
	void        YAC_VCALL yacOperatorWrap  (YAC_Object*, YAC_Object*);
	void        YAC_VCALL yacOperator      (sSI, YAC_Object *, YAC_Value*);
   sUI         YAC_VCALL yacArrayGetNumElements(void);
   sUI         YAC_VCALL yacArrayGetMaxElements(void);
   void        YAC_VCALL yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_value);
   void        YAC_VCALL yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_r);
   sUI         YAC_VCALL yacArrayGetElementType (void);
   sUI         YAC_VCALL yacArrayGetElementByteSize (void);

	YM sF32 _dot          (YAC_Object*);
	YM //void scalef       (sF32);
	YM //void unit         (void);
	YM //void neg          (void);
	YM //void init         (sF32,sF32,sF32);
	YM //void rotateXCW    (void);
	YM //void rotateXCCW   (void);
	YM //void rotateYCW    (void);
	YM //void rotateYCCW   (void);
	YM //void rotateZCW    (void);
	YM //void rotateZCCW   (void);
	YM void _addScale     (YAC_Object*, sF32);
	YM sF32 _getAbs       (void);
	YM sF32 _getAbsMax    (void);
	YM sF32 _getAbsAdd    (void);
	YM void _cross        (YAC_Object*, YAC_Object*);
	YM void _rotate       (YAC_Object*);
	YM void _rotateInv    (YAC_Object*);
	YM sF32 _getX         (void);
	YM sF32 _getY         (void);
	YM sF32 _getZ         (void);
	YM void _setX         (sF32);
	YM void _setY         (sF32);
	YM void _setZ         (sF32);
	YM void _print        (void);
	YM void _sphere       (sF32, sF32, sF32, sF32);
	YM void _add          (YAC_Object *);
	YM void _sub          (YAC_Object *);
	YM void _unitScale    (sF32);
	YM void _scalev       (YAC_Object *);
    YM void _initv        (YAC_Object *);
    YM void _getString    (YAC_Value *);
    YM void _blend        (YAC_Object *, sF32);
	YM void _rotateAdd    (YAC_Object *, YAC_Object *);
	YM void _rotateInvAdd (YAC_Object *, YAC_Object *);
	YM void _glVertex     (void);
	YM void _glNormal     (void);
	YM void _glColor      (void);
	YM void _glColorGamma (sF32);
	YM void _glScale      (void);
	YM void _faSet        (YAC_Object *, sSI);
	YM void _faGet        (YAC_Object *, sSI);
	YM void _initScale    (YAC_Object *, sF32);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ Vector4                                                        ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YC class _Vector4 : public YAC_Object {
public:
    sF32 x,y,z,w;
public:
    _Vector4(void);
    ~_Vector4();
	YAC(_Vector4);

	void YAC_VCALL yacSerialize     (YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize   (YAC_Object *, sUI);
	void YAC_VCALL yacOperatorClamp (YAC_Object*, YAC_Object*);
	void YAC_VCALL yacOperatorWrap  (YAC_Object*, YAC_Object*);
	void YAC_VCALL yacOperator      (sSI, YAC_Object *, YAC_Value*);

    YM void _setX            (sF32);
    YM sF32 _getX            (void);
    YM void _setY            (sF32);
    YM sF32 _getY            (void);
    YM void _setZ            (sF32);
    YM sF32 _getZ            (void);
    YM void _setW            (sF32);
    YM sF32 _getW            (void);
    YM void _init            (sF32, sF32, sF32, sF32);
    YM void _getString       (YAC_Value *);
	YM sF32 _interpolCubic2D (sF32, sF32);
	YM void _glColor         (void);
	YM void _glColorGamma    (sF32);
	YM void _glVertex        (void);
	YM void _faSet           (YAC_Object *, sSI);
	YM void _faGet           (YAC_Object *, sSI);
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ Matrix                                                         ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YC class _Matrix : public sFMatrix  {
public:
	_Matrix(void);
	~_Matrix();
	YAC(_Matrix);

	void YAC_VCALL yacSerialize   (YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize (YAC_Object *, sUI);
	void YAC_VCALL yacOperator    (sSI, YAC_Object*, YAC_Value *);

	YM //void        invert       (void);
	YM //void        scale        (sF32);
	YM //void        init         (sF32, sF32, sF32);
	YM //void        initDeg      (sF32, sF32, sF32);
	YM //void        loadIdentity (void);
	YM void        _add        (YAC_Object*);
	YM void        _mul        (YAC_Object*, YAC_Object*);
	YM void        _sub        (YAC_Object*);
	YM void        _getI       (YAC_Value *);
	YM void        _getJ       (YAC_Value *);
	YM void        _getK       (YAC_Value *);
	YM void        _setI       (YAC_Object*);
	YM void        _setJ       (YAC_Object*);
	YM void        _setK       (YAC_Object*);
	YM void        _print      (void);
	YM void        _initv      (YAC_Object *);
	YM void        _getString  (YAC_Value *);
	YM void        _rotate     (YAC_Object *);
	YM void        _rotateInv  (YAC_Object *);
	YM void        _mulInv     (YAC_Object*, YAC_Object*);
	YM void        _initIJK    (YAC_Object*, YAC_Object*, YAC_Object*);
	YM void        _initIJKInv (YAC_Object*, YAC_Object*, YAC_Object*);
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ _VectorArray                                                   ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YC class _VectorArray : public YAC_Object {
public:
	sUI      max_elements;
    sUI      num_elements;
    _Vector *elements;
public:
	_VectorArray(void);
	~_VectorArray();
	YAC(_VectorArray);

	sBool YAC_VCALL yacIteratorInit(YAC_Iterator *) const;

	void  YAC_VCALL yacSerialize     (YAC_Object *, sUI);
	sUI   YAC_VCALL yacDeserialize   (YAC_Object *, sUI);
   void  YAC_VCALL yacArrayCopySize        (YAC_Object *_vectorarray);
	sBool YAC_VCALL yacArrayAlloc           (sUI _num, sUI,sUI,sUI);
	void  YAC_VCALL yacArraySet             (void *_context, sUI _index, YAC_Value *_value);
	void  YAC_VCALL yacArrayGet             (void *_context, sUI _index, YAC_Value *_ret);
	sUI   YAC_VCALL yacArrayGetWidth          (void)  { return num_elements; }
	sUI   YAC_VCALL yacArrayGetHeight         (void)  { return 1; }
	sUI   YAC_VCALL yacArrayGetElementType    (void)  { return 3; }
	sUI   YAC_VCALL yacArrayGetElementByteSize(void)  { return sizeof(sF32); }
	sUI   YAC_VCALL yacArrayGetStride(void)           { return 0; }
	void *YAC_VCALL yacArrayGetPointer(void)          { return (void*)elements; }
   void  YAC_VCALL yacOperator             (sSI, YAC_Object*, YAC_Value *);

	YM sSI         _alloc          (sSI _maxelements);
	YM void        _free           (void);
	YM sSI         _add            (YAC_Object *_val);
	YM sSI         _addEmpty       (sUI _numelements);
	YM sUI         _getNumElements (void);
	YM sUI         _getMaxElements (void);
	YM void        _empty          (void);
	YM sSI         _realloc        (sSI _maxelements);
	YM sSI         _insert         (sSI _index, YAC_Object *_v);
	YM sSI         _delete         (sSI _index);
	YM void        _zero           (void);
	YM void        _reverse        (void);
	YM void        _print          (void);
	YM void        _rotate         (YAC_Object *_sourcevectorarray, YAC_Object *_matrix);
	YM void        _translatev     (YAC_Object *_sourcevectorarray, YAC_Object *_vector);
	YM void        _scalev         (YAC_Object *_sourcevectorarray, YAC_Object *_vector);
   YM YAC_Object *_getNextFree    (void);
   YM void        _setNumElements (sUI);
	YM void        _rotateInv      (YAC_Object *_sourcevectorarray, YAC_Object *_matrix);
   YM void        sortByX         (YAC_Object *_indexTable); 
   YM void        sortByY         (YAC_Object *_indexTable); 
   YM void        sortByZ         (YAC_Object *_indexTable); 
   void           quicksortByX    (sSI *, sSI _l, sSI _r); 
   void           quicksortByY    (sSI *, sSI _l, sSI _r); 
   void           quicksortByZ    (sSI *, sSI _l, sSI _r); 
   YM sSI         reorderFrom     (YAC_Object *_va, YAC_Object *_ia);
};

class _VectorArrayIterator : public YAC_Iterator {
public:
	const _VectorArray *array;
public:
	_VectorArrayIterator(const _VectorArray *);
	~_VectorArrayIterator();

	void YAC_VCALL getNext (YAC_Value *_r);
};




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ IVector                                                         ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YC class _IVector : public YAC_Object {
public:
	sSI x;
	sSI y;

public:
	_IVector(void);
	~_IVector();
	YAC(_IVector);

	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize(YAC_Object *, sUI); 
	void YAC_VCALL yacOperatorClamp(YAC_Object*, YAC_Object*);
	void YAC_VCALL yacOperatorWrap(YAC_Object*, YAC_Object*);
	void YAC_VCALL yacOperator(sSI, YAC_Object *, YAC_Value *);

	YM void _scale     (sF32 _f);
	YM void _addScale  (YAC_Object*, sF32);
	YM sF32 _abs       (void);
	YM sSI  _absMax    (void);
	YM sSI  _absAdd    (void);
	YM void _rotate    (YAC_Object*);
	YM void _rotateInv (YAC_Object*);
	YM void _neg       (void);
	YM void _init      (sSI, sSI);
	YM sSI  _getX      (void);
	YM sSI  _getY      (void);
	YM void _setX      (sSI);
	YM void _setY      (sSI);
	YM void _print     (void);
	YM void _sphere    (sF32 _a,sF32 _b,sF32 _c,sF32 _dist);
	YM void _add       (YAC_Object *_v);
	YM void _sub       (YAC_Object *_v);
	YM void _scaleXY   (sF32 _sx, sF32 _sy);
    YM void _getString (YAC_Value *_r);
};



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ _WrappedFloat                                                  ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
YC class _WrappedFloat : public YAC_Object {
public:
    sF32 value;
    sF32 add;
    sF32 min;
    sF32 max;

public:
    _WrappedFloat(void);
    ~_WrappedFloat();
	YAC(_WrappedFloat);

	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize(YAC_Object *, sUI);

    void YAC_VCALL yacOperator(sSI, YAC_Object*,YAC_Value*);

    YM void _setValue    (sF32 _value);
    YM sF32 _getValue    (void);
    YM void _setAdd      (sF32 _value);
    YM sF32 _getAdd      (void);
    YM void _setMin      (sF32 _value);
    YM sF32 _getMin      (void);
    YM void _setMax      (sF32 _value);
    YM sF32 _getMax      (void);
    YM void _tick        (void);
    YM void _init        (sF32, sF32, sF32, sF32);
    YM void _tickPrecise (sF32);

};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                   ~
//~ _Math                             ~
//~                                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
YC class _Math : public YAC_Object {
public:
	_Vector ret_vector;

public:
    _Math(void);
    ~_Math();

	YAC(_Math);

    static void Project(sFVector *_v, sFVector *_vret);

    YM sF32        _powerXY            (sF32, sF32);
    YM YAC_Object *_normal             (YAC_Object *, YAC_Object *, YAC_Object *);
    YM sF32        _max                (sF32, sF32);
    YM sF32        _absmax             (sF32, sF32);
    YM sF32        _min                (sF32, sF32);
    YM sF32        _absmin             (sF32, sF32);
    YM sUI         _c32Multiply        (sUI, sUI, sF32);
    YM sUI         _c32Blend           (sUI, sUI, sF32);
    YM void        _reflect            (YAC_Object *, YAC_Object *, YAC_Object *);
    YM void        _vectorMin          (YAC_Object *, YAC_Object *, YAC_Object *);
    YM void        _vectorMax          (YAC_Object *, YAC_Object *, YAC_Object *);
    YM sF32        _distancePointPlane (YAC_Object *, YAC_Object *, YAC_Object *);

};


#endif

