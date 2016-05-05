#pragma once

#include <map>
#include <set>
#include <wx/glCanvas.h>

class GLContextManager {
public:
	~GLContextManager();
	static GLContextManager &GetSingleton();

	void AddContext(int id, wxGLContext *context);
	void RemoveContext(int id, wxGLContext *context);
	void RemoveContexts(int id);
	void Clear();

	wxGLContext *GetMasterContext(int id) const;

private:
	GLContextManager();
	GLContextManager(GLContextManager &other) = delete;
	GLContextManager(GLContextManager &&other) = delete;
	GLContextManager &operator=(GLContextManager &other) = delete;
	GLContextManager &operator=(GLContextManager &&other) = delete;

	static GLContextManager _singleton;

	std::map<int, std::set<wxGLContext *>> _shareMap;
};