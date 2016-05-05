
#include "GLContextManager.h"

GLContextManager GLContextManager::_singleton;


GLContextManager::GLContextManager()
{

}

GLContextManager::~GLContextManager()
{

}

GLContextManager & GLContextManager::GetSingleton()
{
	return _singleton;
}

void GLContextManager::AddContext(int id, wxGLContext *context)
{
	assert(id >= 0);
	auto &set = _shareMap[id];
	set.insert(context);
}

void GLContextManager::RemoveContext(int id, wxGLContext *context)
{
	auto &set = _shareMap.find(id);
	if (set != _shareMap.end()) {
		set->second.erase(context);
	}
}

void GLContextManager::RemoveContexts(int id)
{
	_shareMap.erase(id);
}

void GLContextManager::Clear()
{
	_shareMap.clear();
}

wxGLContext * GLContextManager::GetMasterContext(int id) const
{
	auto &set = _shareMap.find(id);
	if (set != _shareMap.end()) {
		auto &context = set->second.begin();
		if (context != set->second.end()) {
			return *context;
		}
	}
	return nullptr;
}
