
#include <emuall/graphics/font.h>

FontException::FontException(int errorCode, const std::string &msg) :
	BaseException(msg)
{
	_msg->append("(");
	_msg->append(GetErrorMessage(errorCode));
	_msg->append(")");
}

FontException::FontException(int errorCode) : 
	BaseException(GetErrorMessage(errorCode))
{

}

FontException::FontException(const std::string &msg) :
	BaseException(msg)
{
}

FontException::~FontException()
{

}

const char * FontException::GetErrorMessage(int errorCode)
{
	#undef __FTERRORS_H__
	#define FT_ERRORDEF( e, v, s )  case e: return s;
	#define FT_ERROR_START_LIST     switch(errorCode) {
	#define FT_ERROR_END_LIST       }
	#include FT_ERRORS_H
	return "Unknown error";
}
