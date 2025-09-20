
// originally written by Carsten Busse
//  some source cleanup by Bastian Spiegel

/// Define to enable some debug prints
//#define DEBUG_FILEDIALOG defined


#include <stdio.h>
#include <stdarg.h>

#define YAC_PRINTF
#define YAC_BIGSTRING
#include <yac.h>

#include <gtk/gtk.h>
#include "ying_ui.h"

#include "TKUI_CommonDialogs.h"


#ifdef DEBUG_FILEDIALOG
#define Dprintf if(0);else yac_host->printf
#else
#define Dprintf if(1);else yac_host->printf
#endif // DEBUG_FILEDIALOG



sBool gtkinitialised = 0;

GtkWidget *fw = 0;


void TKUI_OpenFileDialog(YAC_Object *suggPath, 
                         YAC_Object *suggFile, 
                         YAC_Object *titleCaption, 
                         YAC_Object *filterNames, 
                         YAC_Object *filters, 
                         sSI multiSel, 
                         sSI selDir, 
                         YAC_Value *_r
                         ) 
{
	gchar *path;
	gchar *file;
	const gchar *title;
	const gchar *okbutton;
	const gchar *cancelbutton;
	GtkFileChooserAction action;
	GSList *returnVal;
	int doFilters;
	bool multi;
	bool dir;

	//gtk minimum initialisation done?
	if (!gtkinitialised) 
   {
		//nay
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("init gtk\n");
#endif
		gtk_init(0,0);
		gtkinitialised++;
	}
   else
   {
      //yep, say so
      Dprintf("no gtk init needed\n");
   }

	//result value minimum initialisation tks
	_r->initObject(0,0);
	
	//check suggPath is String
	if (YAC_BCHK(suggPath,YAC_CLID_STRING)) 
   {
		YAC_String *tmp=(YAC_String*)suggPath;
      path=(gchar*)(tmp->chars);

      Dprintf("Using suggPath %s\n",path);
	} 
   else 
   {
		path = NULL;

      Dprintf("suggPath not a valid String object!\n");
	}

	//check suggFile is String
	if(YAC_Is_String(suggFile))
   {
		YAC_String *tmp = (YAC_String*)suggFile;
		file = (gchar*)(tmp->chars);

      Dprintf("Using suggFile %s\n",file);
	}
   else
   {
      file = NULL;
   }
	
	//check titleCaption is String
	if(YAC_Is_String(titleCaption))
   {
		YAC_String *tmp = (YAC_String*)titleCaption;
		title = (gchar*)(tmp->chars);

      Dprintf("Using title %s\n",title);

	}
   else
   {
      title = "File Selection";
   }
	
	//nah, button names got canceles, so we take stock values from gtk
	okbutton = GTK_STOCK_OK;
	cancelbutton = GTK_STOCK_CANCEL;
	
	//mmhh, is there a reason to look for the filters?
	if((!YAC_VALID(filters)) || (!YAC_VALID(filterNames))) 
   {
		doFilters = 0;
	}
	else 
	{
		if(filters->yacArrayGetNumElements() >= filterNames->yacArrayGetNumElements())
      {
         doFilters = 1;
      }
		else 
      {
         doFilters = 0;
      }
	}

   Dprintf("Filters provided? %d\n",doFilters);
	
	//thought everything was a yac_object, so now we have to copy those small booleans
   // (Note: wrong :) int are ints and floats are floats are floats, everything else is an object)
	multi = multiSel;
	
	dir = selDir;
	
	//action determination
	if(dir)
   {
      action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
   }
	else
   {
      action = GTK_FILE_CHOOSER_ACTION_OPEN;
   }
	
	//did we already create a filechooser widget we could recycle?
	if(!fw) 
   {
		//nay

      Dprintf("file chooser created\n");

		fw = gtk_file_chooser_dialog_new(title,
                                       NULL,
                                       action,
                                       okbutton,
                                       1,
                                       cancelbutton,
                                       GTK_RESPONSE_CANCEL,
                                       NULL
                                       );
	} 
   else
   {
		//yap, so make sure that no old filter interferes with the new ones and set action + title

      Dprintf("file chooser recycled\n");

		gtk_window_set_title(GTK_WINDOW(fw), title);
		gtk_file_chooser_set_action(GTK_FILE_CHOOSER(fw), action);
		GSList *filterList = gtk_file_chooser_list_filters(GTK_FILE_CHOOSER(fw));
		GSList *tmp = filterList;

      Dprintf("Remove old filters\n");

		while(tmp)
      {
			gtk_file_chooser_remove_filter(GTK_FILE_CHOOSER(fw),
                                        (GtkFileFilter*)tmp->data
                                        );
			tmp = tmp->next;
		}
		g_slist_free(filterList);
	}	
	
	//tell clearly, if multiple selection is allowed or not
	if(multi)
   {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(fw), true);
   }
	else
   {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(fw), false);
   }

	//set path
	if(path)
   {
      gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fw), path);
   }

	//(try to) set file
	if(file)
   {
		//needs absolute pathnames!
		YAC_String test;
		test.visit(path);

		YAC_String slash;
		slash.visit("/");    //Where is the common folder separator in gtk?

		test.append(&slash);

		YAC_String toApp;
		toApp.visit(file);

		test.append(&toApp);

      Dprintf("Teststring %s\n", (char*)test.chars);

		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(fw),
                                    (char*)test.chars
                                    );
		gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(fw),
                                       (char*)test.chars
                                       );
	}
	
   //filter dance?
	if(doFilters)
   {
		YAC_Object *tmp = filters;

      Dprintf("Parsing filter\n");
		
		GtkFileFilter *filter = NULL; // assign NULL to satisfy GCC

		sUI numElem = tmp->yacArrayGetNumElements();

		Dprintf("%d Patterns to parse\n",numElem);

		for(sUI i=0; i<numElem; i++)
      {
			YAC_Value elemTmp;
			YAC_Value nameTmp;

			filters->yacArrayGet(NULL, i,&elemTmp);// xxx TKS_MT: should use real thread context (exceptions..)

			if(elemTmp.type < YAC_TYPE_OBJECT)
         {
            continue;
         }

			if(!YAC_Is_String(elemTmp.value.object_val))
         {
            continue;
         }

			YAC_String *elem = elemTmp.value.string_val;

			filterNames->yacArrayGet(NULL, i,&nameTmp); // xxx TKS_MT: should use real thread context (exceptions..)

			if(nameTmp.type >= YAC_TYPE_OBJECT)
         {
				if(YAC_Is_String(nameTmp.value.object_val))
            {
					//clearly a string name provided
					YAC_String *name = nameTmp.value.string_val;
					filter = gtk_file_filter_new();

               Dprintf("Set filtername (provided) %s\n",name->chars);

					gtk_file_filter_set_name(filter, (char*)name->chars);
				}
            else
            {
					//whatever it is, its not a string, so create default name (pure pattern with ';')
               Dprintf("Set filtername (nostring) %s\n",(char*)elem->chars);

					gtk_file_filter_set_name(filter,(char*)elem->chars);
				}
			}
         else
         {
				//this is not even an object, use default (see one comment up)
            Dprintf("Set filtername (noobject) %s\n",elem->chars);

				gtk_file_filter_set_name(filter, (char*)elem->chars);
			}
		
         //separate the string elements of the pattern divided by ';'	
			sUI lastpos = 0;
			for(sUI j=0; j<elem->length; j++)
         {
				if(elem->chars[j] == ';')     //  ------ Filter Separator Char
            {
					YAC_String newPattern;
					elem->substring(&newPattern,lastpos,j-lastpos);

               Dprintf("Filter pattern: %s\n", (char*)newPattern.chars);

					gtk_file_filter_add_pattern(filter, (char*)newPattern.chars);

					lastpos = j + 1;
				}
			}

			//add the last reminaing pattern (or the only one)
			YAC_String lastPattern;
			elem->substring(&lastPattern,lastpos,elem->length-lastpos);

         Dprintf("Filter pattern: %s\n", (char*)lastPattern.chars);

			gtk_file_filter_add_pattern(filter, 
                                     (char*)lastPattern.chars
                                     );
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fw), 
                                     GTK_FILE_FILTER(filter)
                                     );
		}
	}
		
	g_signal_connect(G_OBJECT(fw),
                    "destroy",
                    G_CALLBACK(gtk_main_quit),
                    NULL
                    );

	gtk_widget_show(fw);

   Dprintf("Running dialog->run()\n");

	sS32 responseCode = gtk_dialog_run(GTK_DIALOG(fw));

	switch(responseCode)
   {
		case GTK_RESPONSE_CANCEL:
			break;

		case 1:
			//convert GTK Result GSList back to TKS StringArray
			returnVal = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(fw));
			int num=0;
			GSList *tmp=returnVal;
			while(tmp)
         {
				num++;
				tmp = tmp->next;
			}

			tmp = returnVal;

         if(!multiSel)
         {
            // single selection, return string
            if(num > 0)
            {
               YAC_String *rs = YAC_New_String();
               rs->copy((gchar*)tmp->data);
               _r->initObject(rs, 1);
            }
         }
         else
         {
            // multiple selection, return array of strings
            YAC_StringArray *a = YAC_New_StringArray();///(YAC_StringArray*) yac_host->yacNewByID(YAC_CLID_STRINGARRAY);

            if(a->yacArrayAlloc(num,0,0,0))
            {
               for(int i=0; i<num; i++)
               {
                  YAC_String s;
                  s.visit((gchar*)tmp->data);

                  YAC_Value v;
                  v.initObject(&s,0);
                  a->yacArraySet(NULL, i,&v); // xxx TKS_MT: should use real thread context (exceptions..)

                  tmp = tmp->next;
               }
            }

            _r->initObject(a, 1);
         }

			g_slist_free(returnVal);
			break;	
	}

   Dprintf("Trying to hyde dialog\n");

	gtk_widget_hide(GTK_WIDGET(fw));

   Dprintf("Iterate some events: ");

	//iterate all remaining gtk events, to make sure that the window gets optically removed
	while(gtk_events_pending())
   {
		gtk_main_iteration();

      Dprintf(".");
	}

   Dprintf("\nIteration done, returning\n");
}

void TKUI_SaveFileDialog(YAC_Object *suggPath, 
                         YAC_Object *suggFile, 
                         YAC_Object *titleCaption, 
                         YAC_Object *filterNames, 
                         YAC_Object *filters, 
                         sSI selDir, 
                         YAC_Value *_r)
{
   // TODO: clean me up
	gchar *path;
	gchar *file;
	const gchar *title;
	const gchar *okbutton;
	const gchar *cancelbutton;
	GtkFileChooserAction action;
	GSList *returnVal;
	int doFilters;
	bool dir;

	if (!gtkinitialised) {
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("init gtk\n");
#endif
		gtk_init(0,0);
		gtkinitialised++;
	}
#ifdef DEBUG_FILEDIALOG	
	else yac_host->printf("no gtk init needed\n");
#endif

	_r->initObject(0,0);
	
	if (YAC_BCHK(suggPath,YAC_CLID_STRING)) {
		YAC_String *tmp=(YAC_String*)suggPath;
	        path=(gchar*)(tmp->chars);
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("Using suggPath %s\n",path);
#endif
	} else {
		path=NULL;
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("suggPath not a valid String object!\n");
#endif
	}

	if (YAC_BCHK(suggFile,YAC_CLID_STRING)) {
		YAC_String *tmp=(YAC_String*)suggFile;
		file=(gchar*)(tmp->chars);
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("Using suggFile %s\n",file);
#endif
	} else file=NULL;
	
	if (YAC_BCHK(titleCaption,YAC_CLID_STRING)) {
		YAC_String *tmp=(YAC_String*)titleCaption;
		title=(gchar*)(tmp->chars);
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("Using title %s\n",title);
#endif
	} else title="File Selection";
	
	okbutton=GTK_STOCK_OK;
	cancelbutton=GTK_STOCK_CANCEL;
	
	if ((!YAC_VALID(filters))||(!YAC_VALID(filterNames))) {
		doFilters=0;
	}
	else 
	{
		if (filters->yacArrayGetNumElements()>=filterNames->yacArrayGetNumElements())
		doFilters=1;
		else doFilters=0;
	}

#ifdef DEBUG_FILEDIALOG
	yac_host->printf("Filters provided? %d\n",doFilters);
#endif
	
	dir=selDir;
	
	if (dir) action=GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;
	else action=GTK_FILE_CHOOSER_ACTION_SAVE;

	
	if (!fw) {
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("file chooser created\n");
#endif
		fw=gtk_file_chooser_dialog_new(title,NULL,action,okbutton,1,cancelbutton,GTK_RESPONSE_CANCEL,NULL);
	} else {
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("file chooser recycled\n");
#endif
		gtk_window_set_title(GTK_WINDOW(fw),title);
		gtk_file_chooser_set_action(GTK_FILE_CHOOSER(fw),action);
		GSList *filterList=gtk_file_chooser_list_filters(GTK_FILE_CHOOSER(fw));
		GSList *tmp=filterList;
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("Remove old filters\n");
#endif
		while (tmp) {
			gtk_file_chooser_remove_filter(GTK_FILE_CHOOSER(fw),(GtkFileFilter*)tmp->data);
			tmp=tmp->next;
		}
		g_slist_free(filterList);
	}	
	
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(fw),false);

	if (path) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fw),path);

	if (file) {
		//needs absolute pathnames!
		YAC_String test;
		test.visit(path);
		YAC_String slash;
		slash.visit("/");    //Where is the common folder separator in gtk?
		test.append(&slash);
		YAC_String toApp;
		toApp.visit(file);
		test.append(&toApp);
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("Teststring %s\n", (char*)test.chars);
#endif
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(fw), (char*)test.chars);
		gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(fw), (char*)test.chars);
	}
	

	if (doFilters)
   {
		YAC_Object *tmp=filters;
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("Parsing filter\n");
#endif
		
		GtkFileFilter *filter = NULL; // assign NULL to satisfy GCC
		sUI numElem=tmp->yacArrayGetNumElements();
#ifdef DEBUG_FILEDIALOG
		yac_host->printf("%d Patterns to parse\n",numElem);
#endif
		for (sUI i=0;i<numElem;i++) 
      {
			YAC_Value elemTmp;
			YAC_Value nameTmp;
			filters->yacArrayGet(NULL, i,&elemTmp); // xxx TKS_MT: should use real thread context (exceptions..)
			if(elemTmp.type < YAC_TYPE_OBJECT)
         {
            continue;
         }
			if(!YAC_Is_String(elemTmp.value.object_val))
         {
            continue;
         }
			YAC_String *elem = elemTmp.value.string_val;

			filterNames->yacArrayGet(NULL, i,&nameTmp); // xxx TKS_MT: should use real thread context (exceptions..)
			if(nameTmp.type>=YAC_TYPE_OBJECT)
         {
				if(YAC_Is_String(nameTmp.value.object_val))
            {
					YAC_String *name = nameTmp.value.string_val;
					filter = gtk_file_filter_new();
#ifdef DEBUG_FILEDIALOG
					yac_host->printf("Set filtername (provided) %s\n", (char*)name->chars);
#endif
					gtk_file_filter_set_name(filter, (char*)name->chars);
				} 
            else
            {
#ifdef DEBUG_FILEDIALOG
					yac_host->printf("Set filtername (nostring) %s\n",elem->chars);
#endif
					gtk_file_filter_set_name(filter, (char*)elem->chars);
				}
			} 
         else
         {
#ifdef DEBUG_FILEDIALOG
				yac_host->printf("Set filtername (noobject) %s\n", (char*)elem->chars);
#endif
				gtk_file_filter_set_name(filter, (char*)elem->chars);
			}
			
			sUI lastpos=0;
			for (sUI j=0;j<elem->length;j++)
         {
				if (elem->chars[j]==';')
            {  //  ------ Filter Separator Char
					YAC_String newPattern;
					elem->substring(&newPattern,lastpos,j-lastpos);
#ifdef DEBUG_FILEDIALOG
					yac_host->printf("Filter pattern: %s\n", (char*)newPattern.chars);
#endif
					gtk_file_filter_add_pattern(filter, (char*)newPattern.chars);
					lastpos=j+1;
				}
			}
			YAC_String lastPattern;
			elem->substring(&lastPattern,lastpos,elem->length-lastpos);
#ifdef DEBUG_FILEDIALOG
			yac_host->printf("Filter pattern: %s\n", (char*)lastPattern.chars);
#endif
			gtk_file_filter_add_pattern(filter, (char*)lastPattern.chars);
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fw),GTK_FILE_FILTER(filter));
		}
	}
   
	g_signal_connect(G_OBJECT(fw),"destroy",G_CALLBACK(gtk_main_quit),NULL);

	gtk_widget_show(fw);
#ifdef DEBUG_FILEDIALOG
	yac_host->printf("Running dialog->run()\n");
#endif
	sS32 responseCode=gtk_dialog_run(GTK_DIALOG(fw));
	switch (responseCode) {
		case GTK_RESPONSE_CANCEL:
			break;
		case 1:
			returnVal=gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(fw));
			int num=0;
			GSList *tmp=returnVal;
			while (tmp) {
				num++;
				tmp=tmp->next;
			}
			tmp=returnVal;

         if(0)
         {
            // (note) multiple selection not supported for save file dialogs
            YAC_StringArray *a=(YAC_StringArray*)yac_host->yacNewByID(YAC_CLID_STRINGARRAY);
            if (a->yacArrayAlloc(num,0,0,0)) 
            {
               for (int i=0;i<num;i++) 
               {
                  YAC_String s;
                  s.visit((gchar*)tmp->data);
                  YAC_Value v;
                  v.initObject(&s,0);
                  a->yacArraySet(NULL, i,&v);
                  tmp=tmp->next;
               }
            }
            _r->initObject(a, 1);
         }
         else
         {
            YAC_String *rs = YAC_New_String();
            rs->copy((gchar*)tmp->data);
            _r->initObject(rs, 1);
         }

			g_slist_free(returnVal);

         break;	
	}
#ifdef DEBUG_FILEDIALOG
	yac_host->printf("Trying to hyde dialog\n");
#endif
	gtk_widget_hide(GTK_WIDGET(fw));
#ifdef DEBUG_FILEDIALOG
	yac_host->printf("Iterate some events: ");
#endif
	while (gtk_events_pending()) {
		gtk_main_iteration();
#ifdef DEBUG_FILEDIALOG
		yac_host->printf(".");
#endif
	}
#ifdef DEBUG_FILEDIALOG
	yac_host->printf("\n");
	yac_host->printf("Iteration done, returning\n");
#endif
}
