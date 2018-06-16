
void QWidget::setWhatsThis(const QString &)

whatsThis : QString
This property holds the widget's What's This help text.
By default, this property contains an empty string.
Access functions:

QString whatsThis() const
void setWhatsThis(const QString &)

See also QWhatsThis, QWidget::toolTip, and QWidget::statusTip.

////////////////////////////////////////////////////////////////
int qRegisterMetaType(const char *typeName)
Registers the type name typeName for the type T. Returns the internal ID used by QMetaType. Any class or struct that has a public default constructor, a public copy constructor and a public destructor can be registered.
This function requires that T is a fully defined type at the point where the function is called. For pointer types, it also requires that the pointed to type is fully defined. Use Q_DECLARE_OPAQUE_POINTER() to be able to register pointers to forward declared types.
After a type has been registered, you can create and destroy objects of that type dynamically at run-time.
This example registers the class MyClass:

  qRegisterMetaType<MyClass>("MyClass");

This function is useful to register typedefs so they can be used by QMetaProperty, or in QueuedConnections

  typedef QString CustomString;
  qRegisterMetaType<CustomString>("CustomString");

Warning: This function is useful only for registering an alias (typedef) for every other use case Q_DECLARE_METATYPE and qMetaTypeId() should be used instead.
Note: This function is thread-safe
See also qRegisterMetaTypeStreamOperators(), isRegistered(), and Q_DECLARE_METATYPE().

/////////////////////////////////////////////////////////////////
signals:
	void signalType(QString);
	
	qRegisterMetaType<MyClass>("MyClass");
	connetc(action,signalType("123"),this,slotMyself());
	
	void XXX::signalType(QString)
	{
	}
	
///////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <windows.h>

HANDLE hMutex = NULL;				//信号量

DWORD WINAPI fun(LPVOID lpParamter)
{
	for (int i=0; i<10; ++i)
	{
		WaitForSingleObject(hMutex, INFINITE);
		std::cout << "A Thread Display" << std::endl;
		ReleaseMutex(hMutex);
	}
	return 0L;
}

int main()
{
	HANDLE hThread = CreateThread(NULL, 0, fun, NULL, 0, NULL);
	hMutex = CreateMutex(NULL, FALSE, L"screen");
	CloseHandle(hThread);
	for (int i=0; i<10; ++i)
	{
		WaitForSingleObject(hMutex, INFINITE);
		std::cout << "Main Thread Display" << std::endl;
		ReleaseMutex(hMutex);
	}
	return 0;
}