/*
 * This is an automatically generated file using a python script.
 * ($EFL_HOME/src/utils/evas/generate_gl_thread_api.py)
 * Recommend that you modify data files ($EFL_HOME/src/utils/evas/gl_api_def.txt)
 * and make use of scripts if you need to fix them.
 */
extern GLenum (*evas_glGetError_evgl_th)(void);
extern void (*evas_glVertexAttribPointer_evgl_th)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
extern void (*evas_glEnableVertexAttribArray_evgl_th)(GLuint index);
extern void (*evas_glDisableVertexAttribArray_evgl_th)(GLuint index);
extern void (*evas_glDrawArrays_evgl_th)(GLenum mode, GLint first, GLsizei count);
extern void (*evas_glDrawElements_evgl_th)(GLenum mode, GLsizei count, GLenum type, const void *indices);
extern void (*evas_glGenBuffers_evgl_th)(GLsizei n, GLuint *buffers);
extern void (*evas_glDeleteBuffers_evgl_th)(GLsizei n, const GLuint *buffers);
extern void (*evas_glBindBuffer_evgl_th)(GLenum target, GLuint buffer);
extern void (*evas_glBufferData_evgl_th)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
extern GLuint (*evas_glCreateShader_evgl_th)(GLenum type);
extern void (*evas_glShaderSource_evgl_th)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
extern void (*evas_glCompileShader_evgl_th)(GLuint shader);
extern void (*evas_glDeleteShader_evgl_th)(GLuint shader);
extern GLuint (*evas_glCreateProgram_evgl_th)(void);
extern void (*evas_glAttachShader_evgl_th)(GLuint program, GLuint shader);
extern void (*evas_glDetachShader_evgl_th)(GLuint program, GLuint shader);
extern void (*evas_glLinkProgram_evgl_th)(GLuint program);
extern void (*evas_glUseProgram_evgl_th)(GLuint program);
extern void (*evas_glDeleteProgram_evgl_th)(GLuint program);
extern void (*glGetProgramBinary_orig_evgl_set)(void *func);
extern void *(*glGetProgramBinary_orig_evgl_get)(void);
extern void (*evas_glGetProgramBinary_evgl_th)(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
extern void (*glProgramBinary_orig_evgl_set)(void *func);
extern void *(*glProgramBinary_orig_evgl_get)(void);
extern void (*evas_glProgramBinary_evgl_th)(GLuint program, GLenum binaryFormat, const void *binary, GLint length);
extern void (*evas_glGetActiveAttrib_evgl_th)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
extern GLint (*evas_glGetAttribLocation_evgl_th)(GLuint program, const GLchar *name);
extern void (*evas_glBindAttribLocation_evgl_th)(GLuint program, GLuint index, const GLchar *name);
extern GLint (*evas_glGetUniformLocation_evgl_th)(GLuint program, const GLchar *name);
extern void (*evas_glUniform1f_evgl_th)(GLint location, GLfloat v0);
extern void (*evas_glUniform1i_evgl_th)(GLint location, GLint v0);
extern void (*evas_glUniform2f_evgl_th)(GLint location, GLfloat v0, GLfloat v1);
extern void (*evas_glUniform2i_evgl_th)(GLint location, GLint v0, GLint v1);
extern void (*evas_glUniform3f_evgl_th)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern void (*evas_glUniform3i_evgl_th)(GLint location, GLint v0, GLint v1, GLint v2);
extern void (*evas_glUniform4f_evgl_th)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
extern void (*evas_glUniform4i_evgl_th)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
extern void (*evas_glUniform1fv_evgl_th)(GLint location, GLsizei count, const GLfloat *value);
extern void (*evas_glUniform1iv_evgl_th)(GLint location, GLsizei count, const GLint *value);
extern void (*evas_glUniform2fv_evgl_th)(GLint location, GLsizei count, const GLfloat *value);
extern void (*evas_glUniform2iv_evgl_th)(GLint location, GLsizei count, const GLint *value);
extern void (*evas_glUniform3fv_evgl_th)(GLint location, GLsizei count, const GLfloat *value);
extern void (*evas_glUniform3iv_evgl_th)(GLint location, GLsizei count, const GLint *value);
extern void (*evas_glUniform4fv_evgl_th)(GLint location, GLsizei count, const GLfloat *value);
extern void (*evas_glUniform4iv_evgl_th)(GLint location, GLsizei count, const GLint *value);
extern void (*evas_glUniformMatrix2fv_evgl_th)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (*evas_glUniformMatrix3fv_evgl_th)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (*evas_glUniformMatrix4fv_evgl_th)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (*evas_glViewport_evgl_th)(GLint x, GLint y, GLsizei width, GLsizei height);
extern void (*evas_glEnable_evgl_th)(GLenum cap);
extern void (*evas_glDisable_evgl_th)(GLenum cap);
extern void (*evas_glLineWidth_evgl_th)(GLfloat width);
extern void (*evas_glPolygonOffset_evgl_th)(GLfloat factor, GLfloat units);
extern void (*evas_glPixelStorei_evgl_th)(GLenum pname, GLint param);
extern void (*evas_glActiveTexture_evgl_th)(GLenum texture);
extern void (*evas_glGenTextures_evgl_th)(GLsizei n, GLuint *textures);
extern void (*evas_glBindTexture_evgl_th)(GLenum target, GLuint texture);
extern void (*evas_glDeleteTextures_evgl_th)(GLsizei n, const GLuint *textures);
extern void (*evas_glTexImage2D_evgl_th)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
extern void (*evas_glTexSubImage2D_evgl_th)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
extern void (*evas_glCompressedTexImage2D_evgl_th)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
extern void (*evas_glCompressedTexSubImage2D_evgl_th)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
extern void (*evas_glTexParameterf_evgl_th)(GLenum target, GLenum pname, GLfloat param);
extern void (*evas_glTexParameterfv_evgl_th)(GLenum target, GLenum pname, const GLfloat *params);
extern void (*evas_glTexParameteri_evgl_th)(GLenum target, GLenum pname, GLint param);
extern void (*evas_glTexParameteriv_evgl_th)(GLenum target, GLenum pname, const GLint *params);
extern void (*evas_glScissor_evgl_th)(GLint x, GLint y, GLsizei width, GLsizei height);
extern void (*evas_glBlendFunc_evgl_th)(GLenum sfactor, GLenum dfactor);
extern void (*evas_glBlendColor_evgl_th)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void (*evas_glDepthMask_evgl_th)(GLboolean flag);
extern void (*evas_glClear_evgl_th)(GLbitfield mask);
extern void (*evas_glClearColor_evgl_th)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void (*evas_glReadPixels_evgl_th)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
extern void (*evas_glGenFramebuffers_evgl_th)(GLsizei n, GLuint *framebuffers);
extern void (*evas_glBindFramebuffer_evgl_th)(GLenum target, GLuint framebuffer);
extern void (*evas_glDeleteFramebuffers_evgl_th)(GLsizei n, const GLuint *framebuffers);
extern void (*evas_glGenRenderbuffers_evgl_th)(GLsizei n, GLuint *renderbuffers);
extern void (*evas_glBindRenderbuffer_evgl_th)(GLenum target, GLuint renderbuffer);
extern void (*evas_glDeleteRenderbuffers_evgl_th)(GLsizei n, const GLuint *renderbuffers);
extern void (*evas_glRenderbufferStorage_evgl_th)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
extern void (*evas_glFramebufferRenderbuffer_evgl_th)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
extern void (*evas_glFramebufferTexture2D_evgl_th)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern GLenum (*evas_glCheckFramebufferStatus_evgl_th)(GLenum target);
extern void (*evas_glFlush_evgl_th)(void);
extern void (*evas_glFinish_evgl_th)(void);
extern void (*evas_glHint_evgl_th)(GLenum target, GLenum mode);
extern const GLubyte * (*evas_glGetString_evgl_th)(GLenum name);
extern void (*evas_glGetBooleanv_evgl_th)(GLenum pname, GLboolean *data);
extern void (*evas_glGetFloatv_evgl_th)(GLenum pname, GLfloat *data);
extern void (*evas_glGetIntegerv_evgl_th)(GLenum pname, GLint *data);
extern GLboolean (*evas_glIsBuffer_evgl_th)(GLint buffer);
extern void (*evas_glGetBufferParameteriv_evgl_th)(GLenum target, GLenum pname, GLint *params);
extern GLboolean (*evas_glIsShader_evgl_th)(GLuint shader);
extern void (*evas_glGetShaderiv_evgl_th)(GLuint shader, GLenum pname, GLint *params);
extern void (*evas_glGetAttachedShaders_evgl_th)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
extern void (*evas_glGetShaderInfoLog_evgl_th)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
extern void (*evas_glGetShaderSource_evgl_th)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
extern void (*evas_glGetShaderPrecisionFormat_evgl_th)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
extern void (*evas_glGetVertexAttribfv_evgl_th)(GLuint index, GLenum pname, GLfloat *params);
extern void (*evas_glGetVertexAttribiv_evgl_th)(GLuint index, GLenum pname, GLint *params);
extern GLboolean (*evas_glIsProgram_evgl_th)(GLuint program);
extern void (*evas_glGetProgramInfoLog_evgl_th)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
extern void (*evas_glGetProgramiv_evgl_th)(GLuint program, GLenum pname, GLint *params);
extern GLboolean (*evas_glIsFramebuffer_evgl_th)(GLint framebuffer);
extern GLboolean (*evas_glIsRenderbuffer_evgl_th)(GLint renderbuffer);
extern void (*evas_glGetRenderbufferParameteriv_evgl_th)(GLenum target, GLenum pname, GLint *params);
extern GLboolean (*evas_glIsTexture_evgl_th)(GLint texture);
extern void (*evas_glClearDepthf_evgl_th)(GLclampf depth);
extern void (*evas_glDepthRangef_evgl_th)(GLclampf zNear, GLclampf zFar);
extern void (*evas_glClearDepth_evgl_th)(GLclampf depth);
extern void (*evas_glDepthRange_evgl_th)(GLclampf zNear, GLclampf zFar);
extern void (*evas_glGetFramebufferAttachmentParameteriv_evgl_th)(GLenum target, GLenum attachment, GLenum pname, GLint* params);


extern void _gl_thread_link_evgl_generated_init();
