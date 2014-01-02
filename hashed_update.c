#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// http://stackoverflow.com/a/8389763/266720
#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/md5.h>
#endif

void buf2md5(const char *buf, int length, char digest[16]) {
    int n;
    MD5_CTX c;

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, buf, 512);
        } else {
            MD5_Update(&c, buf, length);
        }
        length -= 512;
        buf += 512;
    }

    MD5_Final(digest, &c);
}

int main(int argc, char* argv[]) {
    char digest[16];
    buf2md5("hello", strlen("hello"), digest);
    printf("%02x\n", digest[0]);
    
    return 0;
}
