/// array.h 
/// 
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de> 
///    - distributed under terms of the GNU general public license (GPL). 
/// 

#ifndef __TKS_TEMPLATE_ARRAY_H__
#define __TKS_TEMPLATE_ARRAY_H__ 


template <class T>
class Array {
public:
  sUI max_elements;
  sUI num_elements;
  T *elements;

public:
  Array(void) {
    max_elements = 0;
    num_elements = 0;
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
	    num_elements = 0;
	    max_elements = 0;
    }
  }

  sBool alloc(sUI _max_elements) {
    if(NULL != elements)
    {
	    delete [] elements;
	    num_elements = 0;
	    max_elements = 0;
    }

    if(_max_elements > 0)
    {
        elements = new T [ _max_elements ];

        if(NULL != elements)
        {
            max_elements = _max_elements;
            num_elements = 0;  
            return YAC_TRUE;
        }
    }

    return YAC_FALSE;
  }

};


#endif // __TKS_TEMPLATE_ARRAY_H__
