/// tksqlite.h
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL).
///
///

/// def -> lib:  $ dlltool --dllname sqlite3.dll --def sqlite3.def --output-lib sqlite3.lib -k
/*

create table person (id integer NOT NULL, name varchar(64) NOT NULL, surname varchar(64) NOT NULL, unique(id), primary key(id AUTOINCREMENT));
create table role (id integer NOT NULL, name varchar(64), unique(id), primary key(id AUTOINCREMENT));
create table person_in_role (person_in_role_id integer NOT NULL, person_id integer NOT NULL, role_id integer NOT NULL, unique(person_id, role_id), primary key(person_in_role_id AUTOINCREMENT));
insert into person (surname, name) values("rainer", "hohn");
insert into person (surname, name) values("carl", "coder");
insert into person (surname, name) values("michael", "mnemonic");
insert into person (surname, name) values("paula", "programmer");
insert into role (name) values("users");
insert into role (name) values("admin");
insert into person_in_role (person_id, role_id) values((select person.id from person where name='hohn' and surname='rainer'), (select role.id from role where name='users'));
insert into person_in_role (person_id, role_id) values((select person.id from person where name='hohn' and surname='rainer'), (select role.id from role where name='admin'));
insert into person_in_role (person_id, role_id) values((select person.id from person where name='mnemonic' and surname='michael'), (select role.id from role where name='users'));
insert into person_in_role (person_id, role_id) values((select person.id from person where name='programmer' and surname='paula'), (select role.id from role where name='users'));

select person.surname,person.name,role.name from person,role,person_in_role where person.id=person_in_role.person_id AND role.id=person_in_role.role_id;
rainer|hohn|users
rainer|hohn|admin
michael|mnemonic|users
paula|programmer|users
  */

#define YAC_BIGSTRING defined
#define YAC_PRINTF defined

#include <yac.h>
#include <yac_host.cpp>
#include <sqlite3.h>

#include "tksqlite.h"

YAC_Host *yac_host;

#include "ying_sqlite.h"
#include "ying_sqlite.cpp"
#include "ying_sqlite_SQLite_Statement.cpp"
#include "ying_sqlite_SQLite_Database.cpp"


SQLite_Statement::SQLite_Statement(void) {
   st_handle = NULL;
   database  = NULL;
   prev      = NULL;
   next      = NULL;

   b_have_row = 0;
}

SQLite_Statement::~SQLite_Statement() {
   if(st_handle != NULL)
   {
      sqlite3_finalize(st_handle);
      st_handle = NULL;
   }
   if(database != NULL)
   {
      // Unlink from database.statements list
      if(prev != NULL)
      {
         prev->next = next;
      }
      if(next != NULL)
      {
         next->prev = prev;
      }
      if(prev == NULL)
      {
         database->first_statement = next;
      }
   }
}

void        SQLite_Statement::clearParameters        (void) {
   if(st_handle != NULL)
   {
      int numParam = sqlite3_bind_parameter_count(st_handle);
      while(--numParam >= 0)
      {
         sqlite3_bind_null(st_handle, numParam);
      }
      ///sqlite3_clear_bindings(st_handle);
      // xxx missing sqlite3.h
   }
}

void        SQLite_Statement::getHashTableResult     (YAC_Value *_r) {
}

void        SQLite_Statement::getValueArrayResult    (YAC_Value *_r) {
}

void        SQLite_Statement::getStringArrayResult   (YAC_Value *_r) {
   if(b_have_row)
   {
      YAC_StringArray *a = YAC_New_StringArray();
      storeStringArrayResult(a);
      _r->initObject(a, 1);
   }
   else
   {
      _r->initNull();
   }
}

YAC_Object *SQLite_Statement::getDatabase            (void) {
   return database;
}

sSI         SQLite_Statement::getColumnCount         (void) {
   if(st_handle != NULL)
   {
      if(b_have_row)
      {
         int cc = sqlite3_column_count(st_handle);
         return cc;
      }
   }
   return 0;
}

void SQLite_Statement::getColumnName          (sSI _columnIndex, YAC_Value *_r) {

   _r->initString(NULL, 0);

   if(st_handle != NULL)
   {
      if(b_have_row)
      {
         const char *cn = sqlite3_column_name(st_handle, _columnIndex);
         YAC_String *s = YAC_New_String();
         s->copy((char*)cn);
         _r->initString(s, 1);
      }
   }
}

sSI SQLite_Statement::getColumnType          (sSI _columnIndex) {
   if(st_handle != NULL)
   {
      if(b_have_row)
      {
         int t = sqlite3_column_type(st_handle, _columnIndex);
         return t;
      }
   }
   return SQLITE_ERROR;
}

void SQLite_Statement::getColumnDeclType       (sSI _columnIndex, YAC_Value *_r) {

   _r->initString(NULL, 0);

   if(st_handle != NULL)
   {
      if(b_have_row)
      {
         const char *ct = sqlite3_column_decltype(st_handle, _columnIndex);
         YAC_String *s = YAC_New_String();
         s->copy((char*)ct);
         _r->initString(s, 1);
      }
   }
}

void        SQLite_Statement::getColumnValue         (sSI _columnIndex, YAC_Value *_r) {
   
   _r->initVoid();

   if(st_handle != NULL)
   {
      int ct = sqlite3_column_type(st_handle, _columnIndex);
      switch(ct)
      {
      case SQLITE_INTEGER:
         _r->initInt( sqlite3_column_int(st_handle, _columnIndex) );
         break;
      case SQLITE_FLOAT:
         {
            YAC_Double *d = YAC_New_Double();
            d->value = sqlite3_column_double(st_handle, _columnIndex);
            _r->initObject(d, 1);
         }
         break;
      case SQLITE3_TEXT:
         {
            YAC_String *s = YAC_New_String();
            _r->initString(s, 1);
            s->copy( (char*)sqlite3_column_text(st_handle, _columnIndex) );
         }
         break;
      case SQLITE_BLOB:
         {
            YAC_Buffer *b = (YAC_Buffer*)YAC_New_Buffer();
            sSI size = sqlite3_column_bytes(st_handle, _columnIndex);
            if(b->yacArrayAlloc(size, 0,0,0))
            {
               sU8*data = (sU8*)sqlite3_column_blob(st_handle, _columnIndex);
               if(data != NULL)
               {
                  sU8*dst = (sU8*) b->yacArrayGetPointer();
                  sSI i=0;
                  for(; i<size; i++)
                  {
                     dst[i] = data[i];
                  }
               }
            }
            else
            {
               yac_host->printf("[---] SQLite_Statement::getColumnValue: cannot allocate %i bytes for BLOB column.\n", size);
               yac_host->yacDelete(b);
            }
         }
         break;
      case SQLITE_NULL:
         _r->initObject(NULL, 0);
         break;
      }
   }

}

sSI         SQLite_Statement::getParameterCount      (void) {
   if(st_handle != NULL)
   {
      int r = sqlite3_bind_parameter_count(st_handle);
      return r;
   }
   return 0;
}

void SQLite_Statement::getParameterName       (sSI _index, YAC_Value *_r) {
   
   _r->initString(NULL, 0);

   if(st_handle != NULL)
   {
      YAC_String *s = YAC_New_String();
      const char *pn = sqlite3_bind_parameter_name(st_handle, _index);
      if(pn != NULL)
      {
         s->copy(s);
      }
   }
}

void        SQLite_Statement::reset                  (void) {
   if(st_handle != NULL)
   {
      sqlite3_reset(st_handle);
      b_have_row = 0;
   }
}

void SQLite_Statement::setBlob(sSI _index, YAC_Object *_Buffer_blob) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_Buffer_blob))
      {
         void *data = _Buffer_blob->yacArrayGetPointer();
         if(data != NULL)
         {
            sUI byteSize = _Buffer_blob->yacArrayGetElementByteSize() * _Buffer_blob->yacArrayGetNumElements();
            sqlite3_bind_blob(st_handle, _index, data, byteSize, SQLITE_TRANSIENT);
         }
      }
   }
}

void SQLite_Statement::setBlobByName(YAC_Object *_String_name, YAC_Object *_Buffer_blob) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            if(idx > 0)
            {
               setBlob(idx, _Buffer_blob);
            }
         }
      }
   }
   
}

void SQLite_Statement::setDouble(sSI _index, YAC_Object *_Double_value) {
   if(st_handle != NULL)
   {
      sF64 val = 0.0;
      
      if(YAC_VALID(_Double_value))
      {
         _Double_value->yacScanF64(&val);
      }
      sqlite3_bind_double(st_handle, _index, val);
   }
}

void SQLite_Statement::setDoubleByName(YAC_Object *_String_name, YAC_Object *_Double_value) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            if(idx > 0)
            {
               sF64 val = 0.0;
               
               if(YAC_VALID(_Double_value))
               {
                  _Double_value->yacScanF64(&val);
               }
               sqlite3_bind_double(st_handle, idx, val);
            }
         }
      }
   }
}

void SQLite_Statement::setFloat(sSI _index, sF32 _value) {
   if(st_handle != NULL)
   {
      sqlite3_bind_double(st_handle, _index, (sF64)_value);
   }
}

void SQLite_Statement::setFloatByName(YAC_Object *_String_name, sF32 _value) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            if(idx > 0)
            {
               sqlite3_bind_double(st_handle, idx, (sF64)_value);
            }
         }
      }
   }
}

void SQLite_Statement::setInt(sSI _index, sSI _value) {
   if(st_handle != NULL)
   {
      int r = sqlite3_bind_int(st_handle, _index, _value);
      (void)r;
   }
}

void SQLite_Statement::setIntByName(YAC_Object *_String_name, sSI _value) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            if(idx > 0)
            {
               int r = sqlite3_bind_int(st_handle, idx, _value);
               (void)r;
            }
         }
      }
   }
}

void SQLite_Statement::setLong (sSI _index, YAC_Object *_Long_value) {
   if(st_handle != NULL)
   {
      sS64 val = 0;
      
      if(YAC_VALID(_Long_value))
      {
         _Long_value->yacScanI64(&val);
      }
      sqlite3_bind_int64(st_handle, _index, val);
   }
}

void SQLite_Statement::setLongByName(YAC_Object *_String_name, YAC_Object *_Long_value) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            if(idx > 0)
            {
               sS64 val = 0;
               
               if(YAC_VALID(_Long_value))
               {
                  _Long_value->yacScanI64(&val);
               }
               sqlite3_bind_int64(st_handle, idx, val);
            }
         }
      }
   }
}

void SQLite_Statement::setNull(sSI _index) {
   if(st_handle != NULL)
   {
      sqlite3_bind_null(st_handle, _index);
   }
}

void SQLite_Statement::setNullByName(YAC_Object *_String_name) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            if(idx > 0)
            {
               sqlite3_bind_null(st_handle, idx);
            }
         }
      }
   }
}

void        SQLite_Statement::setText                (sSI _index, YAC_Object *_String_text) {
   if(st_handle != NULL)
   {
      sBool bOk = 0;
      
      if(YAC_VALID(_String_text))
      {
         if(YAC_Is_String(_String_text))
         {
            YAC_String *text = (YAC_String*) _String_text;
            if( (text->chars!=NULL) && (text->length>0) )
            {
               sqlite3_bind_text(st_handle, _index, (const char*)text->chars, text->length-1, SQLITE_STATIC);
               bOk = 1;
            }
         }
      }

      if(!bOk)
      {
         yac_host->printf("[---] SQLite_Statement::setText(index=%i): text@%p is not a valid string.\n", _index, _String_text);
         sqlite3_bind_null(st_handle, _index);
      }
   }
}

void  SQLite_Statement::setTextByName(YAC_Object *_String_name, YAC_Object *_String_text) {
   if(st_handle != NULL)
   {
      if(YAC_VALID(_String_name))
      {
         if(YAC_Is_String(_String_name))
         {
            YAC_String *name = (YAC_String*) _String_name;
            sSI idx = sqlite3_bind_parameter_index(st_handle, (const char*) name->chars);
            yac_host->printf("map %s -> %i\n", name->chars, idx);
            if(idx > 0)
            {
               setText(idx, _String_text);
            }
         }
      }
   }
}


sSI         SQLite_Statement::step                   (void) {
   if(st_handle != NULL)
   {
      int r = sqlite3_step(st_handle);
      b_have_row = (r == SQLITE_ROW);
      return r;
   }
   return SQLITE_ERROR;
}

void        SQLite_Statement::storeHashTableResult   (YAC_Object *_HashTable_ret) {
}

void        SQLite_Statement::storeValueArrayResult  (YAC_Object *_ValueArray_ret) {
   if(YAC_VALID(_ValueArray_ret))
   {
      if(YAC_BCHK(_ValueArray_ret, YAC_CLID_VALUEARRAY))
      {
         YAC_ValueArray *a = (YAC_ValueArray *) _ValueArray_ret;
         if(st_handle != NULL)
         {
            if(b_have_row)
            {
               sSI numCols = sqlite3_data_count(st_handle);
               ////yac_host->printf("numCols = %i\n", numCols);
               if(a->yacArrayAlloc((sUI) numCols, 0,0,0))
               {
                  YAC_Value v;
                  int i=0;
                  for(; i<numCols; i++)
                  {
                     int t = sqlite3_column_type(st_handle, i);
                     switch(t)
                     {
                     case SQLITE_NULL:
                        v.initNull();
                        break;

                     case SQLITE_INTEGER:
                        {
                           YAC_Long *l = YAC_New_Long();
                           l->value = (sS64) sqlite3_column_int64(st_handle, i);
                        }
                        break;

                     case SQLITE_FLOAT:
                        {
                           YAC_Double *d = YAC_New_Double();
                           d->value = (sF64) sqlite3_column_double(st_handle, i);
                        }
                        break;

                     case SQLITE_BLOB:
                        {
                           YAC_Buffer *b = (YAC_Buffer*) YAC_New_Buffer();
                           int len = sqlite3_column_bytes(st_handle, i);
                           const sU8*src = (const sU8*) sqlite3_column_blob(st_handle, i);
                           if(b->yacArrayAlloc(len, 1, 0,0))
                           {
                              sU8 *dst = (sU8*) b->yacArrayGetPointer();
                              for(int i=0; i<len; i++)
                              {
                                 *dst++ = *src++;
                              }
                           }
                           v.initObject(b, 1);
                        }
                        break;

                     case SQLITE_TEXT:
                        {
                           YAC_String *s = YAC_New_String();
                           const sU8 *src = sqlite3_column_text(st_handle, i);
                           int len = sqlite3_column_bytes(st_handle, i);
                           YAC_String t;
                           t.alloc(len+1);
                           sU8 *d = v.value.string_val->chars;
                           for(int j=0; j<len; j++)
                           {
                              *d++ = src[j];
                           }
                           *d++ = 0;
                           t.length = len + 1;
                           s->yacCopy(&t);
                           v.initString(s, 1);
                        }
                        break;

                     default:
                        yac_host->printf("[---] unhandled column type %i (idx=%i).\n", t, i);
                        v.initNull();
                        break;
                     }
                     a->yacArraySet(NULL/*context*/, i, &v);
                  }
               }
               return;
            }
         }
         a->yacArrayAlloc(0,0,0,0);
      }
   }
}

void SQLite_Statement::storeStringArrayResult (YAC_Object *_StringArray_ret) {
   if(YAC_VALID(_StringArray_ret))
   {
      if(YAC_BCHK(_StringArray_ret, YAC_CLID_STRINGARRAY))
      {
         YAC_StringArray *a = (YAC_StringArray*) _StringArray_ret;
         if(st_handle != NULL)
         {
            if(b_have_row)
            {
               sSI numCols = sqlite3_data_count(st_handle);
               ////yac_host->printf("numCols = %i\n", numCols);
               if(a->yacArrayAlloc((sUI) numCols, 0,0,0))
               {
                  YAC_String t;
                  YAC_Value v;
                  v.initString(&t, 0);
                  int i=0;
                  for(; i<numCols; i++)
                  {
                     const unsigned char *src = sqlite3_column_text(st_handle, i);
                     int len = sqlite3_column_bytes(st_handle, i);
                     ////yac_host->printf("src len=%i data=%s\n", len, src);
                     t.alloc(len+1);
                     sU8 *d = v.value.string_val->chars;
                     for(int j=0; j<len; j++)
                     {
                        *d++ = src[j];
                     }
                     *d++ = 0;
                     t.key = YAC_LOSTKEY;
                     a->yacArraySet(NULL/*context*/, i, &v);
                  }
               }
               return;
            }
         }
         a->yacArrayAlloc(0, 0,0,0);
      }
   }
}



SQLite_Database::SQLite_Database(void) {
   db_handle       = NULL;
   first_statement = NULL;
}

SQLite_Database::~SQLite_Database() {
   SQLite_Database::close();
}

void SQLite_Database::yacGetConstantStringList(YAC_String *_s) {
   // Note: the /**/ comments are taken from sqlite3.h  
   _s->copy(
      "SQLITE_OK:0"           /* Successful result */
      " SQLITE_ERROR:1"       /* SQL error or missing database */
      " SQLITE_INTERNAL:2"    /* NOT USED. Internal logic error in SQLite */
      " SQLITE_PERM:3"        /* Access permission denied */
      " SQLITE_ABORT:4"       /* Callback routine requested an abort */
      " SQLITE_BUSY:5"        /* The database file is locked */
      " SQLITE_LOCKED:6"      /* A table in the database is locked */
      " SQLITE_NOMEM:7"       /* A malloc() failed */
      " SQLITE_READONLY:8"    /* Attempt to write a readonly database */ 
      " SQLITE_INTERRUPT:9"   /* Operation terminated by sqlite3_interrupt()*/
      " SQLITE_IOERR:10"      /* Some kind of disk I/O error occurred */
      " SQLITE_CORRUPT:11"    /* The database disk image is malformed */
      " SQLITE_NOTFOUND:12"   /* NOT USED. Table or record not found */
      " SQLITE_FULL:13"       /* Insertion failed because database is full */
      " SQLITE_CANTOPEN:14"   /* Unable to open the database file */
      " SQLITE_PROTOCOL:15"   /* Database lock protocol error */
      " SQLITE_EMPTY:16"      /* Database is empty */
      " SQLITE_SCHEMA:17"     /* The database schema changed */
      " SQLITE_TOOBIG:18"     /* NOT USED. Too much data for one row */
      " SQLITE_CONSTRAINT:19" /* Abort due to contraint violation */
      " SQLITE_MISMATCH:20"   /* Data type mismatch */
      " SQLITE_MISUSE:21"     /* Library used incorrectly */
      " SQLITE_NOLFS:22"      /* Uses OS features not supported on host */
      " SQLITE_AUTH:23"       /* Authorization denied */
      " SQLITE_FORMAT:24"     /* Auxiliary database format error */
      " SQLITE_RANGE:25"      /* 2nd parameter to sqlite3_bind out of range */
      " SQLITE_NOTADB:26"     /* File opened that is not a database file */
      " SQLITE_ROW:100"       /* sqlite3_step() has another row ready */
      " SQLITE_DONE:101"      /* sqlite3_step() has finished executing */
      " SQLITE_INTEGER:1"
      " SQLITE_FLOAT:2"
      " SQLITE_TEXT:3"
      " SQLITE_BLOB:4"
      " SQLITE_NULL:5"
      );
}

void SQLite_Database::close               (void) {
   // Free all prepare statements
   SQLite_Statement *st = first_statement;
   while(st != NULL)
   {
      SQLite_Statement *t = st;
      st = st->next;
      delete t;
   }

   // Close database
   if(db_handle != NULL)
   {
      sqlite3_close(db_handle);
   }
}

void SQLite_Database::prepareStatement    (YAC_Object *_String_sql, YAC_Value *_r) {
   
   _r->initObject(NULL, 0);

   if(YAC_VALID(_String_sql))
   {
      if(YAC_Is_String(_String_sql))
      {
         YAC_String *sql = (YAC_String*) _String_sql;

         if(db_handle != NULL)
         {
            const char *endOfSql = NULL;
            SQLite_Statement *st= (SQLite_Statement*) yac_host->yacNewByID(clid_SQLite_Statement);
            st->database = this;
            int r = sqlite3_prepare(db_handle, (const char*)sql->chars, (int)sql->length, &st->st_handle, &endOfSql);
            if(r == SQLITE_OK)
            {
               if(first_statement != NULL)
               {
                  first_statement->prev = st;
                  st->next = first_statement;
               }
               first_statement = st;

               _r->initObject(st, 0);
            }
            else
            {
               delete st;
            }
         }
      }
   }
}

sSI  SQLite_Database::getErrorCode        (void) {
   if(db_handle != NULL)
   {
      return sqlite3_errcode(db_handle);
   }
   else
   {
      return SQLITE_ERROR;
   }
}

void SQLite_Database::getErrorMessage     (YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   if(db_handle != NULL)
   {
      s->visit( (char*) sqlite3_errmsg(db_handle) );
   }
   else
   {
      s->visit("no open database");
   }
   _r->initString(s, 1);
}

sSI SQLite_Database::getTotalChanges(void) {
   if(db_handle != NULL)
   {
      int r = sqlite3_total_changes(db_handle); 
      return r;
   }
   return 0;
}

void SQLite_Database::getVersion          (YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit((char*) sqlite3_libversion());
   _r->initString(s, 1);
}

void SQLite_Database::interrupt(void) {
   if(db_handle != NULL)
   {
      sqlite3_interrupt(db_handle);
   }
}

sSI  SQLite_Database::isAutoCommitEnabled (void) {
   if(db_handle != NULL)
   {
      return sqlite3_get_autocommit(db_handle);
   }
   else
   {
      return 0;
   }
}

sSI  SQLite_Database::open                (YAC_Object *_String_filename) {
   SQLite_Database::close();

   if(YAC_VALID(_String_filename))
   {
      if(YAC_Is_String(_String_filename))
      {
         YAC_String *fileName = (YAC_String*) _String_filename;

         sSI r = sqlite3_open((const char*)fileName->chars, &db_handle);

         return r;
      }
   }
   return SQLITE_ERROR;
}

void SQLite_Database::setBusyTimeout      (sSI _ms) {
   if(db_handle != NULL)
   {
      sqlite3_busy_timeout(db_handle, _ms);
   }
}





YAC_APIC void YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	YAC_Init_sqlite(_host);
}

YAC_APIC void YAC_Exit(YAC_Host *_host) {
	YAC_Exit_sqlite(_host);
}

sUI YAC_Version(void) {
	return 0x00010000;
}

