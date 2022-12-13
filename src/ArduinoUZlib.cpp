#include "ArduinoUZlib.h"
#define OUT_CHUNK_SIZE 1
ArduinoUZlib::ArduinoUZlib(){}
static void error(String msg) {
  Serial.print("!!!ERROR:");
  Serial.println(msg);
}
void exit_error(const char *what)
{
    printf("ERROR: %s\n", what);
    exit(1);
}


// https://github.com/pfalcon/uzlib/blob/master/examples/tgunzip/tgunzip.c
int32_t ArduinoUZlib::decompress(uint8_t *in_buf, uint32_t in_size, uint8_t *&dest, uint32_t &out_size) {
	if (in_size < 4) exit_error("file too small");
	unsigned int len,dlen, outlen;
	unsigned char *source =(unsigned char*)in_buf;
	
	int res;
	len=in_size;
    dlen =            source[len - 1];
    dlen = 256*dlen + source[len - 2];
    dlen = 256*dlen + source[len - 3];
    dlen = 256*dlen + source[len - 4];

    outlen = dlen;
	dlen++;
	dest = (unsigned char *)malloc(dlen);
    if (dest == NULL) exit_error("memory");

  	struct uzlib_uncomp d;
	//    uzlib_uncompress_init(&d, malloc(32768), 32768);
	uzlib_uncompress_init(&d, NULL, 0);
	d.source = source;
    d.source_limit = source + len - 4;
    d.source_read_cb = NULL;

    res = uzlib_gzip_parse_header(&d);
    if (res != TINF_OK) {
        printf("Error parsing header: %d\n", res);
        exit(1);
    }

    d.dest_start = d.dest = dest;

    while (dlen) {
        unsigned int chunk_len = dlen < OUT_CHUNK_SIZE ? dlen : OUT_CHUNK_SIZE;
        d.dest_limit = d.dest + chunk_len;
        res = uzlib_uncompress_chksum(&d);
        dlen -= chunk_len;
        if (res != TINF_OK) {
            break;
        }
    }

    if (res != TINF_DONE) {
        printf("Error during decompression: %d\n", res);
        exit(-res);
    }

    printf("decompressed %lu bytes\n", d.dest - dest);
	out_size= d.dest - dest;
	return  out_size;





// 	/* some common variables. */
// 	int32_t result = 0;
// 	int32_t tb     = 0;
//   	outprintsize=0;
// 	z_stream z;

// 	/* fill the stream structure for zlib. */
// 	z.next_in   = (Bytef *)in_buf;
// 	z.avail_in  = (uInt)in_size;
// 	z.total_in  = in_size;
// 	z.next_out  = (Bytef *)out_buf;
// 	z.avail_out = (uInt)out_size;
// 	z.total_out = 0;
// 	z.zalloc    = NULL;
// 	z.zfree     = NULL;
  
//   if((result=inflateInit2(&z, MAX_WBITS|32)) != Z_OK) {
  
//   // if((result=inflateInit2(&z, 47)) != Z_OK) {
//     error("something on zlib initialization failed.");
//       return result;
//   }

// 	/* call zlib to decompress the data. */
// 	if ((result = inflate(&z, Z_FINISH)) != Z_STREAM_END) {
//     error("inflate something on zlib decompression failed");
//     Serial.println(result);
// 		/* something on zlib decompression failed. */
// 		return result;
// 	}

// 	/* save transferred bytes. */
// 	tb = z.total_out;

// 	/* cleanup zlib. */
// 	if ((result = inflateEnd(&z)) != Z_OK) {
//     error("something on zlib finalization failed.");
// 		/* something on zlib finalization failed. */
// 		return result;
// 	}

// 	/* return transferred bytes. */
//   Serial.println("decompress success!");
//     outprintsize=tb;
//     Serial.println(tb);
// 	return tb;
} 
