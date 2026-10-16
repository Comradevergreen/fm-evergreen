// SPDX-License-Identifier: GPL-3.0-only
// JIANT Probe: isochronous IN reads through usbfs on the file descriptor UsbDeviceConnection hands us (no root: the
// app owns the device once the user grants it). Android's audio framework passes an 8-channel USB input on as stereo
// on some phones (the rest zero); reading the FM-1's stream ourselves gets every channel, as the commercial
// multichannel recorders do.
#include <jni.h>
#include <errno.h>
#include <linux/usbdevice_fs.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>

#define NURB 8              /* URBs in flight */
#define NPK 8               /* packets (1 ms each at full speed) an URB */

static long long now_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

static void setup(struct usbdevfs_urb *u, int ep, int maxp)
{
    u->type = USBDEVFS_URB_TYPE_ISO;
    u->endpoint = (unsigned char)ep;
    u->flags = USBDEVFS_URB_ISO_ASAP;
    u->buffer_length = NPK * maxp;
    u->number_of_packets = NPK;
    u->status = 0;
    u->actual_length = 0;
    u->error_count = 0;
    for (int k = 0; k < NPK; k++) {
        u->iso_frame_desc[k].length = (unsigned)maxp;
        u->iso_frame_desc[k].actual_length = 0;
        u->iso_frame_desc[k].status = 0;
    }
}

/* fills out (bytes, as the device sends them) until it is full or timeout_ms passes; stats: packets, empty packets,
 * packet errors, errno of a failed ioctl (0 none), the largest packet. Returns the bytes read, or -errno */
JNIEXPORT jint JNICALL Java_fm_jiant_probe_MainActivity_isoRead(JNIEnv *env, jclass cls, jint fd, jint ep, jint maxp,
                                                                jbyteArray out, jint timeout_ms, jintArray stats)
{
    (void)cls;
    jsize cap = (*env)->GetArrayLength(env, out);
    jbyte *dst = (*env)->GetByteArrayElements(env, out, NULL);
    size_t sz = sizeof(struct usbdevfs_urb) + NPK * sizeof(struct usbdevfs_iso_packet_desc);
    struct usbdevfs_urb *u[NURB];
    int got = 0, packets = 0, empty = 0, perr = 0, err = 0, big = 0, live = 0;
    for (int i = 0; i < NURB; i++) {
        u[i] = calloc(1, sz);
        u[i]->buffer = malloc((size_t)NPK * maxp);
        setup(u[i], ep, maxp);
        if (ioctl(fd, USBDEVFS_SUBMITURB, u[i]) < 0) { err = errno; break; }
        live++;
    }
    long long end = now_ms() + timeout_ms;
    while (!err && got < cap && now_ms() < end) {
        struct usbdevfs_urb *r = NULL;
        if (ioctl(fd, USBDEVFS_REAPURBNDELAY, &r) < 0) {
            if (errno == EAGAIN) { struct timespec s = {0, 1000000}; nanosleep(&s, NULL); continue; }
            if (errno == EINTR) continue;
            err = errno;
            break;
        }
        live--;
        const char *b = r->buffer;
        for (int k = 0; k < NPK; k++) {
            int n = (int)r->iso_frame_desc[k].actual_length;
            packets++;
            if (r->iso_frame_desc[k].status) perr++;
            if (!n) { empty++; continue; }
            if (n > big) big = n;
            if (n > cap - got) n = cap - got;
            memcpy(dst + got, b + k * maxp, (size_t)n);   /* iso packets sit at their requested offsets */
            got += n;
        }
        setup(r, ep, maxp);
        if (got < cap) {
            if (ioctl(fd, USBDEVFS_SUBMITURB, r) < 0) { err = errno; break; }
            live++;
        }
    }
    for (int i = 0; i < NURB; i++) ioctl(fd, USBDEVFS_DISCARDURB, u[i]);
    long long stop = now_ms() + 500;
    while (live > 0 && now_ms() < stop) {               /* every URB back before its memory goes */
        struct usbdevfs_urb *r = NULL;
        if (ioctl(fd, USBDEVFS_REAPURBNDELAY, &r) == 0) { live--; continue; }
        if (errno != EAGAIN && errno != EINTR) break;
        struct timespec s = {0, 1000000};
        nanosleep(&s, NULL);
    }
    if (live == 0)
        for (int i = 0; i < NURB; i++) { free(u[i]->buffer); free(u[i]); }   /* (else leak: the kernel may still write) */
    (*env)->ReleaseByteArrayElements(env, out, dst, 0);
    jint st[6] = {packets, empty, perr, err, big, live};
    (*env)->SetIntArrayRegion(env, stats, 0, 6, st);
    return got ? got : -err;
}
