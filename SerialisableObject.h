#ifndef SERIALISABLEOBJECT_H
#define SERIALISABLEOBJECT_H
class SerialisableObject {
	// dummy
	public:
	SerialisableObject(){};
	virtual ~SerialisableObject(){};
	virtual std::string GetVersion()=0;
	virtual bool Print()=0;
};
#endif
