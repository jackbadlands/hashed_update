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

void buf2md5(const char *buf, int length, unsigned char digest[16]) {
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
    const char* in_filename=NULL;
    const char* out_filename=NULL;
    const char* in_hash_filename=NULL;
    const char* out_hash_filename=NULL;
    int blocksize=0;
    
    if(argc!=1+5) {
        printf("hashed_update: Copies changed blocks from source to destination, according to stored separately block md5 hashes.\n");
        printf("Usage: hashed_update input input_hashes output output_hashes blocksize\n");
        printf("       Use NULL instead of arguments for various modes\n");
        printf("       Output file must exist and be seekable\n");
        printf("  Create hashes:   hashed_update smth NULL NULL   smth.hashes   262144\n");
        printf("  Measure changes: hashed_update smth smth.hashes NULL NULL     262144\n");
        printf("  Just copy:       hashed_update smth NULL        outfile NULL  262144\n");
        printf("  Hashed update:   hashed_update smth smth.hashes outfile NULL  262144\n");
        printf("  Hashed update plus save new hashfile: hashed_update smth smth.hashes outfile outfile.hashes 262144\n");
        return 1;
    }
#define NULLIZE(x) if(!strcmp(x,"NULL"))x=NULL;
    in_filename = argv[1];      NULLIZE(in_filename);
    in_hash_filename = argv[2];  NULLIZE(in_hash_filename);
    out_filename = argv[3];     NULLIZE(out_filename);
    out_hash_filename = argv[4]; NULLIZE(out_hash_filename);
#undef NULLIZE
    assert(sscanf(argv[5], "%d", &blocksize)==1);
    assert(blocksize>0 && blocksize<0x80000000);
    
    
    //fprintf(stderr, "Parameters: %s %s %s %s %d\n", in_filename, in_hash_filename, out_filename, out_hash_filename, blocksize);
    
    char* buf = (char*)malloc(blocksize);
    assert(buf);
    
    FILE *in=NULL, *out=NULL, *in_hash=NULL, *out_hash=NULL;
#define OPENTHIS(x,mode) if(x##_filename) { x = fopen(x##_filename, mode); assert(x); }
    OPENTHIS(in, "rb");
    OPENTHIS(in_hash, "rb");
    OPENTHIS(out, "r+b");
    OPENTHIS(out_hash, "wb");
#undef OPENTHIS
    
    if(!in) { fprintf(stderr, "No input file - nothing to do\n"); return 0; }
    
    int s;
    
    long long int total_blocks=0;
    long long int equal_blocks=0;
    
    int chopped_block_warning=0;
    int exhausted_in_digests_warning=0;
    
    int previous_block_was_skipped=0;
    
    while(!feof(in)) {
        unsigned char digest[16];
        unsigned char in_digest[16];
        int in_digest_present=0;
        int in_digest_equal=0;
        
        s = fread(buf, 1, blocksize, in);
        if (s!=blocksize) {
            if(!s) break;
            if(!chopped_block_warning) {
                chopped_block_warning=1;
                fprintf(stderr, "Warning: chopped block %lld; size=%d\n", total_blocks, s);
            }
        }
        
        buf2md5(buf, s, digest);
        
        if (in_hash) {
            int ret = fread(in_digest, 16, 1, in_hash);
            if(ret==1) {
                in_digest_present=1;
            } else {
                if(!exhausted_in_digests_warning) {
                    exhausted_in_digests_warning=1;
                    fprintf(stderr, "Warning: input hash file too small\n");
                }
            }
        }
        
        if (in_digest_present) {
            if (!memcmp(digest, in_digest, 16)) {
                in_digest_equal=1;
                ++equal_blocks;
            }
        }
        
        if (out_hash) {
            int ret = fwrite(digest, 16, 1, out_hash);
            assert(ret==1);
        }
        
        if (out) {
            if (in_digest_equal) {
                previous_block_was_skipped = 1;
            } else {
                if (previous_block_was_skipped) {
                    assert(!fseeko(out, blocksize*total_blocks, SEEK_SET));
                }
                previous_block_was_skipped=0;
                assert(fwrite(buf, 1, s, out) == s);
            }
        }
        
        ++total_blocks;
    }
    
    if (in) fclose(in);
    if (out) fclose(out);
    if (in_hash) fclose(in_hash);
    if (out_hash) fclose(out_hash);
    
    printf("%lld %lld\n", total_blocks-equal_blocks, total_blocks);
    return 0;
}
