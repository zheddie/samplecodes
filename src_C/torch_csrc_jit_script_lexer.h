--- pytorch.src/torch/csrc/jit/script/lexer.h	2018-12-07 07:44:56.000000000 +0800
+++ pytorch/torch/csrc/jit/script/lexer.h	2019-01-12 09:27:25.000000000 +0800
@@ -160,21 +160,26 @@
 #ifdef _WIN32
   static double strtod_c(const char * str, char** end) {
     /// NOLINTNEXTLINE(hicpp-signed-bitwise)
     static _locale_t loc = _create_locale(LC_ALL, "C");
     return _strtod_l(str, end, loc);
   }
 #else
   static double strtod_c(const char * str, char** end) {
     /// NOLINTNEXTLINE(hicpp-signed-bitwise)
     static locale_t loc = newlocale(LC_ALL_MASK, "C", nullptr);
+#if defined(__PASE__)
+    //zg.temp remove locale.TODO.
+    return strtod(str, end);
+#else
     return strtod_l(str, end, loc);
+#endif
   }
 #endif
   // 1. skip whitespace
   // 2. handle comment or newline
   //
   bool isNumber(const std::string& str, size_t start, size_t* len) {
     char first = str[start];
     // strtod allows numbers to start with + or - or nan or inf
     // http://en.cppreference.com/w/cpp/string/byte/strtof
     // but we want only the number part, otherwise 1+3 will turn into two
