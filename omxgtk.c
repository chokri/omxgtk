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

struct timeval t_start, t_end;
static int playing = 0;
static gboolean fullscreen = FALSE;

static void quit_omxplayer()
{
        playing = 0;
        gettimeofday(&t_end, NULL);
        system("echo -n q > /tmp/omxgtk_cmd");
}

static gint omxplayer(GtkWidget* window, char* arg)
{
        gdk_window_flush(window->window);
        sleep(1);
        int seconds = t_end.tv_sec - t_start.tv_sec;
        playing = 1;
        gint x,y,w,h,d;
        gint xpos, ypos;
        GdkWindow* gdkwin = gtk_widget_get_window(GTK_WIDGET(window));
        gdk_window_set_keep_above(window->window, TRUE);
        gdk_window_get_geometry(gdkwin ,&x ,&y ,&w ,&h, &d);
        gdk_window_get_position(window->window,&xpos,&ypos);
        gchar* winstring = g_strdup_printf("%i %i %i %i", xpos,ypos,xpos+w,ypos+h);
        int pos = seconds - 6;
        if (pos < 0)
                pos = 0;
        gchar* posstring = g_strdup_printf("%i", pos);
        char* win_option = "--win";
        if (fullscreen == TRUE){
                win_option = NULL;};

        int r = 0;
        r = fork();
        if (r > 0) {
                 execl("/usr/local/bin/omxpipe.sh","omxpipe.sh", 
                                                   "--pos",posstring,
                                                   arg,
                                                   win_option,winstring,
                                                   NULL);  
                return TRUE;
        } else {
                system("echo -n p > /tmp/omxgtk_cmd");
                system("echo -n p > /tmp/omxgtk_cmd");
                return TRUE;
        }
}

static void destroy(GtkWidget* widget, GtkWidget* window)
{
        system("rm -f /tmp/omxgtk_cmd");
        quit_omxplayer();
        system("killall omxplayer.bin");
        gtk_main_quit();
}

static gint configure_event(GtkWidget* window, GdkEvent* event, gchar* arg)
{
        static int counter = 0;
        g_print("event fired!\n");
        if ( counter != 0 ){
                if (playing == 1)
                        quit_omxplayer();
                if (playing == 0){
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

static void omxgtk_key_pressed(GtkWidget* widget, GdkEventKey* event, char* arg)
{
        if (event->type == GDK_KEY_PRESS){

                switch ( event -> keyval )
                {
                case GDK_f:
                        fullscreen = !fullscreen;
                        configure_event(widget, NULL, arg);
                        break;
                case GDK_p:
                        system("echo -n p > /tmp/omxgtk_cmd");
                        break;
                case GDK_KEY_space:
                        system("echo -n p > /tmp/omxgtk_cmd");
                        break;
                case GDK_KEY_plus:
                        system("echo -n + > /tmp/omxgtk_cmd");
                        break;
                case GDK_KEY_minus:
                        system("echo -n - > /tmp/omxgtk_cmd");
                        break;
                case GDK_Right:
                        system("echo -n $'\x1b\x5b\x43' > /tmp/omxgtk_cmd");
                        break;
                case GDK_Left:
                        system("echo -n $'\x1b\x5b\x44' > /tmp/omxgtk_cmd");
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
        GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget* button;
        button = gtk_button_new_with_label("");
        gtk_window_set_default_size(GTK_WINDOW(window), 960,600 );
        gtk_container_add(GTK_CONTAINER(window),button);
        gtk_widget_show_all(window);
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

int main(int argc, char* argv[])
{
        gtk_init(&argc, &argv);
        init_fifo();
        create_OmxView(argv[1]);
        gtk_main();
        return 0;
}
