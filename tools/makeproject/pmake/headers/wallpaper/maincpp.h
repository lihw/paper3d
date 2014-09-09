const char MAINCPP_STR[] = 
"// main.cpp\n"
"%s\n"
"//\n"
"\n"
"\n"
"// Just a wrapper to the pass the control to the main entry of paper3d engine.\n"
"int main(int argc, char* argv[])\n"
"{\n"
"    extern int pwin32main(int argc, char* argv[]);\n"
"    return pwin32main(argc, argv);\n"
"}\n"
;