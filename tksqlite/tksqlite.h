/// tksqlite.h
///
/// (c) 2006 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL).
///
///

#ifndef __TKSQLITE_H__
#define __TKSQLITE_H__

YG("sqlite");

class SQLite_Database;


YC class SQLite_Statement : public YAC_Object {
public:
   sqlite3_stmt     *st_handle;
   SQLite_Database  *database;
   SQLite_Statement *prev;
   SQLite_Statement *next;

   sSI b_have_row;

public:
   YAC(SQLite_Statement);

   SQLite_Statement(void);
   ~SQLite_Statement();

public:
  YM void        clearParameters        (void);
  YM void        getHashTableResult     (YAC_Value *_r);
  YM void        getValueArrayResult    (YAC_Value *_r);
  YM void        getStringArrayResult   (YAC_Value *_r);
  YM YAC_Object *getDatabase            (void);
  YM sSI         getColumnCount         (void);
  YM void        getColumnDeclType      (sSI _columnIndex, YAC_Value *_r);
  YM void        getColumnName          (sSI _columnIndex, YAC_Value *_r);
  YM sSI         getColumnType          (sSI _columnIndex);
  YM void        getColumnValue         (sSI _columnIndex, YAC_Value *_r);
  YM sSI         getParameterCount      (void);
  YM void        getParameterName       (sSI _index, YAC_Value *_r);
  YM void        reset                  (void);
  YM void        setBlob                (sSI _index, YAC_Object *_Buffer_blob);
  YM void        setBlobByName          (YAC_Object *_String_name, YAC_Object *_Buffer_blob);
  YM void        setDouble              (sSI _index, YAC_Object *_Double_value);
  YM void        setDoubleByName        (YAC_Object *_String_name, YAC_Object *_Double_value);
  YM void        setFloat               (sSI _index, sF32 _value);
  YM void        setFloatByName         (YAC_Object *_String_name, sF32 _value);
  YM void        setInt                 (sSI _index, sSI _value);
  YM void        setIntByName           (YAC_Object *_String_name, sSI _value);
  YM void        setLong                (sSI _index, YAC_Object *_Long_value);
  YM void        setLongByName          (YAC_Object *_String_name, YAC_Object *_Long_value);
  YM void        setNull                (sSI _index) ;
  YM void        setNullByName          (YAC_Object *_String_name) ;
  YM void        setText                (sSI _index, YAC_Object *_String_text);
  YM void        setTextByName          (YAC_Object *_String_name, YAC_Object *_String_text);
  YM sSI         step                   (void);
  YM void        storeHashTableResult   (YAC_Object *_HashTable_ret);
  YM void        storeValueArrayResult  (YAC_Object *_ValueArray_ret);
  YM void        storeStringArrayResult (YAC_Object *_StringArray_ret);
};

////  YM sSI         hasExpired             (void);


YC class SQLite_Database : public YAC_Object {
public:
   sqlite3          *db_handle;
   SQLite_Statement *first_statement;

public:
   YAC(SQLite_Database);

   SQLite_Database(void);
   ~SQLite_Database();

public:
   void yacGetConstantStringList(YAC_String *); 

public:
   YM void close               (void);
   YM void prepareStatement    (YAC_Object *_String_sql, YAC_Value *_r);
   YM sSI  getErrorCode        (void);
   YM void getErrorMessage     (YAC_Value *_r);
   YM sSI  getTotalChanges     (void);
   YM void getVersion          (YAC_Value *_r);
   YM void interrupt           (void);
   YM sSI  isAutoCommitEnabled (void);
   YM sSI  open                (YAC_Object *_String_filename);
   YM void setBusyTimeout      (sSI _ms);
};


#endif
