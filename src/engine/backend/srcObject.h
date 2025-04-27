#ifndef __SRC_OBJECT_H__
#define __SRC_OBJECT_H__

//********************************************************************************************
//*                                     Defines                                              *
//********************************************************************************************

class BACKEND_API IMetaObjectSourceData;

//********************************************************************************************
//*								      ISourceObject											 *
//********************************************************************************************

class ISourceObject {
public:
	
	//get metaData from object 
	virtual IMetaObjectSourceData* GetSourceMetaObject() const = 0;
	
	//Get ref class 
	virtual class_identifier_t GetSourceClassType() const = 0;
};

//********************************************************************************************
//*								      ITabularObject										 *
//********************************************************************************************

class ITabularObject {
public:

	//get metaData from object 
	virtual IMetaObjectSourceData* GetSourceMetaObject() const = 0;

	//Get ref class 
	virtual class_identifier_t GetSourceClassType() const = 0;
};

#endif 