/* Copyright (c) 2018, David Hauweele <david@hauweele.net>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GEOURI_H_
#define _GEOURI_H_

/* Transform macro argument as string literal. */
#define _stringify(s) #s
#define stringify(s) _stringify(s)

#define MAJOR_VERSION 0
#define MINOR_VERSION 3

#define VERSION stringify(MAJOR_VERSION) "." stringify(MINOR_VERSION)

/* name */
#define NAME      "geouri" /* internal name */
#define DISP_NAME "GeoURI" /* displayed name */

/* plugin information */
#define PLUGIN_ID   "core-" NAME /* core plugin id */
#define PLUGIN_NAME DISP_NAME

#ifndef COMMIT
# define PLUGIN_VERSION VERSION
#else
/* add commit when available */
# define PLUGIN_VERSION VERSION " (commit: " PARTIAL_COMMIT ")"
#endif

#define PLUGIN_SUMMARY     "Convert geo URI scheme to Google Maps or OpenStreetMap link."
#define PLUGIN_DESCRIPTION PLUGIN_SUMMARY
#define PLUGIN_AUTHOR      "David Hauweele <david@hauweele.net>"
#define PLUGIN_WEBSITE     "http://hauweele.net/~gawen/pidgin-geouri.html"

#define PLUGIN_PREF "/plugins/core/" NAME /* preference root */

#endif /* _GEOURI_H_ */
