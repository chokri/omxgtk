/*
                          omxgtk 

  omxgtk - a slim gtk frontend for omxplayer
 
  This program is free software: you can redistribute it and/or modify 
  it under the terms of the GNU General Public License as published by 
  the Free Software Foundation, either version 3 of the License, or 
  (at your option) any later version. 
              
  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
  GNU General Public License for more details. 

  You should have received a copy of the GNU General Public License 
  along with this program.  If not, see <http://www.gnu.org/licenses/>. 

  Copyright 2013 Ralph Glass                                        */ 

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

struct timeval t_start, t_end;

struct status{
        int playing;
        int toolbar_height;
        gboolean fullscreen;
        gboolean fixfullscreen;
        char* url;
        } omxgtk;
        
static void quit_omxplayer()
{
        omxgtk.playing = 0;
        gettimeofday(&t_end, NULL);
        system("echo -n q > /tmp/omxgtk_cmd");
}

static gint omxplayer(GtkWidget* window, char* arg)
{
        if (omxgtk.fixfullscreen == FALSE) 
                sleep(1);
        int seconds = t_end.tv_sec - t_start.tv_sec;
        omxgtk.playing = 1;
        gint x,y,w,h,d;
        gint xpos, ypos;
        GdkWindow* gdkwin = gtk_widget_get_window(GTK_WIDGET(window));
        gdk_window_set_keep_above(window->window, TRUE);
        gdk_window_get_geometry(gdkwin ,&x ,&y ,&w ,&h, &d);
        gdk_window_get_position(window->window,&xpos,&ypos);
        ypos = ypos + omxgtk.toolbar_height;
        h = h - omxgtk.toolbar_height;
        gchar* winstring = g_strdup_printf("%i %i %i %i", xpos,ypos,xpos+w,ypos+h);
        int pos = seconds - 7;
        if (pos < 0)
                pos = 0;
        gchar* posstring = g_strdup_printf("%i", pos);
        char* win_option = "--win";
        if (omxgtk.fullscreen == TRUE)
                win_option = NULL;
        char* script = "omxpipe.sh";
        char* scriptpath = "/usr/local/bin/omxpipe.sh";
        if (omxgtk.fixfullscreen == TRUE){
                win_option = NULL;
                script = "omxpipeonce.sh";
                scriptpath = "/usr/local/bin/omxpipeonce.sh";
        }
        
        int r = 0;
        r = fork();
        if (r > 0) {
                 execl(scriptpath, script, "--pos", posstring,
                                            arg,
                                            win_option,winstring,
                                           "> /tmp/omxgtk.log",
                                            NULL);  
                return TRUE;
        } else {
                system("echo -n pp > /tmp/omxgtk_cmd");
                return TRUE;
        }
}

static void destroy(GtkWidget* widget, GtkWidget* window)
{
        system("rm -f /tmp/omxgtk_cmd");
        system("rm -f /tmp/mystdin");
        quit_omxplayer();
        system("killall omxplayer.bin");
        gtk_main_quit();
}

static gint configure_event(GtkWidget* window, GdkEvent* event, gchar* arg)
{
        if (arg == NULL)
                return 0;
        static int counter = 0;
        if ( counter != 0 ){
                if (omxgtk.playing == 1){
                        quit_omxplayer();
                }
                if (omxgtk.playing == 0){
                        omxplayer(window, arg);
                        return TRUE;
                }
        }
        counter++;
        return FALSE;
}

static void clicked(GtkWidget* widget, GtkWidget* window)
{
        system("echo -n p > /tmp/omxgtk_cmd");
}

static void omxgtk_play(GtkWidget* widget, gpointer* data)
{
        system("echo -n p > /tmp/omxgtk_cmd");
}

static void omxgtk_pause(GtkWidget* widget, gpointer* data)
{
        system("echo -n p > /tmp/omxgtk_cmd");
}

static void omxgtk_volume_up(GtkWidget* widget, gpointer* data)
{
        system("echo -n + > /tmp/omxgtk_cmd");
}

static void omxgtk_volume_down(GtkWidget* widget, gpointer* data)
{
        system("echo -n - > /tmp/omxgtk_cmd");
}

static void omxgtk_forward(GtkWidget* widget, gpointer* data)
{
        system("echo -n $'\x1b\x5b\x43' > /tmp/omxgtk_cmd");
}

static void omxgtk_rewind(GtkWidget* widget, gpointer* data)
{
        system("echo -n $'\x1b\x5b\x44' > /tmp/omxgtk_cmd");
}

static void omxgtk_key_pressed(GtkWidget* widget, GdkEventKey* event, char* arg)
{
        if (event->type == GDK_KEY_PRESS){

                switch (event->keyval){
                case GDK_f:
                        if (omxgtk.fixfullscreen == FALSE){
                                omxgtk.fullscreen = !omxgtk.fullscreen;
                                configure_event(widget, NULL, arg);
                        }
                        break;
                case GDK_p:
                        omxgtk_pause(NULL,NULL);
                        break;
                case GDK_KEY_plus:
                        omxgtk_volume_up(NULL,NULL);
                        break;
                case GDK_KEY_minus:
                        omxgtk_volume_down(NULL,NULL);
                        break;
                case GDK_Right:
                        omxgtk_forward(NULL, NULL);
                        break;
                case GDK_Left:
                        omxgtk_rewind(NULL, NULL);
                        break;
                case GDK_q:
                        destroy(widget,NULL); 
                        break;
                }
        }
}

static void create_OmxView(char* arg)
{
        gint startup = 1;
        GtkWidget* window =  gtk_window_new(GTK_WINDOW_TOPLEVEL);
        
        GtkWidget* vbox =    gtk_vbox_new(FALSE, 0);
        GtkWidget* toolbar = gtk_toolbar_new();
        GtkToolItem* item;

        item = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Play" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK(omxgtk_play),NULL);

        item = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PAUSE);
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Pause" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK(omxgtk_play),NULL);

        item = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_FORWARD);
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Forward" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK(omxgtk_forward),NULL);

        item = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_REWIND);
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Rewind" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK(omxgtk_rewind ),NULL);

        item = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Rewind" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK(omxgtk_volume_up),NULL);

        item = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Rewind" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK(omxgtk_volume_down),NULL);

        GtkWidget* button;
        button = gtk_button_new_with_label("");
        gtk_window_set_default_size(GTK_WINDOW(window), 960,600 );

        if (omxgtk.fixfullscreen == FALSE){
                gtk_container_add(GTK_CONTAINER(window), vbox);
                gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
                gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
        }
        if (omxgtk.fixfullscreen == TRUE){
                gtk_window_set_default_size(GTK_WINDOW(window), 640, omxgtk.toolbar_height);
                gtk_window_fullscreen(GTK_WINDOW(window));
        }

        gtk_widget_grab_focus(GTK_WIDGET(window)); 
        gtk_widget_show_all(window);
        omxgtk.toolbar_height = toolbar->allocation.height;
        g_signal_connect(window, "destroy", G_CALLBACK(destroy), window);
        g_signal_connect(window, "delete-event", G_CALLBACK(destroy), window);
        g_signal_connect(button, "clicked", G_CALLBACK(clicked), window);
        g_signal_connect(window, "configure-event", G_CALLBACK(configure_event), arg);
        g_signal_connect(window, "key-press-event", G_CALLBACK(omxgtk_key_pressed), arg);
        gettimeofday(&t_start, NULL);
        return;
}

static void init_fifo()
{
        system("rm -f /tmp/omxgtk_cmd");
        system("mkfifo /tmp/omxgtk_cmd");
}

static char* omxgtk_init(int argc, char* argv[])
{
        /* option --window only usefull if omxplayer version supports --win */
        /* fixfullscreen default for now (--win avail for testing only)     */
        char* result = NULL;
        omxgtk.fixfullscreen = TRUE;
        for (int i = 1; i < argc; i++){
                if (argv[i] != NULL){
                        if ( strncmp(argv[i], "--window", 8) == 0 )
                                omxgtk.fixfullscreen = FALSE;
                        if ( strncmp(argv[i], "-", 1) != 0 )
                                result = argv[i];
                }
        }
        return result;
}

int main(int argc, char* argv[])
{
        char* media;
        gtk_init(&argc, &argv);
        media = omxgtk_init(argc, argv);
        init_fifo();
        create_OmxView(media);
        gtk_main();
        return 0;
}
