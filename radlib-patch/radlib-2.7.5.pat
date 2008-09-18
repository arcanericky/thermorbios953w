--- radlib-2.7.5/./src/radconffile.c	2007-11-23 07:22:21.000000000 -0600
+++ radlib-2.7.5-mods/./src/radconffile.c	2008-09-18 18:16:02.000000000 -0500
@@ -433,13 +433,18 @@
 
 
             /*  Get the value  */
-            curToken = strtok (NULL, " #\t\n");
+            curToken = strtok (NULL, "#\t\n");
             if (curToken == NULL)
             {
                 entry->value[0] = '\0';
             }
             else
             {
+				while (*(curToken + strlen(curToken) - 1) == ' ')
+						{
+						*(curToken + strlen(curToken) - 1) = '\0';
+						}
+
                 strncpy (entry->value, curToken, MAX_LINE_LENGTH);
             }
 
