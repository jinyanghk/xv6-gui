struct stat;
struct rtcdate;

struct RGBA;
struct RGB;
struct message;
struct window;

typedef void(*Handler)(struct message *msg);

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);

//window_manager.c
int GUI_createWindow(struct window*, const char*);
int GUI_closeWindow(struct window*);
int GUI_maximizeWindow(struct window*);
int GUI_minimizeWindow(struct window*);
int GUI_getMessage(int, struct message *);
void GUI_updateScreen();

// themis_ui.c
void createWindow(struct window *, const char*);
void closeWindow(struct window *);
void updateWindow(struct window*);
int addButtonWidget(struct window *win,struct  RGBA c, struct RGBA bc, char* text, int x, int y, int w, int h, Handler handler);

void drawFillRect(struct window *win, struct RGBA color, int x, int y, int width, int height);
void drawString(struct window *win, int x, int y, char *str, struct RGBA color, int width);