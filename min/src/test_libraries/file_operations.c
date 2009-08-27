/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
 * Contact e-mail: DG.MIN-Support@nokia.com
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 2 of the License. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
 * more details. You should have received a copy of the GNU General Public 
 * License along with this program. If not,  see 
 * <http://www.gnu.org/licenses/>.
 */



#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int create_file (const char *path)
{
        FILE           *file;
        if (path == NULL) {
                return 1;
        }

        file = fopen (path, "w");

        if (!file) {
                return 1;
        }

        fclose (file);

        return 0;
}

FILE           *open_file (const char *path)
{
        FILE           *file;

        if (path == NULL) {
                return NULL;
        }

        file = fopen (path, "r+");

        return file;
}

int read_char_from_file (FILE * file, char *c)
{
        int             res;

        if (file == NULL) {
                return 1;
        }

        res = fgetc (file);
        if (res == EOF) {
                return 1;
        }
        *c = (char)res;

        rewind (file);

        return 0;
}

int write_char_to_file (FILE * file, const char c)
{
        if (fputc (c, file) == EOF) {
                return 1;
        }

        rewind (file);
        return 0;
}

int delete_file (const char *path)
{
        if (remove (path)) {
                return 1;
        }

        return 0;
}

