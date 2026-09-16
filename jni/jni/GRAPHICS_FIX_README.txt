DAG STYLE graphics port - build fixes

Applied fixes based on JNI_error_log (1).txt:

1. game/RenderWare.cpp
   - fixed #include path from ../../main.h to ../main.h

2. game/Coronas.h
   - uses RW/RenderWare.h instead of including rwcore.h directly, so the RW primitive types are defined first.

3. game/Coronas.cpp
   - removed dependency on missing ../util/patch.h and CHook.
   - replaced the two CallFunction calls with direct function-pointer calls using the same game offsets.
   - replaced CHook::Write with DAG STYLE WriteMemory.
   - replaced CHook::CallFunction in Render() with a direct function-pointer call.

4. game/RegisteredCorona.cpp
   - replaced std::clamp with C++14-compatible manual clamping for NDK r16b.

5. game/CRenderTarget.cpp
   - fixed include typo RW/rwcore.h. -> RW/rwcore.h

6. Android.mk
   - removed the explicit game/RW/RenderWare.cpp entry because game/*.cpp already includes game/RenderWare.cpp and compiling both implementations would create duplicate RenderWare symbols.

CustomCarEnvMapPipeline is not added to the build because its current source still depends on CHook and mangled-symbol helpers that are not provided by DAG STYLE's armhook API. It should be adapted separately rather than enabled blindly.
