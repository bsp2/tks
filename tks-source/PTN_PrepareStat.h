/// PTN_PrepareStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __PTN_PREPARESTAT_H__
#define __PTN_PREPARESTAT_H__


class PTN_PrepareStat : public PTN_Statement {
public:
    PTN_Statement *body;
    sBool          not_prepared;

public:
          PTN_PrepareStat  (PTN_Statement *_body);
          ~PTN_PrepareStat ();

	  void    eval            (PTN_Env *_env) const;
	  Fevalst getEvalSt       (void) const;
	  sBool   semanticCheck   (void);
	  void    optimize        (void);
	  void    subGenCallList  (void);
	  sBool   resolveXRef     (void);
	  sUI     getID           (void) const;
};


#endif // __PTN_PREPARESTAT_H__
