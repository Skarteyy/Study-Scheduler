#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[37];
    int type;
    int pages;
}Book;

typedef struct { 
    char name[37];
    float duration;
}Video;

typedef struct {
    GtkWidget *button;
    GtkWidget *popover;
} ButtonPopoverData;

void save_entries(GtkWidget *widget, gpointer data);
void entry_window();
void remove_widget(GtkWidget *widget, gpointer data);
void save_dates(GtkWidget *widget, gpointer data);
void book_selection(GtkWidget *window);
void add_row(GtkWidget *button, gpointer data);
void remove_all_widgets(GtkWidget *window);
void on_type_combo_changed(GtkComboBox *combo);
void save_materials(GtkWidget *button, gpointer data);
void toggle_theme(GtkButton *button, gpointer data);
void test_switch(GtkWidget *widget, gpointer data);
void result_screen(float decimal_hours);
void show_books_of_type(GtkGrid *grid, Book **books_array, int type, int *row, float hours);
void show_videos(GtkGrid *grid, Video **videos_array, int *row, float hours);
void handle_save(GtkWidget *button, gpointer user_data);
void handle_date_save(GtkWidget *button, gpointer user_data);
void show_popover(GtkWidget *sender, GdkEvent *event, gpointer data);
void apply_popover_style(GtkWidget *popover);
static void window_state_event_cb(GtkWidget *window, GdkEventWindowState *event, gpointer data);
int day_counter(int day1, int month1, int year1, int day2, int month2, int year2);

#endif