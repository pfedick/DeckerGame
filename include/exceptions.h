#ifndef INCLUDE_EXCEPTIONS_H_
#define INCLUDE_EXCEPTIONS_H_

#ifndef DECKER_EXCEPTION
#define DECKER_EXCEPTION
#define STR_VALUE(arg)      #arg
#define EXCEPTION(name,inherit)	class name : public inherit { public: \
	name() throw() {}; \
	name(const char *msg, ...) throw() {  \
		va_list args; va_start(args, msg); copyText(msg,args); \
		va_end(args); } \
		const char* what() const overrides throw() { return (STR_VALUE(name)); } \
	};
#endif

EXCEPTION(AudioPoolNotInitialized, ppl7::Exception);

#endif // INCLUDE_EXCEPTIONS_H_
