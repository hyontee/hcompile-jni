/**
 * sctool_jni.cpp v2
 * Пакет задаётся через -DSCTOOL_JNI_CLASS в Android.mk
 */
#include <jni.h>
#include <android/log.h>
#include <string>
#include "sc_core.h"

static std::string jstr(JNIEnv* e, jstring s) {
    if (!s) return "";
    const char* c = e->GetStringUTFChars(s, nullptr);
    std::string r(c); e->ReleaseStringUTFChars(s, c);
    return r;
}
static jstring toj(JNIEnv* e, const std::string& s) { return e->NewStringUTF(s.c_str()); }

#ifndef SCTOOL_JNI_CLASS
#define SCTOOL_JNI_CLASS Java_com_compose_sctool_ScProcessor
#endif
#define _CAT(a,b) a##b
#define CAT(a,b) _CAT(a,b)
#define FN(name) CAT(SCTOOL_JNI_CLASS, name)

// Forward declarations
std::string sc_info      (const std::string&);
std::string sc_split     (const std::string&, const std::string&);
std::string sc_join      (const std::string&, const std::string&, const std::string&);
std::string sc_extract_ktx(const std::string&, int, const std::string&);
std::string sc_downgrade (const std::string&, const std::string&);
std::string ktx2png      (const std::string&, const std::string&);
std::string png2ktx      (const std::string&, const std::string&, const std::string&);
std::string png2sc       (const std::string&, const std::string&, int, int, const std::string&);
std::string sctx2png     (const std::string&, const std::string&);
std::string png2sctx     (const std::string&, const std::string&);
std::string cut_sprites  (const std::string&, const std::string&, const std::string&);
std::string sc2png       (const std::string&, int, const std::string&);
std::string sc_exports   (const std::string&, const std::string&);
std::string inject_preview(const std::string&, const std::string&);
std::string inject_put   (const std::string&, const std::string&, const std::string&, const std::string&);
std::string inject_list  (const std::string&);
std::string anim2static  (const std::string&, const std::string&);
std::string build_sc     (const std::string&, const std::string&, const std::string&, const std::string&);
std::string mc_copy      (const std::string&, const std::string&, const std::string&, const std::string&);

extern "C" {

JNIEXPORT jstring JNICALL FN(_nativeScInfo)(JNIEnv* e, jclass, jstring a)
    { return toj(e, sc_info(jstr(e,a))); }

JNIEXPORT jstring JNICALL FN(_nativeSplitSc)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, sc_split(jstr(e,a), jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativeJoinSc)(JNIEnv* e, jclass, jstring a, jstring b, jstring c)
    { return toj(e, sc_join(jstr(e,a), jstr(e,b), jstr(e,c))); }

JNIEXPORT jstring JNICALL FN(_nativeExtractKtx)(JNIEnv* e, jclass, jstring a, jint idx, jstring b)
    { return toj(e, sc_extract_ktx(jstr(e,a), (int)idx, jstr(e,b))); }

// nativeDowngrade и nativeDowngradeV реализованы в downgrade_jni.cpp

JNIEXPORT jstring JNICALL FN(_nativeKtx2Png)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, ktx2png(jstr(e,a), jstr(e,b))); }

// block теперь передаётся как строка "8x8"
JNIEXPORT jstring JNICALL FN(_nativePng2Ktx)(JNIEnv* e, jclass, jstring png, jstring block, jstring out)
    { return toj(e, png2ktx(jstr(e,png), jstr(e,block), jstr(e,out))); }

JNIEXPORT jstring JNICALL FN(_nativePng2Sc)(JNIEnv* e, jclass, jstring png, jstring orig, jint idx, jstring rot, jstring out)
    { int r=0; try{r=std::stoi(jstr(e,rot));}catch(...){} return toj(e, png2sc(jstr(e,png), jstr(e,orig), (int)idx, r, jstr(e,out))); }

JNIEXPORT jstring JNICALL FN(_nativeSctx2Png)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, sctx2png(jstr(e,a), jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativePng2Sctx)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, png2sctx(jstr(e,a), jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativeCutSprites)(JNIEnv* e, jclass, jstring a, jstring b, jstring c)
    { return toj(e, cut_sprites(jstr(e,a), jstr(e,b), jstr(e,c))); }

JNIEXPORT jstring JNICALL FN(_nativeSc2Png)(JNIEnv* e, jclass, jstring a, jint idx, jstring b)
    { return toj(e, sc2png(jstr(e,a), (int)idx, jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativeExports)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, sc_exports(jstr(e,a), jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativeInjectPreview)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, inject_preview(jstr(e,a), jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativeInjectPut)(JNIEnv* e, jclass, jstring a, jstring b, jstring name, jstring out)
    { return toj(e, inject_put(jstr(e,a), jstr(e,b), jstr(e,name), jstr(e,out))); }

JNIEXPORT jstring JNICALL FN(_nativeInjectList)(JNIEnv* e, jclass, jstring a)
    { return toj(e, inject_list(jstr(e,a))); }

JNIEXPORT jstring JNICALL FN(_nativeAnim2Static)(JNIEnv* e, jclass, jstring a, jstring b)
    { return toj(e, anim2static(jstr(e,a), jstr(e,b))); }

JNIEXPORT jstring JNICALL FN(_nativeBuildSc)(JNIEnv* e, jclass, jstring paths, jstring names, jstring block, jstring out)
    { return toj(e, build_sc(jstr(e,paths), jstr(e,names), jstr(e,block), jstr(e,out))); }

JNIEXPORT jstring JNICALL FN(_nativeMcCopy)(JNIEnv* e, jclass, jstring src, jstring dst, jstring names, jstring out)
    { return toj(e, mc_copy(jstr(e,src), jstr(e,dst), jstr(e,names), jstr(e,out))); }

} // extern "C"
