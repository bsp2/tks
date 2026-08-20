/// array.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///    - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_ARRAY_H__
#define TKS_ARRAY_H__


template <class T>
class Array {
public:
  sUI max_elements;
  sUI num_elements;
  T *elements;

public:
  Array(void) {
    max_elements = 0u;
    num_elements = 0u;
    elements     = NULL;
  }

  ~Array() {
	  Array::free();
  }

  void free(void) {
    if(NULL != elements)
    {
	    delete [] elements;
       elements = NULL;
	    num_elements = 0u;
	    max_elements = 0u;
    }
  }

  sBool alloc(sUI _max_elements) {
    if(NULL != elements)
    {
	    delete [] elements;
	    num_elements = 0u;
	    max_elements = 0u;
    }

    if(_max_elements > 0u)
    {
       elements = new(std::nothrow) T [ _max_elements ];

       if(NULL != elements)
       {
          max_elements = _max_elements;
          num_elements = 0u;
          return YAC_TRUE;
        }
    }

    return YAC_FALSE;
  }

};


#endif // TKS_ARRAY_H__
