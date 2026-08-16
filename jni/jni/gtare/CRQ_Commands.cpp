#include "CRQ_Commands.h"
#include "util/patch.h"
#include <GLES2/gl2.h>
void(*rqVertexBufferSelect)(unsigned int **result);
void rqVertexBufferSelect_HOOK(unsigned int **result)
{
	unsigned int buffer = *(*result)++;

	if (!buffer)
		return glBindBuffer(0x8892, 0);

	glBindBuffer(0x8892, buffer + 8);
	*(uint32_t*)(SA_ADDR(0x617234)) = 0;
}
void(*rqVertexBufferDelete)(int result);
void rqVertexBufferDelete_HOOK(int result)
{
	int v2;
	bool v3;

	int buffer = **(uint32_t**)result;
	if (buffer)
	{
		*(uint32_t *)result += 4;
		glDeleteBuffers(1, reinterpret_cast<const GLuint *>(buffer + 8));

		result = *(uint32_t *)buffer;
		*(uint32_t *)(buffer + 8) = 0;
		v3 = result == 0;

		if (result)
		{
			v3 = v2 == 0;
		}

		if (!v3)
			result = v2;
	}
}
void SetUpCRQHook(){
    CHook::InlineHook(g_libGTASA, 0x1A1F6C, &rqVertexBufferDelete_HOOK, &rqVertexBufferDelete);
    CHook::InlineHook(g_libGTASA, 0x1A1ED8, &rqVertexBufferSelect_HOOK, &rqVertexBufferSelect);
}
