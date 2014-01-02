hashed_update
=============

Copy changed blocks from one file to another.

The program reads input file and hash file and copies only blocks where the hash is mismatched.

Example script:

```
# initial full copy
hashed_update fast_source NULL      slow_sink hashfile    65536

# periodically update
while true; do
    sleep 120
    hashed_update fast_source hashfile     slow_sink hashfile.new 65536
    mv hashfile.new hashfile
done
```

In case of using this for keeping in sync some mounted filesystem, usage of LVM snapshots both for source and for destination is recommended ([Example](http://superuser.com/questions/695125/best-practices-of-keeping-firefox-profile-on-ramdisk)).


```
hashed_update: Copies changed blocks from source to destination, according to stored separately block md5 hashes.
Usage: hashed_update input input_hashes output output_hashes blocksize
       Use NULL instead of arguments for various modes
       Output file must exist and be seekable
  Create hashes:   hashed_update smth NULL NULL   smth.hashes   262144
  Measure changes: hashed_update smth smth.hashes NULL NULL     262144
  Just copy:       hashed_update smth NULL        outfile NULL  262144
  Hashed update:   hashed_update smth smth.hashes outfile NULL  262144
  Hashed update plus save new hashfile: hashed_update smth smth.hashes outfile outfile.hashes 262144
```
