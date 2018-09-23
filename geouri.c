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

#define PURPLE_PLUGINS

#include <string.h>
#include <ctype.h>
#include <glib.h>

#include "prpl.h"
#include "notify.h"
#include "debug.h"
#include "plugin.h"
#include "version.h"

#include "geouri.h"

#ifndef UNUSED
/* avoid warnings about unused variables */
# define UNUSED(x) (void)(x)
#endif

#define LAT_FORMAT_CHAR  "f" /* phi */
#define LONG_FORMAT_CHAR "l" /* lambda */
#define URI_FORMAT_CHAR  "u"

#define DEFAULT_URI_FORMAT  "http://maps.google.com/maps?q=%" LAT_FORMAT_CHAR ",%" LONG_FORMAT_CHAR
#define DEFAULT_HTML_FORMAT "<a href=\"%" URI_FORMAT_CHAR "\">Location</a>"

struct geouri {
  char *latitude;
  char *longitude;
  char *uncertainty;
};

static gboolean plugin_load(PurplePlugin *plugin);
static gboolean plugin_unload(PurplePlugin *plugin);
static PurplePluginPrefFrame * get_plugin_pref_frame(PurplePlugin *plugin);

static PurplePluginUiInfo prefs_info = {
  get_plugin_pref_frame,
  0, /* page_num (reserved) */
  NULL, /* frame (reserved) */

  /* reserved */
  NULL,
  NULL,
  NULL,
  NULL
};

static PurplePluginInfo info = {
  PURPLE_PLUGIN_MAGIC,
  PURPLE_MAJOR_VERSION,
  PURPLE_MINOR_VERSION,
  PURPLE_PLUGIN_STANDARD,
  NULL,
  0,
  NULL,
  PURPLE_PRIORITY_DEFAULT,
  PLUGIN_ID,
  PLUGIN_NAME,
  PLUGIN_VERSION,
  PLUGIN_SUMMARY,
  PLUGIN_DESCRIPTION,
  PLUGIN_AUTHOR,
  PLUGIN_WEBSITE,
  plugin_load,
  plugin_unload,
  NULL,
  NULL,
  NULL,
  &prefs_info,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static const gchar *uri_format;  /* URI format to fetch the map */
static const gchar *html_format; /* HTML format to display in conversation */

/* replace format character <c><r> with <n> string and escape with <c><c> */
static gchar * g_strreplacefmt(const gchar *s, gchar c, gchar r, const gchar *n)
{
  int sn = strlen(n);
  int sr = strlen(s);
  int index = 0;
  gchar *ret = g_malloc(sr);

  for(; *s != '\0' ; s++) {
    if(*s == c) {
      s++;
      if(*s == r) {
        const gchar *i = n;

        sr += sn;
        ret = g_realloc(ret, sr);
        for(; *i != '\0' ; i++, index++)
          ret[index] = *i;
        continue;
      }
      else if(*s != c || *s == '\0')
        s--;
    }

    ret[index] = *s;
    index++;
  }

  ret[index] = '\0';

  return ret;
}

static struct geouri * parse_geouri(struct geouri *geo, char *msg)
{
  if(strncmp(msg, "geo:", strlen("geo:")))
    return NULL; /* parse error */

  msg += strlen("geo:");
  msg++;

  geo->latitude = msg;

  for(;; msg++) {
    if(!(isdigit(*msg) || *msg == '.' || *msg == '-'))
      break;
  }

  if(*msg == '\0' || *msg != ',')
    return NULL; /* parse error */

  *msg = '\0';
  msg++;

  geo->longitude = msg;

  for(;; msg++) {
    if(!(isdigit(*msg) || *msg == '.' || *msg == '-'))
      break;
  }

  if(*msg == '\0') {
    geo->uncertainty = NULL;

    return geo;
  }
  else if (*msg == ';') {
    *msg = '\0';
    msg++;

    geo->uncertainty = msg;

    for(;; msg++) {
      if(!isdigit(*msg))
        break;
    }

    if(*msg != '\0')
      return NULL;

    return geo;
  }

  return NULL;
}

static gboolean msg_cb(PurpleAccount *account, const char *who,
                       char **message, PurpleConversation *conv,
                       PurpleMessageFlags flags)
{
  UNUSED(account);
  UNUSED(who);
  UNUSED(conv);
  UNUSED(flags);

  struct geouri geo;
  char *parsed_message = strdup(*message);
  char *formatted1, *formatted2;

  if(!parse_geouri(&geo, parsed_message))
    goto EXIT;

  formatted1 = g_strreplacefmt(uri_format, '%', 'f', geo.latitude);
  formatted2 = g_strreplacefmt(formatted1, '%', 'l', geo.longitude);
  free(formatted1);
  formatted1 = g_strreplacefmt(html_format, '%', 'u', formatted2);
  free(formatted2);

  *message = formatted1;

EXIT:
  free(parsed_message);
  return FALSE;
}

static PurplePluginPrefFrame * get_plugin_pref_frame(PurplePlugin *plugin)
{
  UNUSED(plugin);

  PurplePluginPrefFrame *frame;
  PurplePluginPref      *pref;

  frame = purple_plugin_pref_frame_new();

  /* uri-format */
  pref = purple_plugin_pref_new_with_label("Format of the URI generated to visualize the location.\n"
                                           "Use %f for latitude and %l for longitude.");
  purple_plugin_pref_frame_add(frame, pref);

  pref = purple_plugin_pref_new_with_name_and_label(PLUGIN_PREF "/uri-format", "URI format");
  purple_plugin_pref_frame_add(frame, pref);

  /* uri-format */
  pref = purple_plugin_pref_new_with_label("Format of the HTML tag generated.\n"
                                           "Use %u for the location URI.");
  purple_plugin_pref_frame_add(frame, pref);

  pref = purple_plugin_pref_new_with_name_and_label(PLUGIN_PREF "/html-format", "HTML format");
  purple_plugin_pref_frame_add(frame, pref);

  return frame;
}

static gboolean plugin_load(PurplePlugin *plugin)
{
  void *conv;

  uri_format  = purple_prefs_get_string(PLUGIN_PREF "/uri-format");
  html_format = purple_prefs_get_string(PLUGIN_PREF "/html-format");

  if(!uri_format)
    uri_format  = DEFAULT_URI_FORMAT;
  if(!html_format)
    html_format = DEFAULT_HTML_FORMAT;

  conv = purple_conversations_get_handle();
  purple_signal_connect(conv, "writing-im-msg", plugin,
                        PURPLE_CALLBACK(msg_cb), NULL);
  purple_signal_connect(conv, "writing-chat-msg", plugin,
                        PURPLE_CALLBACK(msg_cb), NULL);

  purple_debug_info(NAME, "plugin initialized...\n");

  return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin)
{
  UNUSED(plugin);

  purple_debug_info(NAME, "plugin destroyed...\n");

  return TRUE;
}

static void init_plugin(PurplePlugin *plugin)
{
  UNUSED(plugin);

  const struct pref_string {
    const char *name;
    const char *value;
  } prefs_add_string[] = {
    { PLUGIN_PREF "/uri-format", DEFAULT_URI_FORMAT },
    { PLUGIN_PREF "/html-format", DEFAULT_HTML_FORMAT },
    { NULL, NULL }
  };

  /* add preferences */
  purple_prefs_add_none(PLUGIN_PREF);
  for(const struct pref_string *s = prefs_add_string ; s->name ; s++)
    purple_prefs_add_string(s->name, s->value);
}

PURPLE_INIT_PLUGIN(geouri, init_plugin, info)
