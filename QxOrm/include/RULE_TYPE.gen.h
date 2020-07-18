/************************************************************************************************
** File created by QxEntityEditor 1.2.5 (2020/07/13 22:51) : please, do NOT modify this file ! **
************************************************************************************************/

#ifndef _LICENTA_RULE_TYPE_H_
#define _LICENTA_RULE_TYPE_H_

#ifdef _QX_NO_PRECOMPILED_HEADER
#ifndef Q_MOC_RUN
#include "../include/Licenta_precompiled_header.gen.h" // Need to include precompiled header for the generated moc file
#endif // Q_MOC_RUN
#endif // _QX_NO_PRECOMPILED_HEADER

class LICENTA_EXPORT RULE_TYPE : public QObject
{

   Q_OBJECT
   Q_ENUMS(enum_RULE_TYPE)

public:

   enum enum_RULE_TYPE
   {
      SEMAPHORE = 1,
      FORBIDDEN_FORWARD = 2,
      FORBIDDEN_ON_THE_LEFT = 3,
      FORBIDDEN_FORWARD_OR_ON_THE_LEFT = 4,
      FORBIDDEN_ON_THE_RIGHT = 5,
      FORBIDDEN_FORWARD_OR_ON_THE_RIGHT = 6
   };

private:

   RULE_TYPE();
   virtual ~RULE_TYPE();

};

#include "../custom/include/RULE_TYPE.h"

#endif // _LICENTA_RULE_TYPE_H_
