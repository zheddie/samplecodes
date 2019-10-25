--- pytorch.src/torch/csrc/jit/fuser/cpu/temp_file.h	2018-12-07 07:44:56.000000000 +0800
+++ pytorch/torch/csrc/jit/fuser/cpu/temp_file.h	2019-01-12 09:57:35.000000000 +0800
@@ -14,21 +14,26 @@
 
 namespace torch { namespace jit { namespace fuser { namespace cpu {
 
 struct TempFile {
   TH_DISALLOW_COPY_AND_ASSIGN(TempFile);
 
   TempFile(const std::string& t, int suffix) {
     // mkstemps edits its first argument in places
     // so we make a copy of the string here, including null terminator
     std::vector<char> tt(t.c_str(), t.c_str() + t.size() + 1);
+#if defined(__PASE__)
+    //zg.we do not have mkstemps on PASE.remove the suffix.TODO.
+    int fd = mkstemp(tt.data());
+#else
     int fd = mkstemps(tt.data(), suffix);
+#endif
     JIT_ASSERT(fd != -1);
     file_ = fdopen(fd, "r+");
 
     // - 1 becuase tt.size() includes the null terminator,
     // but std::string does not expect one
     name_ = std::string(tt.begin(), tt.end() - 1);
   }
 
   const std::string& name() const {
     return name_;
