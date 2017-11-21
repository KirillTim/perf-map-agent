/*
 *   libperfmap: a JVM agent to create perf-<pid>.map files for consumption
 *               with linux perf-tools
 *   Copyright (C) 2013-2015 Johannes Rudolph<johannes.rudolph@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <sys/types.h>
#include <stdio.h>

#ifdef __APPLE__

#include <stdlib.h>

#else
#include <error.h>
#endif

#include <errno.h>

#include "perf-map-file.h"

FILE *_map_file_open(pid_t pid, const char *name_suffix) {
    char filename[500];
    snprintf(filename, sizeof(filename), "/tmp/perf-%d%s.map", pid, name_suffix);
    FILE *res = fopen(filename, "w");
    if (!res) {
#ifdef __APPLE__
        fprintf(stderr, "Couldn't open %s: errno(%d)", filename, errno);
        exit(0);
#else
        error(0, errno, "Couldn't open %s.", filename);
#endif
    }
    return res;
}

FILE *perf_map_open(pid_t pid) {
    return _map_file_open(pid, "");
}

int perf_map_close(FILE *fp) {
    if (fp)
        return fclose(fp);
    else
        return 0;
}

void perf_map_write_entry(FILE *method_file, const void *code_addr, unsigned int code_size, const char *entry) {
    if (method_file) {
        fprintf(method_file, "%lx %x %s\n", (unsigned long) code_addr, code_size, entry);
        fflush(method_file);
    }
}

FILE *perf_thread_map_open(pid_t pid) {
    return _map_file_open(pid, "-thread");
}

void perf_thread_man_write_entry(FILE *thread_file, const uint64_t pthread_tid, const char* jvm_thread_name) {
    if (thread_file) {
        fprintf(thread_file, "%llu %s\n", pthread_tid, jvm_thread_name);
        fflush(thread_file);
    }
}
