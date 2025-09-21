#ifndef POLYRASTER_H__
#define POLYRASTER_H__

YG("polyraster")


YF void YAC_CALL polyraster_calc_bbox (YAC_Object *_fa, YAC_Object *_retMinX, YAC_Object *_retMinY, YAC_Object *_retMaxX, YAC_Object *_maxY);

YF void YAC_CALL polyraster_edges (YAC_Object *_tex, YAC_Object *_fa);

YF void YAC_CALL polyraster_fill (YAC_Object *_tex);

YF void YAC_CALL polyraster (YAC_Object *_tex, YAC_Object *_faVerts);


#endif // POLYRASTER_H__
