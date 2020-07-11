/************************************************************************************************
** File created by QxEntityEditor 1.2.5 (2020/07/07 22:14) : please, do NOT modify this file ! **
************************************************************************************************/

#include "../include/Licenta_precompiled_header.gen.h"

#include "../include/Camera.gen.h"
#include "../include/Violation.gen.h"

#include <QxOrm_Impl.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_LICENTA(Camera, Camera)

namespace qx {

template <>
void register_class(QxClass<Camera> & t)
{
   qx::IxDataMember * pData = NULL; Q_UNUSED(pData);
   qx::IxSqlRelation * pRelation = NULL; Q_UNUSED(pRelation);
   qx::IxFunction * pFct = NULL; Q_UNUSED(pFct);
   qx::IxValidator * pValidator = NULL; Q_UNUSED(pValidator);

   t.setName("t_camera");

   pData = t.id(& Camera::m_CameraId, "CameraId", 0);
   pData->setName("camera_id");

   pData = t.data(& Camera::m_Name, "Name", 0, true, true);
   pData->setName("name");
   pData = t.data(& Camera::m_Location, "Location", 0, true, true);
   pData->setName("location");

   pRelation = t.relationOneToMany(& Camera::m_ListOfViolation, "ListOfViolation", "Camera", 0);
   pRelation->getDataMember()->setName("list_of_violation");

   qx::QxValidatorX<Camera> * pAllValidator = t.getAllValidator(); Q_UNUSED(pAllValidator);
}

} // namespace qx

Camera::Camera() : m_CameraId(0) { ; }

Camera::Camera(const long & id) : m_CameraId(id) { ; }

Camera::~Camera() { ; }

long Camera::getCameraId() const { return m_CameraId; }

QString Camera::getName() const { return m_Name; }

QString Camera::getLocation() const { return m_Location; }

Camera::type_ListOfViolation Camera::getListOfViolation() const { return m_ListOfViolation; }

Camera::type_ListOfViolation & Camera::ListOfViolation() { return m_ListOfViolation; }

const Camera::type_ListOfViolation & Camera::ListOfViolation() const { return m_ListOfViolation; }

void Camera::setCameraId(const long & val) { m_CameraId = val; }

void Camera::setName(const QString & val) { m_Name = val; }

void Camera::setLocation(const QString & val) { m_Location = val; }

void Camera::setListOfViolation(const Camera::type_ListOfViolation & val) { m_ListOfViolation = val; }

Camera::type_ListOfViolation Camera::getListOfViolation(bool bLoadFromDatabase, const QString & sAppendRelations /* = QString() */, QSqlDatabase * pDatabase /* = NULL */, QSqlError * pDaoError /* = NULL */)
{
   if (pDaoError) { (* pDaoError) = QSqlError(); }
   if (! bLoadFromDatabase) { return getListOfViolation(); }
   QString sRelation = "{CameraId} | ListOfViolation";
   if (! sAppendRelations.isEmpty() && ! sAppendRelations.startsWith("->") && ! sAppendRelations.startsWith(">>")) { sRelation += "->" + sAppendRelations; }
   else if (! sAppendRelations.isEmpty()) { sRelation += sAppendRelations; }
   Camera tmp;
   tmp.m_CameraId = this->m_CameraId;
   QSqlError daoError = qx::dao::fetch_by_id_with_relation(sRelation, tmp, pDatabase);
   if (! daoError.isValid()) { this->m_ListOfViolation = tmp.m_ListOfViolation; }
   if (pDaoError) { (* pDaoError) = daoError; }
   return m_ListOfViolation;
}

Camera::type_ListOfViolation & Camera::ListOfViolation(bool bLoadFromDatabase, const QString & sAppendRelations /* = QString() */, QSqlDatabase * pDatabase /* = NULL */, QSqlError * pDaoError /* = NULL */)
{
   if (pDaoError) { (* pDaoError) = QSqlError(); }
   if (! bLoadFromDatabase) { return ListOfViolation(); }
   QString sRelation = "{CameraId} | ListOfViolation";
   if (! sAppendRelations.isEmpty() && ! sAppendRelations.startsWith("->") && ! sAppendRelations.startsWith(">>")) { sRelation += "->" + sAppendRelations; }
   else if (! sAppendRelations.isEmpty()) { sRelation += sAppendRelations; }
   Camera tmp;
   tmp.m_CameraId = this->m_CameraId;
   QSqlError daoError = qx::dao::fetch_by_id_with_relation(sRelation, tmp, pDatabase);
   if (! daoError.isValid()) { this->m_ListOfViolation = tmp.m_ListOfViolation; }
   if (pDaoError) { (* pDaoError) = daoError; }
   return m_ListOfViolation;
}

