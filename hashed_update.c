#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
    
    const char* in_filename;
    const char* out_filename;
    const char* in_hashfilename;
    const char* out_hashfilename;
    int blocksize=0;
    
    if(argc!=1+5) {
        printf("Usage: hashed_update input input_hashes output output_hashes blocksize\n");
        printf("       Use NULL instead of arguments for various modes\n");
        printf("  Create hashes: hashed_update smth NULL NULL   smth.hashes   262144\n");
        printf("  Verify hashes: hashed_update smth smth.hashes NULL NULL     262144\n");
        printf("  Just copy:     hashed_update smth NULL        outfile NULL  262144\n");
        printf("  Hashed update: hashed_update smth smth.hashes outfile NULL  262144\n");
        printf("  Hashed update plus save new hashfile: hashed_update smth smth.hashes outfile outfile.hashes 262144\n");
        return 1;
    }
    
    in_filename = argv[1];
    in_hashfilename = argv[2];
    out_filename = argv[3];
    out_hashfilename = argv[4];
    assert(sscanf(argv[5], "%d", &blocksize)==1);
    
    return 0;
}
