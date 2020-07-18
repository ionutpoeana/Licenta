/************************************************************************************************
** File created by QxEntityEditor 1.2.5 (2020/07/15 00:46) : please, do NOT modify this file ! **
************************************************************************************************/

#include "../include/Licenta_precompiled_header.gen.h"

#include "../include/Violation.gen.h"
#include "../include/Camera.gen.h"

#include <QxOrm_Impl.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_LICENTA(Violation, Violation)

namespace qx {

template <>
void register_class(QxClass<Violation> & t)
{
   qx::IxDataMember * pData = NULL; Q_UNUSED(pData);
   qx::IxSqlRelation * pRelation = NULL; Q_UNUSED(pRelation);
   qx::IxFunction * pFct = NULL; Q_UNUSED(pFct);
   qx::IxValidator * pValidator = NULL; Q_UNUSED(pValidator);

   t.setName("t_violation");

   pData = t.id(& Violation::m_ViolationId, "ViolationId", 0);
   pData->setName("violation_id");

   pData = t.data(& Violation::m_Time, "Time", 0, true, true);
   pData->setName("time");
   pData = t.data(& Violation::m_VideoLocation, "VideoLocation", 0, true, true);
   pData->setName("video_location");
   pData = t.data(& Violation::m_PhotoLocation, "PhotoLocation", 0, true, true);
   pData->setName("photo_location");
   pData = t.data(& Violation::m_RuleType, "RuleType", 0, true, true);
   pData->setName("rule_type");

   pRelation = t.relationManyToOne(& Violation::m_Camera, "Camera", 0);
   pRelation->getDataMember()->setName("camera");

   qx::QxValidatorX<Violation> * pAllValidator = t.getAllValidator(); Q_UNUSED(pAllValidator);
}

} // namespace qx

Violation::Violation() : m_RuleType(0) { ; }

Violation::Violation(const QUuid & id) : m_ViolationId(id), m_RuleType(0) { ; }

Violation::~Violation() { ; }

QUuid Violation::getViolationId() const { return m_ViolationId; }

QDateTime Violation::getTime() const { return m_Time; }

QString Violation::getVideoLocation() const { return m_VideoLocation; }

QString Violation::getPhotoLocation() const { return m_PhotoLocation; }

int Violation::getRuleType() const { return m_RuleType; }

Violation::type_Camera Violation::getCamera() const { return m_Camera; }

void Violation::setViolationId(const QUuid & val) { m_ViolationId = val; }

void Violation::setTime(const QDateTime & val) { m_Time = val; }

void Violation::setVideoLocation(const QString & val) { m_VideoLocation = val; }

void Violation::setPhotoLocation(const QString & val) { m_PhotoLocation = val; }

void Violation::setRuleType(const int & val) { m_RuleType = val; }

void Violation::setCamera(const Violation::type_Camera & val) { m_Camera = val; }

Violation::type_Camera Violation::getCamera(bool bLoadFromDatabase, const QString & sAppendRelations /* = QString() */, QSqlDatabase * pDatabase /* = NULL */, QSqlError * pDaoError /* = NULL */)
{
   if (pDaoError) { (* pDaoError) = QSqlError(); }
   if (! bLoadFromDatabase) { return getCamera(); }
   QString sRelation = "{ViolationId} | Camera";
   if (! sAppendRelations.isEmpty() && ! sAppendRelations.startsWith("->") && ! sAppendRelations.startsWith(">>")) { sRelation += "->" + sAppendRelations; }
   else if (! sAppendRelations.isEmpty()) { sRelation += sAppendRelations; }
   Violation tmp;
   tmp.m_ViolationId = this->m_ViolationId;
   QSqlError daoError = qx::dao::fetch_by_id_with_relation(sRelation, tmp, pDatabase);
   if (! daoError.isValid()) { this->m_Camera = tmp.m_Camera; }
   if (pDaoError) { (* pDaoError) = daoError; }
   return m_Camera;
}

