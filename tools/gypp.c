#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#else
#include <process.h>
#endif
#ifndef GYPP_PROJECT_ROOT
#define GYPP_PROJECT_ROOT "."
#endif
#ifndef GYPP_EMBEDDED_SOURCE
#define GYPP_EMBEDDED_SOURCE "build/embedded_libraries.c"
#endif
#define MAX_FLAGS 64

static void usage(FILE *f) {
    fputs("gy++ - YLANG++ native compiler (Clang++ toolchain)\n"
          "Usage: gy++ <program.ycpp> [-o output] [-O0|-O1|-O2|-O3] [-g]\n"
          "Options: --clang++ PATH, --xx-warn-all, --xx-warn-extra, --xx-optimize=0..3,\n"
          "         -Wall, -Wextra, -Wpedantic, -Werror, -Wno-WARNING,\n"
          "         -Werror=WARNING, -g, --help, --version\n"
          "The source is embedded into a native executable linked with the YPP runtime.\n", f);
}
static char *dupstr(const char *s) {
    size_t n=strlen(s)+1; char *p=malloc(n); if(p) memcpy(p,s,n); return p;
}
static char *join(const char *a,const char *b) {
    size_t n=strlen(a)+strlen(b)+2; char *p=malloc(n);
    if(p) snprintf(p,n,"%s/%s",a,b);
    return p;
}
static char *default_output(const char *s) {
    const char *b=s; for(const char*p=s;*p;++p)if(*p=='/'||*p=='\\')b=p+1;
    char *r=dupstr(b); if(!r)return NULL; char*d=strrchr(r,'.');if(d&&d!=r)*d=0;return r;
}
static int has_extension(const char *path,const char *ext) {
    size_t a=strlen(path),b=strlen(ext); return a>=b && strcmp(path+a-b,ext)==0;
}
static char *read_source(const char *path) {
    FILE*f=fopen(path,"rb");
    if(!f){fprintf(stderr,"%s: error: %s\n",path,strerror(errno));return NULL;}
    if(fseek(f,0,SEEK_END)){fclose(f);return NULL;}
    long n=ftell(f);
    if(n<0||fseek(f,0,SEEK_SET)){fclose(f);return NULL;}
    char*s=malloc((size_t)n+1);if(!s){fclose(f);return NULL;}
    size_t got=fread(s,1,(size_t)n,f);
    if(got!=(size_t)n&&ferror(f)){free(s);fclose(f);return NULL;}
    s[got]=0;fclose(f);return s;
}
static int write_c_string(FILE*f,const char*s) {
    if(fputc('"',f)==EOF)return 0;
    for(const unsigned char*p=(const unsigned char*)s;*p;++p)
        if(fprintf(f,"\\%03o",(unsigned)*p)<0)return 0;
    return fputc('"',f)!=EOF;
}
static int wrapper_file(const char*path,const char*source,const char*source_path) {
    FILE*f=fopen(path,"wb");
    if(!f){fprintf(stderr,"gy++: error: cannot create %s: %s\n",path,strerror(errno));return 0;}
    int ok=fputs("#include \"runtime.h\"\n#include <stdio.h>\n"
                 "static const char gypp_source[] = ",f)>=0
        &&write_c_string(f,source)
        &&fputs(";\nstatic const char gypp_path[] = ",f)>=0
        &&write_c_string(f,source_path)
        &&fputs(";\n"
          "static int gypp_cpp_add(const YPPValue *args, size_t count, YPPValue *result, char *error, size_t capacity) {\n"
          " if (count != 2 || args[0].kind != YPP_VALUE_NUMBER || args[1].kind != YPP_VALUE_NUMBER) {\n"
          "  if (capacity) snprintf(error, capacity, \"cpp_add expects exactly two numbers\"); return 0; }\n"
          " result->kind = YPP_VALUE_NUMBER; result->number_value = args[0].number_value + args[1].number_value; return 1; }\n"
          "int main(void) { ypp_register_native(\"cpp_add\", gypp_cpp_add); return ypp_runtime_run_source(gypp_source, gypp_path); }\n",f)>=0;
    if(fclose(f))ok=0;
    return ok;
}
static int run(char *const argv[]) {
#ifndef _WIN32
    pid_t pid=fork();
    if(pid<0){fprintf(stderr,"gy++: error: cannot start Clang++: %s\n",strerror(errno));return 127;}
    if(pid==0){execvp(argv[0],argv);fprintf(stderr,"gy++: error: cannot execute %s: %s\n",argv[0],strerror(errno));_exit(127);}
    int status=0;
    if(waitpid(pid,&status,0)<0)return 127;
    if(WIFEXITED(status))return WEXITSTATUS(status);
    if(WIFSIGNALED(status))return 128+WTERMSIG(status);
    return 1;
#else
    intptr_t status=_spawnvp(_P_WAIT,argv[0],(const char * const *)argv);
    if(status==-1){fprintf(stderr,"gy++: error: cannot execute %s: %s\n",argv[0],strerror(errno));return 127;}
    return (int)status;
#endif
}
int main(int argc,char**argv) {
    const char *clang=getenv("LLVM_CLANGXX"),*opt="-O0",*input=NULL,*outarg=NULL;
    const char *warnings[MAX_FLAGS];size_t warning_count=0;int debug=0;
    char *wrapper=NULL,*inc=NULL,*include_dir=NULL,*rt=NULL,*mem=NULL,*io=NULL;
    if(!clang||!*clang)clang="clang++";
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i],"--help")||!strcmp(argv[i],"-h")){usage(stdout);return 0;}
        if(!strcmp(argv[i],"--version")){puts("gy++ 0.1.0 (YLANG++ native compiler)");return 0;}
        if(!strcmp(argv[i],"-o")){if(++i>=argc){fputs("gy++: missing output after -o\n",stderr);return 2;}outarg=argv[i];}
        else if(!strcmp(argv[i],"--clang++")){if(++i>=argc){fputs("gy++: missing path after --clang++\n",stderr);return 2;}clang=argv[i];}
        else if(!strncmp(argv[i],"-O",2)){if(strcmp(argv[i],"-O0")&&strcmp(argv[i],"-O1")&&strcmp(argv[i],"-O2")&&strcmp(argv[i],"-O3")){fprintf(stderr,"gy++: invalid optimization '%s'\n",argv[i]);return 2;}opt=argv[i];}
        else if(!strcmp(argv[i],"-g"))debug=1;
        else if(!strcmp(argv[i],"--xx-warn-all")){if(warning_count==MAX_FLAGS)return 2;warnings[warning_count++]="-Wall";}
        else if(!strcmp(argv[i],"--xx-warn-extra")){if(warning_count==MAX_FLAGS)return 2;warnings[warning_count++]="-Wextra";}
        else if(!strncmp(argv[i],"--xx-optimize=",14)){const char *level=argv[i]+14;if(strlen(level)!=1||level[0]<'0'||level[0]>'3'){fprintf(stderr,"gy++: invalid optimization '%s'\n",argv[i]);return 2;}static char xx_opt[4];snprintf(xx_opt,sizeof(xx_opt),"-O%c",level[0]);opt=xx_opt;}
        else if(!strcmp(argv[i],"-Wall")||!strcmp(argv[i],"-Wextra")||!strcmp(argv[i],"-Wpedantic")||!strcmp(argv[i],"-Werror")||!strncmp(argv[i],"-Wno-",5)||!strncmp(argv[i],"-Werror=",8)){if(warning_count==MAX_FLAGS)return 2;warnings[warning_count++]=argv[i];}
        else if(argv[i][0]=='-'){fprintf(stderr,"gy++: unknown option '%s'\n",argv[i]);return 2;}
        else if(input){fputs("gy++: only one .ycpp entry file is accepted\n",stderr);return 2;}
        else input=argv[i];
    }
    if(!input||!has_extension(input,".ycpp")){usage(stderr);return 2;}
    char *source=read_source(input);if(!source)return 1;
    char *owned=NULL;if(!outarg){owned=default_output(input);outarg=owned;}
    if(!outarg){free(source);return 2;}
    size_t wn=strlen(outarg)+16;wrapper=malloc(wn);
    inc=join(GYPP_PROJECT_ROOT,"src");include_dir=join(GYPP_PROJECT_ROOT,"include");
    rt=join(GYPP_PROJECT_ROOT,"src/runtime.c");mem=join(GYPP_PROJECT_ROOT,"src/memory.c");io=join(GYPP_PROJECT_ROOT,"src/file_io.c");
    if(!wrapper||!inc||!include_dir||!rt||!mem||!io){fputs("gy++: out of memory\n",stderr);free(source);free(owned);free(wrapper);free(inc);free(include_dir);free(rt);free(mem);free(io);return 2;}
    snprintf(wrapper,wn,"%s.gypp.c",outarg);
    if(!wrapper_file(wrapper,source,input)){free(source);free(owned);free(wrapper);free(inc);free(include_dir);free(rt);free(mem);free(io);return 2;}
    char *args[160];size_t a=0;
    args[a++]=(char*)clang;args[a++]="-std=c11";args[a++]="-Wall";args[a++]="-Wextra";args[a++]="-Wpedantic";args[a++]=(char*)opt;
    if(debug)args[a++]="-g";
    for(size_t i=0;i<warning_count;++i)args[a++]=(char*)warnings[i];
    args[a++]="-I";args[a++]=inc;args[a++]="-I";args[a++]=include_dir;
    args[a++]="-x";args[a++]="c";args[a++]=wrapper;args[a++]=rt;args[a++]=mem;args[a++]=io;args[a++]=(char*)GYPP_EMBEDDED_SOURCE;
    args[a++]="-lm";args[a++]="-o";args[a++]=(char*)outarg;args[a]=NULL;
    int result=run(args);
    remove(wrapper);free(source);free(owned);free(wrapper);free(inc);free(include_dir);free(rt);free(mem);free(io);
    return result;
}
