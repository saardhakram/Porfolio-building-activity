#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LEN   512

typedef struct {
    char *lines[MAX_LINES];
    int   count;
} Document;

static Document doc, backup;
static int has_backup = 0;

void strip(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

int valid(int n) { return n >= 1 && n <= doc.count; }

char *dup_str(const char *s) {
    char *p = malloc(strlen(s) + 1);
    if (p) strcpy(p, s);
    return p;
}

void doc_free(Document *d) {
    for (int i = 0; i < d->count; i++) free(d->lines[i]);
    d->count = 0;
}

void snapshot(void) {
    doc_free(&backup);
    for (int i = 0; i < doc.count; i++) backup.lines[i] = dup_str(doc.lines[i]);
    backup.count = doc.count;
    has_backup = 1;
}

int cmd_insert(int n, const char *text) {
    if (doc.count >= MAX_LINES || n < 1 || n > doc.count + 1) return 0;
    char *copy = dup_str(text);
    if (!copy) return 0;
    for (int i = doc.count; i > n - 1; i--) doc.lines[i] = doc.lines[i-1];
    doc.lines[n-1] = copy;
    doc.count++;
    return 1;
}

int cmd_delete(int n) {
    if (!valid(n)) return 0;
    free(doc.lines[n-1]);
    for (int i = n - 1; i < doc.count - 1; i++) doc.lines[i] = doc.lines[i+1];
    doc.count--;
    return 1;
}

void cmd_print(void) {
    if (!doc.count) { printf("(document is empty)\n"); return; }
    for (int i = 0; i < doc.count; i++) printf("%4d | %s\n", i+1, doc.lines[i]);
}

int cmd_save(const char *f) {
    FILE *fp = fopen(f, "w");
    if (!fp) return 0;
    for (int i = 0; i < doc.count; i++) fprintf(fp, "%s\n", doc.lines[i]);
    fclose(fp);
    return 1;
}

int cmd_load(const char *f) {
    FILE *fp = fopen(f, "r");
    if (!fp) return 0;
    doc_free(&doc);
    char buf[MAX_LEN];
    while (doc.count < MAX_LINES && fgets(buf, sizeof buf, fp)) {
        strip(buf);
        doc.lines[doc.count++] = dup_str(buf);
    }
    fclose(fp);
    return 1;
}

void cmd_search(const char *word) {
    int hits = 0;
    for (int i = 0; i < doc.count; i++)
        if (strstr(doc.lines[i], word)) { printf("%4d | %s\n", i+1, doc.lines[i]); hits++; }
    printf(hits ? "%d line(s) matched.\n" : "Not found.\n", hits);
}

int cmd_replace(int n, const char *old, const char *new) {
    if (!valid(n)) return 0;
    char *hit = strstr(doc.lines[n-1], old);
    if (!hit) return 0;
    char buf[MAX_LEN * 2];
    int prefix = (int)(hit - doc.lines[n-1]);
    sprintf(buf, "%.*s%s%s", prefix, doc.lines[n-1], new, hit + strlen(old));
    free(doc.lines[n-1]);
    doc.lines[n-1] = dup_str(buf);
    printf("%4d | %s\n", n, doc.lines[n-1]);
    return 1;
}

void cmd_stats(void) {
    int words = 0, chars = 0;
    for (int i = 0; i < doc.count; i++) {
        chars += (int)strlen(doc.lines[i]);
        for (int in = 0, j = 0; doc.lines[i][j]; j++) {
            if (doc.lines[i][j] == ' ' || doc.lines[i][j] == '\t') in = 0;
            else if (!in) { in = 1; words++; }
        }
    }
    printf("Lines: %d   Words: %d   Characters: %d\n", doc.count, words, chars);
}

int cmd_undo(void) {
    if (!has_backup) return 0;
    Document t = doc; doc = backup; backup = t;
    return 1;
}

void print_help(void) {
    printf("\n  i <n> <text>        insert text as line n\n"
           "  d <n>               delete line n\n"
           "  p                   print the document\n"
           "  s <file>            save to file\n"
           "  l <file>            load from file\n"
           "  f <word>            find lines containing word\n"
           "  r <n> <old> <new>   replace old with new on line n\n"
           "  c                   line/word/character counts\n"
           "  u                   undo last change\n"
           "  h                   this help\n"
           "  q                   quit\n\n");
}

int main(void) {
    char in[MAX_LEN], cmd[16], a[MAX_LEN], b[MAX_LEN];
    int n, off;

    printf("Line Editor. Type 'h' for help, 'q' to quit.\n");

    while (printf("> "), fflush(stdout), fgets(in, sizeof in, stdin)) {
        strip(in);
        if (sscanf(in, "%15s", cmd) != 1) continue;

        if (!strcmp(cmd, "q")) break;
        else if (!strcmp(cmd, "h")) print_help();
        else if (!strcmp(cmd, "p")) cmd_print();
        else if (!strcmp(cmd, "c")) cmd_stats();
        else if (!strcmp(cmd, "u")) printf(cmd_undo() ? "Undone.\n" : "Nothing to undo.\n");
        else if (!strcmp(cmd, "i")) {
            off = 0;
            if (sscanf(in, "%*s %d %n", &n, &off) == 1 && off > 0) {
                snapshot();
                if (!cmd_insert(n, in + off)) printf("Error: cannot insert at line %d.\n", n);
            } else printf("Usage: i <lineno> <text>\n");
        }
        else if (!strcmp(cmd, "d")) {
            if (sscanf(in, "%*s %d", &n) == 1) {
                snapshot();
                if (!cmd_delete(n)) printf("Error: no line %d.\n", n);
            } else printf("Usage: d <lineno>\n");
        }
        else if (!strcmp(cmd, "s")) {
            if (sscanf(in, "%*s %255s", a) == 1)
                printf(cmd_save(a) ? "Saved.\n" : "Error: cannot write file.\n");
            else printf("Usage: s <filename>\n");
        }
        else if (!strcmp(cmd, "l")) {
            if (sscanf(in, "%*s %255s", a) == 1) {
                snapshot();
                printf(cmd_load(a) ? "Loaded.\n" : "Error: cannot read file.\n");
            } else printf("Usage: l <filename>\n");
        }
        else if (!strcmp(cmd, "f")) {
            off = 0;
            sscanf(in, "%*s %n", &off);
            if (off > 0 && in[off]) cmd_search(in + off);
            else printf("Usage: f <word>\n");
        }
        else if (!strcmp(cmd, "r")) {
            if (sscanf(in, "%*s %d %511s %511s", &n, a, b) == 3) {
                snapshot();
                if (!cmd_replace(n, a, b)) printf("Not found on line %d.\n", n);
            } else printf("Usage: r <lineno> <old> <new>\n");
        }
        else printf("Unknown command '%s'. Type 'h' for help.\n", cmd);
    }

    doc_free(&doc);
    doc_free(&backup);
    printf("Goodbye.\n");
    return 0;
}