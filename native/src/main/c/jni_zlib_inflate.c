#include <assert.h>
#include <jni.h>
#include <stdbool.h>
#include <stdlib.h>
#include <libdeflate.h>
#include "jni_zlib_inflate.h"
#include "jni_util.h"

/*
 * Class:     com_velocitypowered_natives_compression_NativeZlibInflate
 * Method:    init
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_velocitypowered_natives_compression_NativeZlibInflate_init(JNIEnv *env, jclass clazz) {
    struct libdeflate_decompressor *decompress = libdeflate_alloc_decompressor();
    if (decompress == NULL) {
        throwException(env, "java/lang/OutOfMemoryError", "libdeflate allocate decompressor failed");
        return 0;
    }
    return (jlong) decompress;
}

/*
 * Class:     com_velocitypowered_natives_compression_NativeZlibInflate
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_velocitypowered_natives_compression_NativeZlibInflate_free(JNIEnv *env, jclass clazz, jlong ctx) {
    if (ctx != 0) {
        struct libdeflate_decompressor *decompress = (struct libdeflate_decompressor *) ctx;
        libdeflate_free_decompressor(decompress);
    }
}

/*
 * Class:     com_velocitypowered_natives_compression_NativeZlibInflate
 * Method:    process
 * Signature: (JJIJI)Z
 */
JNIEXPORT jboolean JNICALL Java_com_velocitypowered_natives_compression_NativeZlibInflate_process(
    JNIEnv *env,
    jclass clazz,
    jlong ctx,
    jlong sourceAddress,
    jint sourceLength,
    jlong destinationAddress,
    jint destinationLength) {

    if (ctx == 0) {
        throwException(env, "java/lang/IllegalStateException", "Decompressor context is null");
        return JNI_FALSE;
    }

    if (sourceAddress == 0 || destinationAddress == 0) {
        throwException(env, "java/lang/IllegalArgumentException", "Source or destination address is null");
        return JNI_FALSE;
    }

    if (sourceLength <= 0 || destinationLength <= 0) {
        throwException(env, "java/lang/IllegalArgumentException", "Invalid buffer length");
        return JNI_FALSE;
    }

    struct libdeflate_decompressor *decompress = (struct libdeflate_decompressor *) ctx;
    size_t actual_out_size;

    enum libdeflate_result result = libdeflate_zlib_decompress(
        decompress,
        (void *) sourceAddress,
        (size_t) sourceLength,
        (void *) destinationAddress,
        (size_t) destinationLength,
        &actual_out_size
    );

    switch (result) {
        case LIBDEFLATE_SUCCESS:
            return JNI_TRUE;

        case LIBDEFLATE_BAD_DATA:
            throwException(env, "java/util/zip/DataFormatException",
                         "Invalid or corrupted compressed data");
            return JNI_FALSE;

        case LIBDEFLATE_SHORT_OUTPUT:
            char error_message_a[256];
            snprintf(error_message_a, sizeof(error_message_a),
                     "Output buffer is too small (actual size needed: %zu)",
                     actual_out_size);
            throwException(env, "java/util/zip/DataFormatException", error_message_a);
            return JNI_FALSE;

        case LIBDEFLATE_INSUFFICIENT_SPACE:
            throwException(env, "java/util/zip/DataFormatException",
                         "Insufficient output buffer space");
            return JNI_FALSE;

        default:
            char error_message_b[256];
            snprintf(error_message_b, sizeof(error_message_b),
                "Unknown libdeflate error code: %d",
                result);
            throwException(env, "java/util/zip/DataFormatException", error_message_b);
            return JNI_FALSE;
    }
}