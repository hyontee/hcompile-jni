DAG JNI - clean graphics integration

This archive is based on the uploaded DAG JNI project.

Changes:
- Removed incompatible JNJ duplicate jni/game/RenderWare.cpp and RenderWare.h.
- Kept DAG's jni/game/RW/RenderWare.cpp and RenderWare.h.
- Coronas now includes DAG RW/RenderWare.h instead of bare rwcore.h.
- Removed JNJ patch.h / CHook dependency from Coronas.cpp.
- Replaced std::clamp in RegisteredCorona.cpp for NDK r16b compatibility.
- Fixed CRenderTarget.cpp include typo.

CustomCarEnvMapPipeline is intentionally NOT added to Android.mk yet because its implementation still depends on JNJ-specific CHook/RxPipeline integration. It should be adapted separately rather than copied blindly.

Build the project first. This package is intended to get the DAG source tree back to a clean, compatible compilation baseline before enabling the custom-car EnvMap pipeline.
