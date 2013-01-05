diff -Naur radlib-2.7.5/debug/Makefile.am radlib-2.7.5-modded/debug/Makefile.am
--- radlib-2.7.5/debug/Makefile.am	2008-02-08 07:56:01.000000000 -0600
+++ radlib-2.7.5-modded/debug/Makefile.am	2008-09-19 15:54:09.000000000 -0500
@@ -35,6 +35,6 @@
 endif
 endif
 
-if CROSSCOMPILE
-raddebug_LDFLAGS += $(prefix)/lib/crt1.o $(prefix)/lib/crti.o $(prefix)/lib/crtn.o
-endif
+#if CROSSCOMPILE
+#raddebug_LDFLAGS += $(prefix)/lib/crt1.o $(prefix)/lib/crti.o $(prefix)/lib/crtn.o
+#endif
diff -Naur radlib-2.7.5/msgRouter/Makefile.am radlib-2.7.5-modded/msgRouter/Makefile.am
--- radlib-2.7.5/msgRouter/Makefile.am	2008-02-08 07:57:44.000000000 -0600
+++ radlib-2.7.5-modded/msgRouter/Makefile.am	2008-09-19 15:41:47.000000000 -0500
@@ -35,6 +35,6 @@
 endif
 endif
 
-if CROSSCOMPILE
-radmrouted_LDFLAGS += $(prefix)/lib/crt1.o $(prefix)/lib/crti.o $(prefix)/lib/crtn.o
-endif
+#if CROSSCOMPILE
+#radmrouted_LDFLAGS += $(prefix)/lib/crt1.o $(prefix)/lib/crti.o $(prefix)/lib/crtn.o
+#endif
