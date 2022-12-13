# Arduino zlib库

用法:

```cpp
size_t size = stream->available(); // 还剩下多少数据没有读完？
uint8_t inbuff[size]; // 准备一个数组来装流数据，有多少装多少
stream->readBytes(inbuff, size); // 将http流数据写入inbuff中
outbuf=(uint8_t*)malloc(sizeof(uint8_t)*15000); // 解压这些流预计需要的空间大小，这里很难估算。
uint32_t outprintsize=0; // 解压后多大？在调用解压方法后会被赋值。
// 调用解压函数
int result=ArduinoUZlib::decompress(buff, readBytesSize, outbuf, 15000,outprintsize);
// 输出解密后的数据到控制台。
Serial.write(outbuf,outprintsize);
```
说明:
1. `uint8_t *outbuf` 是解压后占用的内存，通常体积是原来的1-10倍，请确保你有足够的内存解压，其次不要将它定义在loop()中，否则如果太大会报栈溢出错误。
2. `outbuf`: 的定义不要放在loop()里面避免栈溢出。
3. ESP8266内存空间十分有限，解压时会经常出现不足的情况

# 参考
1. https://yuanze.wang/posts/esp32-unzip-gzip-http-response/
2. https://www.programcreek.com/cpp/?code=Looking4Group%2FL4G_Core%2FL4G_Core-master%2Ftools%2Flibmpq%2Flibmpq%2Fextract.c
3. https://stackoverflow.com/questions/1838699/how-can-i-decompress-a-gzip-stream-with-zlib


