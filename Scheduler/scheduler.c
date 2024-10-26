#include "scheduler.h"

GtkWidget *window, *scrolled_window;
GtkWidget *pages_entry, *questions_entry, *summary_entry,  *write_entry, *playspeed_combo;
GtkWidget *current_date_day, *current_date_month, *current_date_year, *deadline_day, *deadline_month, *deadline_year;
GtkWidget *username_entry, *password_entry;
GtkWidget *header_label;
GtkWidget *save_button;
gint x, y, size_x, size_y;
gboolean is_dark_theme = FALSE;
GtkCssProvider *provider;

int day_data, month_data, year_data;
int num_rows = 0;
int pages, questions, sum_pages, writing;
int total_days = 0;
int num_videos = 0;
int num_books = 0;
int total_reading_pages = 0;
int total_test_pages = 0;
int total_sum_pages = 0;
int total_writing_pages = 0;
float total_duration = 0.0;
float playspeed_f;
float total_mins;

gboolean is_questions = FALSE; 

Book** books_array = NULL;
Video** videos_array = NULL;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    size_x = 1250;
    size_y = 900;
    
    entry_window();
    gtk_main();

    return 0;
}

void entry_window() {

    char line[256];
    int read_data, test_data, sum_data, write_data;
    float playback_data;

    FILE *fp = fopen("UserData","r");
    if (fp == NULL){
        printf("Error opening file\n");
        return;
    }
    
    fgets(line, sizeof(line), fp);
    sscanf(line, "read: %d", &read_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "test: %d", &test_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "sum: %d", &sum_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "write: %d", &write_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "playback: %f", &playback_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "day: %d",&day_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "month: %d",&month_data);

    fgets(line, sizeof(line), fp);
    sscanf(line, "year: %d",&year_data);
    
    const gchar *read_data_str = g_strdup_printf("%d", read_data);
    const gchar *test_data_str = g_strdup_printf("%d", test_data);
    const gchar *sum_data_str = g_strdup_printf("%d", sum_data);
    const gchar *write_data_str = g_strdup_printf("%d", write_data);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    provider = gtk_css_provider_new();
    const gchar *css = "window { background-color: #C0C0C0; } "
                        "label { color: #000000; } "
                        "button label { color: #FFFFFF; }";
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_window_set_title(GTK_WINDOW(window), "Study Scheduler");
    gtk_container_set_border_width(GTK_CONTAINER(window), 200);

    gtk_window_set_default_size(GTK_WINDOW(window), size_x, size_y);
    gtk_window_move(GTK_WINDOW(window), 400, 300);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid); 

    header_label = gtk_label_new("                                                      Numbers That Can Be Done\n                                                                  In One Hour");
    gtk_grid_attach(GTK_GRID(grid), header_label, 0, 0, 2, 1);

    GtkCssProvider *header_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(header_provider, "label { font-size: 20px; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(header_label), GTK_STYLE_PROVIDER(header_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *pages_label = gtk_label_new("Pages of Reading:");
    gtk_grid_attach(GTK_GRID(grid), pages_label, 0, 2, 1, 1);
    pages_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), pages_entry, 1, 2, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(pages_entry), read_data_str);

    GtkWidget *questions_label = gtk_label_new("Test Book Pages:");
    gtk_grid_attach(GTK_GRID(grid), questions_label, 0, 3, 1, 1);
    questions_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), questions_entry, 1, 3, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(questions_entry), test_data_str);

    GtkWidget *PQbutton = gtk_button_new_with_label("Pages/Questions");
    gtk_grid_attach(GTK_GRID(grid), PQbutton, 2, 3, 1, 1);

    GtkWidget *summary_label = gtk_label_new("Pages To Summarize:");
    gtk_grid_attach(GTK_GRID(grid), summary_label, 0, 4, 1, 1);
    summary_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), summary_entry, 1, 4, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(summary_entry), sum_data_str);

    GtkWidget *write_label = gtk_label_new("Full Writing:");
    gtk_grid_attach(GTK_GRID(grid), write_label, 0, 5, 1, 1);
    write_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), write_entry, 1, 5, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(write_entry), write_data_str);

    GtkWidget *playspeed_label = gtk_label_new("Video Playback Speed:"); 
    gtk_grid_attach(GTK_GRID(grid), playspeed_label, 0, 6, 1, 1);

    playspeed_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(playspeed_combo), "0.5x", "0.5x");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(playspeed_combo), "1x", "1x");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(playspeed_combo), "1.25x", "1.25x");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(playspeed_combo), "1.5x", "1.5x");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(playspeed_combo), "1.75x", "1.75x");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(playspeed_combo), "2x", "2x");

    gtk_grid_attach(GTK_GRID(grid), playspeed_combo, 1, 6, 1, 1);
    
    int playback_index = playback_data*4;
    switch (playback_index)
    {
    case 2:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 0);
        break;
    case 4:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 1);
        break;
    case 5:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 2);
        break;
    case 6:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 3);
        break;
    case 7:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 4);
        break;
    case 8:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 5);
        break;
    default:
        gtk_combo_box_set_active(GTK_COMBO_BOX(playspeed_combo), 0);
        break;
    }

    GtkWidget *button = gtk_button_new_with_label("Save and Continue");
    gtk_grid_attach(GTK_GRID(grid), button, 1, 8, 1, 1);

    GtkWidget *themeButton = gtk_button_new_with_label("Theme");
    gtk_grid_attach(GTK_GRID(grid), themeButton, 1, 7, 1, 1); 

    g_signal_connect(button, "clicked", G_CALLBACK(save_entries), NULL);
    g_signal_connect(themeButton, "clicked", G_CALLBACK(toggle_theme), NULL);
    g_signal_connect(PQbutton, "clicked", G_CALLBACK(test_switch), questions_label);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    fclose(fp);
}

void save_entries(GtkWidget *widget, gpointer data) {
    const gchar *pages_text = gtk_entry_get_text(GTK_ENTRY(pages_entry));
    const gchar *questions_text = gtk_entry_get_text(GTK_ENTRY(questions_entry));
    const gchar *summary_text = gtk_entry_get_text(GTK_ENTRY(summary_entry));
    const gchar *write_text = gtk_entry_get_text(GTK_ENTRY(write_entry));
    const gchar *playspeed_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(playspeed_combo));
    char line[256];
    

    FILE *fp = fopen("UserData","w");
    if (fp == NULL){
        printf("Error opening file\n");
        return;
    }

    const gchar *day_data_str = g_strdup_printf("%d", day_data);
    const gchar *month_data_str = g_strdup_printf("%d", month_data);
    const gchar *year_data_str = g_strdup_printf("%d", year_data);

    sscanf(pages_text, "%d", &pages);
    sscanf(questions_text, "%d", &questions);
    sscanf(summary_text, "%d", &sum_pages);
    sscanf(write_text, "%d", &writing);
    sscanf(playspeed_text, "%fx", &playspeed_f);
    
    fprintf(fp, "read: %d\n",pages);
    fprintf(fp, "test: %d\n",questions);
    fprintf(fp, "sum: %d\n",sum_pages);
    fprintf(fp, "write: %d\n",writing);
    fprintf(fp, "playback: %f\n",playspeed_f);

    fclose(fp);
    
    gtk_container_foreach(GTK_CONTAINER(window), remove_widget, NULL);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL); 
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid);

    header_label = gtk_label_new("Please Enter Requested Dates");
    gtk_grid_attach(GTK_GRID(grid), header_label, 1, 0, 2, 1);

    GtkWidget *day_label = gtk_label_new("Day");
    gtk_grid_attach(GTK_GRID(grid), day_label, 1, 1, 1, 1);
    GtkWidget *month_label = gtk_label_new("Month");
    gtk_grid_attach(GTK_GRID(grid), month_label, 2, 1, 1, 1);
    GtkWidget *year_label = gtk_label_new("Year");
    gtk_grid_attach(GTK_GRID(grid), year_label, 3, 1, 1, 1);

    GtkWidget *current_date_label = gtk_label_new("Current Date:");
    gtk_grid_attach(GTK_GRID(grid), current_date_label, 0, 2, 1, 1);
    current_date_day = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), current_date_day, 1, 2, 1, 1);
    current_date_month = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), current_date_month, 2, 2, 1, 1);
    current_date_year = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), current_date_year, 3, 2, 1, 1);

    GtkWidget *deadline_label = gtk_label_new("Deadline:");
    gtk_grid_attach(GTK_GRID(grid), deadline_label, 0, 3, 1, 1);
    deadline_day = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), deadline_day, 1, 3, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(deadline_day), day_data_str);

    deadline_month = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), deadline_month, 2, 3, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(deadline_month), month_data_str);

    deadline_year = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), deadline_year, 3, 3, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(deadline_year), year_data_str);

    GtkWidget *button = gtk_button_new_with_label("Save");
    gtk_grid_attach(GTK_GRID(grid), button, 1, 5, 1, 1);

    gtk_container_set_border_width(GTK_CONTAINER(grid), 5);
    
    GtkCssProvider *header_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(header_provider, "label { font-size: 20px; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(header_label), GTK_STYLE_PROVIDER(header_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_signal_connect(button, "clicked", G_CALLBACK(handle_date_save), grid); 
    
    gtk_widget_show_all(window);
    
}

void save_dates(GtkWidget *widget, gpointer data){
    const gchar *curr_day_text = gtk_entry_get_text(GTK_ENTRY(current_date_day));
    const gchar *curr_month_text = gtk_entry_get_text(GTK_ENTRY(current_date_month));
    const gchar *curr_year_text = gtk_entry_get_text(GTK_ENTRY(current_date_year));

    const gchar *deadlines_day_text = gtk_entry_get_text(GTK_ENTRY(deadline_day));
    const gchar *deadlines_month_text = gtk_entry_get_text(GTK_ENTRY(deadline_month));
    const gchar *deadlines_year_text = gtk_entry_get_text(GTK_ENTRY(deadline_year));

    int curr_day,curr_month,curr_year,deadlines_day,deadlines_month,deadlines_year;
    
    FILE *fp = fopen("UserData","a");
    if (fp == NULL){
        printf("Error opening file\n");
        return;
    }

    sscanf(curr_day_text, "%d", &curr_day);
    sscanf(curr_month_text, "%d", &curr_month);
    sscanf(curr_year_text, "%d", &curr_year);
    sscanf(deadlines_day_text, "%d", &deadlines_day);
    sscanf(deadlines_month_text, "%d", &deadlines_month);
    sscanf(deadlines_year_text, "%d", &deadlines_year);
   
    fprintf(fp, "day: %d\n", deadlines_day);
    fprintf(fp, "month: %d\n", deadlines_month);
    fprintf(fp, "year: %d\n", deadlines_year);

    fclose(fp);

    total_days = day_counter(curr_day,curr_month,curr_year,deadlines_day,deadlines_month,deadlines_year);

    if(curr_year > deadlines_year || (curr_year == deadlines_year && curr_month > deadlines_month) || (curr_year == deadlines_year && curr_month == deadlines_month && curr_day > deadlines_day)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid date entry. Please try again.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    else{
        book_selection(window);
    }
}

void book_selection(GtkWidget *window) {
    gtk_container_foreach(GTK_CONTAINER(window), remove_widget, NULL);
    
    scrolled_window = gtk_scrolled_window_new(NULL, NULL); 
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    gtk_container_set_border_width(GTK_CONTAINER(window), 180);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid); 
    
    GtkWidget *book_label = gtk_label_new("Book:");
    gtk_widget_set_size_request(book_label, 50, -1);
    gtk_grid_attach(GTK_GRID(grid), book_label, 2, 0, 1, 1);

    GtkWidget *book_entry = gtk_entry_new();
    gtk_widget_set_size_request(book_entry, 50, -1);
    gtk_grid_attach(GTK_GRID(grid), book_entry, 3, 0, 1, 1);

    GtkWidget *pages_label = gtk_label_new("Pages:");
    gtk_widget_set_size_request(pages_label, 50, -1);
    gtk_grid_attach(GTK_GRID(grid), pages_label, 4, 0, 1, 1);

    GtkWidget *pages_entry = gtk_entry_new();
    gtk_widget_set_size_request(pages_entry, 50, -1); 
    gtk_grid_attach(GTK_GRID(grid), pages_entry, 5, 0, 1, 1);

    GtkWidget *type_label = gtk_label_new("Type:");
    gtk_widget_set_size_request(type_label, 50, -1);
    gtk_grid_attach(GTK_GRID(grid), type_label, 0, 0, 1, 1);

    GtkWidget *type_combo = gtk_combo_box_text_new();
    gtk_widget_set_size_request(type_combo, 50, -1);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Reading Book");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Test Book");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Summarizing");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Writing");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Video");
    gtk_grid_attach(GTK_GRID(grid), type_combo, 1, 0, 1, 1);

    
    g_object_set_data(G_OBJECT(type_combo), "book-label", book_label);
    g_object_set_data(G_OBJECT(type_combo), "pages-label", pages_label);

    g_signal_connect(type_combo, "changed", G_CALLBACK(on_type_combo_changed), type_combo);

    GtkWidget *add_button = gtk_button_new_with_label("+");
    gtk_widget_set_size_request(add_button, 30, -1);
    gtk_grid_attach(GTK_GRID(grid), add_button, 6, 0, 1, 1);

    save_button = gtk_button_new_with_label("Save");
    gtk_widget_set_size_request(save_button, 50, -1);
    gtk_grid_attach(GTK_GRID(grid), save_button, 7, 0, 1, 1);

    g_signal_connect(add_button, "clicked", G_CALLBACK(add_row), grid);
    g_signal_connect(save_button, "clicked", G_CALLBACK(handle_save), grid);
    
    gtk_widget_show_all(window);
}

void add_row(GtkWidget *button, gpointer data) {
    GtkWidget *grid = GTK_WIDGET(data);
    num_rows++;
    
    if (save_button) {
        gtk_widget_destroy(save_button);
        save_button = NULL;
        save_button = gtk_button_new_with_label("Save");
        gtk_widget_set_size_request(save_button, 50, -1);
        gtk_grid_attach(GTK_GRID(grid), save_button, 7, num_rows, 1, 1);
    }
    
    GtkWidget *book_label = gtk_label_new("Book:");
    gtk_grid_attach(GTK_GRID(grid), book_label, 2, num_rows, 1, 1);

    GtkWidget *book_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), book_entry, 3, num_rows, 1, 1);

    GtkWidget *pages_label = gtk_label_new("Pages:");
    gtk_grid_attach(GTK_GRID(grid), pages_label, 4, num_rows, 1, 1);

    GtkWidget *pages_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), pages_entry, 5, num_rows, 1, 1);

    GtkWidget *type_label = gtk_label_new("Type:");
    gtk_grid_attach(GTK_GRID(grid), type_label, 0, num_rows, 1, 1);

    GtkWidget *type_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Reading Book");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Test Book");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Summarizing");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Writing");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo), NULL, "Video");
    gtk_grid_attach(GTK_GRID(grid), type_combo, 1, num_rows, 1, 1);

    g_object_set_data(G_OBJECT(type_combo), "book-label", book_label);
    g_object_set_data(G_OBJECT(type_combo), "pages-label", pages_label);

    g_signal_connect(type_combo, "changed", G_CALLBACK(on_type_combo_changed), type_combo);
    g_signal_connect(save_button, "clicked", G_CALLBACK(handle_save), grid);

    gtk_container_remove(GTK_CONTAINER(grid), button);
    gtk_grid_attach(GTK_GRID(grid), button, 6, num_rows, 1, 1);

    gtk_widget_show_all(GTK_WIDGET(grid));
}

void save_materials(GtkWidget *button, gpointer data) { //TODO: There are arrays to free
    GtkWidget *grid = GTK_WIDGET(data);
    gint i;
    GtkWidget *type_combo, *book_entry, *pages_entry;
    gchar *selected_text, *book_text, *pages_text;
    
    float decimal_hours;

    if (books_array == NULL && num_books == 0) {
        books_array = (Book**)malloc(sizeof(Book*));
        if (books_array == NULL) {
            printf("Error: malloc failed\n");
            return; // Return an error code
        }
    }
    for (i = 0; i < num_rows+1; i++) {
        type_combo = gtk_grid_get_child_at(GTK_GRID(grid), 1, i);
        book_entry = gtk_grid_get_child_at(GTK_GRID(grid), 3, i);
        pages_entry = gtk_grid_get_child_at(GTK_GRID(grid), 5, i);

        selected_text = g_strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(type_combo)));
        book_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(book_entry)));
        pages_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(pages_entry)));
        if (g_strcmp0(selected_text, "Video") == 0) {
            videos_array = (Video**)realloc(videos_array, (num_videos + 1) * (sizeof(Video*)));
            videos_array[num_videos] = (Video*)malloc(sizeof(Video));
            strcpy(videos_array[num_videos]->name, book_text);
            videos_array[num_videos]->duration = atof(pages_text);

            total_duration += videos_array[num_videos]->duration;
            num_videos++;
        } 
        else {
            books_array = (Book**)realloc(books_array, (num_books + 1) * sizeof(Book*));
            books_array[num_books] = (Book*)malloc(sizeof(Book));
            strcpy(books_array[num_books]->name, book_text);
            books_array[num_books]->pages = atoi(pages_text);

            if(g_strcmp0(selected_text, "Reading Book") == 0){
                books_array[num_books]->type = 0;
                total_reading_pages += books_array[num_books]->pages;
            }
            else if(g_strcmp0(selected_text, "Test Book") == 0){
                books_array[num_books]->type = 1;
                total_test_pages += books_array[num_books]->pages;
            }
            else if(g_strcmp0(selected_text, "Summarizing") == 0){
                books_array[num_books]->type = 2;
                total_sum_pages += books_array[num_books]->pages;
            }
            else if(g_strcmp0(selected_text, "Writing") == 0){
                books_array[num_books]->type = 3;
                total_writing_pages += books_array[num_books]->pages;
            }
            else{
                printf("NULL\n");
            }
            
            num_books++;
        }
    
        g_free(selected_text);
        g_free(book_text);
        g_free(pages_text);  
    }
    
    result_screen(decimal_hours);
}

void result_screen(float decimal_hours){
    float hours_of_reading = 0;
    float hours_of_test = 0;
    float hours_of_video = 0;
    float hours_of_sum = 0;
    float hours_of_writing = 0;

    int hours,mins,i;
    int read_row = 0;
    int test_row = 0;
    int sum_row = 0;
    int wri_row = 0;
    int video_row = 0;

    if (pages)
        hours_of_reading = (float)total_reading_pages/(total_days*pages);
    if (questions)
        hours_of_test = (float)total_test_pages/(total_days*questions);
    if (sum_pages)
        hours_of_sum = (float)total_sum_pages/(total_days*sum_pages);
    if (writing)
        hours_of_writing = (float)total_writing_pages/(total_days*writing);
    if (playspeed_f)
        hours_of_video = (total_duration/(total_days*playspeed_f))/60;
    
    decimal_hours = hours_of_reading + hours_of_test + hours_of_sum + hours_of_writing + hours_of_video;

    total_mins = decimal_hours * 60;
    hours = total_mins / 60;
    mins = total_mins - hours * 60;

    gtk_container_foreach(GTK_CONTAINER(window), remove_widget, NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *newgrid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(newgrid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(newgrid), FALSE);

    GtkWidget *reading_button = gtk_button_new_with_label("Show");
    GtkWidget *questions_button = gtk_button_new_with_label("Show");
    GtkWidget *summary_button = gtk_button_new_with_label("Show");
    GtkWidget *writing_button = gtk_button_new_with_label("Show");
    GtkWidget *video_button = gtk_button_new_with_label("Show");    

    gtk_widget_set_hexpand(reading_button, FALSE);
    gtk_widget_set_valign(reading_button, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(reading_button, GTK_ALIGN_START);
    gtk_widget_set_size_request(reading_button, 30, -1);

    gtk_widget_set_hexpand(questions_button, FALSE);
    gtk_widget_set_valign(questions_button, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(questions_button, GTK_ALIGN_START);
    gtk_widget_set_size_request(questions_button, 30, -1);

    gtk_widget_set_hexpand(summary_button, FALSE);
    gtk_widget_set_valign(summary_button, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(summary_button, GTK_ALIGN_START);
    gtk_widget_set_size_request(summary_button, 30, -1);

    gtk_widget_set_hexpand(writing_button, FALSE);
    gtk_widget_set_valign(writing_button, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(writing_button, GTK_ALIGN_START);
    gtk_widget_set_size_request(writing_button, 30, -1);

    gtk_widget_set_hexpand(video_button, FALSE);
    gtk_widget_set_valign(video_button, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(video_button, GTK_ALIGN_START);
    gtk_widget_set_size_request(video_button, 30, -1);

    gtk_container_add(GTK_CONTAINER(box), newgrid);

    GtkWidget *result_label = gtk_label_new(NULL);
    gchar *result_text = g_strdup_printf("%d Hours %d Mins\n        Per Day\n", hours, mins);
    gtk_label_set_text(GTK_LABEL(result_label), result_text);
    g_free(result_text);
    gtk_widget_set_size_request(result_label, 200, -1);

    gtk_widget_set_hexpand(result_label, TRUE);
    gtk_label_set_xalign(GTK_LABEL(result_label), 0.4);
    gtk_grid_attach(GTK_GRID(newgrid), result_label, 1, 0, 1, 1);

    GtkWidget *empty_label = gtk_label_new(" ");
    gtk_grid_attach(GTK_GRID(newgrid), empty_label, 2, 0, 1, 1);

    GtkWidget *reading_popover = gtk_popover_new(reading_button);
    gtk_popover_set_modal(GTK_POPOVER(reading_popover), TRUE);

    GtkWidget *reading_popover_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(reading_popover_grid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(reading_popover_grid), FALSE);
    gtk_popover_set_position(GTK_POPOVER(reading_popover), GTK_POS_RIGHT);

    gtk_container_add(GTK_CONTAINER(reading_popover), reading_popover_grid);
    show_books_of_type(GTK_GRID(reading_popover_grid), books_array, 0, &read_row, hours_of_reading);
    gtk_widget_show_all(GTK_WIDGET(reading_popover_grid));

    GtkWidget *questions_popover = gtk_popover_new(questions_button);
    gtk_popover_set_modal(GTK_POPOVER(questions_popover), TRUE);

    GtkWidget *questions_popover_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(questions_popover_grid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(questions_popover_grid), FALSE);
    gtk_popover_set_position(GTK_POPOVER(questions_popover), GTK_POS_RIGHT);

    gtk_container_add(GTK_CONTAINER(questions_popover), questions_popover_grid);
    show_books_of_type(GTK_GRID(questions_popover_grid), books_array, 1, &test_row, hours_of_test);
    gtk_widget_show_all(GTK_WIDGET(questions_popover_grid));

    GtkWidget *summary_popover = gtk_popover_new(summary_button);
    gtk_popover_set_modal(GTK_POPOVER(summary_popover), TRUE);

    GtkWidget *summary_popover_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(summary_popover_grid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(summary_popover_grid), FALSE);
    gtk_popover_set_position(GTK_POPOVER(summary_popover), GTK_POS_RIGHT);

    gtk_container_add(GTK_CONTAINER(summary_popover), summary_popover_grid);
    show_books_of_type(GTK_GRID(summary_popover_grid), books_array, 2, &sum_row, hours_of_sum);
    gtk_widget_show_all(GTK_WIDGET(summary_popover_grid));

    GtkWidget *writing_popover = gtk_popover_new(writing_button);
    gtk_popover_set_modal(GTK_POPOVER(questions_popover), TRUE);

    GtkWidget *writing_popover_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(writing_popover_grid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(writing_popover_grid), FALSE);
    gtk_popover_set_position(GTK_POPOVER(writing_popover), GTK_POS_RIGHT);

    gtk_container_add(GTK_CONTAINER(writing_popover), writing_popover_grid);
    show_books_of_type(GTK_GRID(writing_popover_grid), books_array, 3, &wri_row, hours_of_writing);
    gtk_widget_show_all(GTK_WIDGET(writing_popover_grid));

    GtkWidget *video_popover = gtk_popover_new(video_button);
    gtk_popover_set_modal(GTK_POPOVER(video_popover), TRUE);

    GtkWidget *video_popover_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(video_popover_grid), FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(video_popover_grid), FALSE);
    gtk_popover_set_position(GTK_POPOVER(video_popover), GTK_POS_RIGHT);

    gtk_container_add(GTK_CONTAINER(video_popover), video_popover_grid);
    show_videos(GTK_GRID(video_popover_grid), videos_array, &video_row, hours_of_video);
    gtk_widget_show_all(GTK_WIDGET(video_popover_grid));

    ButtonPopoverData *reading_button_popover_data = g_malloc(sizeof(ButtonPopoverData));
    reading_button_popover_data->button = reading_button;
    reading_button_popover_data->popover = reading_popover;
    g_signal_connect_data(reading_button, "clicked", G_CALLBACK(show_popover), reading_button_popover_data, (GClosureNotify)g_free, 0);

    ButtonPopoverData *questions_button_popover_data = g_malloc(sizeof(ButtonPopoverData));
    questions_button_popover_data->button = questions_button;
    questions_button_popover_data->popover = questions_popover;
    g_signal_connect_data(questions_button, "clicked", G_CALLBACK(show_popover), questions_button_popover_data, (GClosureNotify)g_free, 0);

    ButtonPopoverData *summary_button_popover_data = g_malloc(sizeof(ButtonPopoverData));
    summary_button_popover_data->button = summary_button;
    summary_button_popover_data->popover = summary_popover;
    g_signal_connect_data(summary_button, "clicked", G_CALLBACK(show_popover), summary_button_popover_data, (GClosureNotify)g_free, 0);

    ButtonPopoverData *writing_button_popover_data = g_malloc(sizeof(ButtonPopoverData));
    writing_button_popover_data->button = writing_button;
    writing_button_popover_data->popover = writing_popover;
    g_signal_connect_data(writing_button, "clicked", G_CALLBACK(show_popover), writing_button_popover_data, (GClosureNotify)g_free, 0);

    ButtonPopoverData *video_button_popover_data = g_malloc(sizeof(ButtonPopoverData));
    video_button_popover_data->button = video_button;
    video_button_popover_data->popover = video_popover;
    g_signal_connect_data(video_button, "clicked", G_CALLBACK(show_popover), video_button_popover_data, (GClosureNotify)g_free, 0);

    hours = hours_of_reading;
    mins = hours_of_reading * 60 - hours * 60;
    GtkWidget *hours_of_reading_label = gtk_label_new(NULL);
    gchar *hours_of_reading_text = g_strdup_printf("reading\n%d Hours %d Mins", hours, mins);
    gtk_label_set_text(GTK_LABEL(hours_of_reading_label), hours_of_reading_text);
    g_free(hours_of_reading_text);
    if (hours + mins != 0){
        gtk_grid_attach(GTK_GRID(newgrid), hours_of_reading_label, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(newgrid), reading_button, 1, 1, 1, 1);
    }

    hours = hours_of_test;
    mins = hours_of_test * 60 - hours * 60;
    GtkWidget *hours_of_test_label = gtk_label_new(NULL);
    gchar *hours_of_test_text = g_strdup_printf("Questions\n%d Hours %d Mins ", hours, mins);
    gtk_label_set_text(GTK_LABEL(hours_of_test_label), hours_of_test_text);
    g_free(hours_of_test_text);
    if (hours + mins != 0){
        gtk_grid_attach(GTK_GRID(newgrid), hours_of_test_label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(newgrid), questions_button, 1, 2, 1, 1);
    }

    hours = hours_of_sum;
    mins = hours_of_sum * 60 - hours * 60;
    GtkWidget *hours_of_sum_label = gtk_label_new(NULL);
    gchar *hours_of_sum_text = g_strdup_printf("Summarizing\n%d Hours %d Mins", hours, mins);
    gtk_label_set_text(GTK_LABEL(hours_of_sum_label), hours_of_sum_text);
    g_free(hours_of_sum_text);
    if (hours + mins != 0){
        gtk_grid_attach(GTK_GRID(newgrid), hours_of_sum_label, 0, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(newgrid), summary_button, 1, 3, 1, 1);
    }

    hours = hours_of_writing;
    mins = hours_of_writing * 60 - hours * 60;
    GtkWidget *hours_of_writing_label = gtk_label_new(NULL);
    gchar *hours_of_writing_text = g_strdup_printf("Writing\n%d Hours %d Mins", hours, mins);
    gtk_label_set_text(GTK_LABEL(hours_of_writing_label), hours_of_writing_text);
    g_free(hours_of_writing_text);
    if (hours + mins != 0){
        gtk_grid_attach(GTK_GRID(newgrid), hours_of_writing_label, 0, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(newgrid), writing_button, 1, 4, 1, 1);
    }

    hours = hours_of_video;
    mins = hours_of_video * 60 - hours * 60;
    GtkWidget *hours_of_video_label = gtk_label_new(NULL);
    gchar *hours_of_video_text = g_strdup_printf("Video\n%d Hours %d Mins", hours, mins);
    gtk_label_set_text(GTK_LABEL(hours_of_video_label), hours_of_video_text);
    g_free(hours_of_video_text);
    if (hours + mins != 0){
        gtk_grid_attach(GTK_GRID(newgrid), hours_of_video_label, 0, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(newgrid), video_button, 1, 5, 1, 1);
    }

    GtkCssProvider *result_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(result_provider, "label { font-size: 36px; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(result_label), GTK_STYLE_PROVIDER(result_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "label { font-size: 20px; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(hours_of_reading_label), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_add_provider(gtk_widget_get_style_context(hours_of_test_label), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_add_provider(gtk_widget_get_style_context(hours_of_sum_label), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_add_provider(gtk_widget_get_style_context(hours_of_writing_label), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_add_provider(gtk_widget_get_style_context(hours_of_video_label), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    apply_popover_style(reading_popover);
    apply_popover_style(questions_popover);
    apply_popover_style(summary_popover);
    apply_popover_style(writing_popover);
    apply_popover_style(video_popover);

    gtk_grid_set_row_spacing(GTK_GRID(newgrid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(newgrid), 50);

    gtk_container_set_border_width(GTK_CONTAINER(window), 150);
    gtk_widget_set_hexpand(newgrid, TRUE);
    gtk_widget_set_vexpand(newgrid, TRUE);
    
    gtk_widget_show_all(window);

    for (i = 0; i < num_books; i++) {
        free(books_array[i]);
    }
    free(books_array);

    for (i = 0; i < num_videos; i++) {
        free(videos_array[i]);
    }
    free(videos_array);
}

void show_popover(GtkWidget *sender, GdkEvent *event, gpointer data) {
    ButtonPopoverData *button_popover_data = (ButtonPopoverData*) data;
    gtk_popover_popup(GTK_POPOVER(button_popover_data->popover));
}

void apply_popover_style(GtkWidget *popover) {
    GtkCssProvider *pop_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(pop_provider, "popover {background-color: grey; color: white; font-size: 18px;}", -1, NULL);
    GtkStyleContext *popover_style_context = gtk_widget_get_style_context(GTK_WIDGET(popover));
    gtk_style_context_add_provider(popover_style_context, GTK_STYLE_PROVIDER(pop_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(pop_provider);
}

int is_leap_year(int year){ 
    return (year % 4 == 0 && year % 100!= 0) || year % 400 == 0;
}

int days_in_month(int month, int year) {
    if (month == 2) {
        return is_leap_year(year)? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else {
        return 31;
    }
}

int day_counter(int day1, int month1, int year1, int day2, int month2, int year2){
    int days = 0;
    int i, j;
    
    if (year1 == year2) {
        // Calculate days from day1 to day2 in the same year
        for (int i = month1; i < month2; i++) {
            days += days_in_month(i, year1);
        }
        days += day2 - day1;
    } else {
        // Calculate days from day1 to end of month1
        for (i = day1; i <= days_in_month(month1, year1); i++) {
            days++;
        }

        // Calculate days for full months
        for (i = month1 + 1; i <= 12; i++) {
            days += days_in_month(i, year1);
        }
        for (i = year1 + 1; i < year2; i++) {
            if (is_leap_year(i)) {
                days += 366;
            } else {
                days += 365;
            }
        }
        for (i = 1; i < month2; i++) {
            days += days_in_month(i, year2);
        }

        // Calculate days from start of month2 to day2
        for (i = 1; i < day2; i++) {
            days++;
        }
    }
    return days;
}

float hours_per_day(float total_hours, int days){
    return total_hours/days;
}

void remove_widget(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(widget);
}

void remove_all_widgets(GtkWidget *window) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *child = children; child!= NULL; child = child->next) {
        gtk_container_remove(GTK_CONTAINER(window), GTK_WIDGET(child->data));
    }
    g_list_free(children);
}

void toggle_theme(GtkButton *button, gpointer data) {
    const gchar *css;

    if (is_dark_theme) {
        css = "window { background-color: #C0C0C0; } "
              "label { color: #000000; } "
              "button label { color: #FFFFFF; }";
        is_dark_theme = FALSE;
    } else {
        css = "window { background-color: #404040; } "
              "label { color: #FFFFFF; } "
              "button label { color: #FFFFFF; }";
        is_dark_theme = TRUE;
    }

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
}

void handle_date_save(GtkWidget *button, gpointer user_data) {
    GtkWidget *grid = (GtkWidget *)user_data;
    GtkWidget *current_date_day_entry, *current_date_month_entry, *current_date_year_entry;
    GtkWidget *deadline_day_entry, *deadline_month_entry, *deadline_year_entry;
    const gchar *current_date_day_text, *current_date_month_text, *current_date_year_text;
    const gchar *deadline_day_text, *deadline_month_text, *deadline_year_text;
    int current_date_day, current_date_month, current_date_year;
    int deadline_day, deadline_month, deadline_year;
    int is_valid = 1;

    // Get the entry widgets
    current_date_day_entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 2);
    current_date_month_entry = gtk_grid_get_child_at(GTK_GRID(grid), 2, 2);
    current_date_year_entry = gtk_grid_get_child_at(GTK_GRID(grid), 3, 2);
    deadline_day_entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 3);
    deadline_month_entry = gtk_grid_get_child_at(GTK_GRID(grid), 2, 3);
    deadline_year_entry = gtk_grid_get_child_at(GTK_GRID(grid), 3, 3);

    // Get the text from the entries
    current_date_day_text = gtk_entry_get_text(GTK_ENTRY(current_date_day_entry));
    current_date_month_text = gtk_entry_get_text(GTK_ENTRY(current_date_month_entry));
    current_date_year_text = gtk_entry_get_text(GTK_ENTRY(current_date_year_entry));
    deadline_day_text = gtk_entry_get_text(GTK_ENTRY(deadline_day_entry));
    deadline_month_text = gtk_entry_get_text(GTK_ENTRY(deadline_month_entry));
    deadline_year_text = gtk_entry_get_text(GTK_ENTRY(deadline_year_entry));

    // Convert the text to integers
    current_date_day = atoi(current_date_day_text);
    current_date_month = atoi(current_date_month_text);
    current_date_year = atoi(current_date_year_text);
    deadline_day = atoi(deadline_day_text);
    deadline_month = atoi(deadline_month_text);
    deadline_year = atoi(deadline_year_text);

    // Check if the dates are valid
    if (current_date_day < 1 || current_date_day > 31 || current_date_month < 1 || current_date_month > 12 || current_date_year < 1) {
        is_valid = 0;
    }
    if (deadline_day < 1 || deadline_day > 31 || deadline_month < 1 || deadline_month > 12 || deadline_year < 1) {
        is_valid = 0;
    }

    // If the dates are not valid, display a warning message
    if (!is_valid) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Error: Invalid date. Please enter a valid date.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        // If the dates are valid, call the save_dates function
        save_dates(button, user_data);
    }
}

void on_type_combo_changed(GtkComboBox *type_combo) {
    GtkWidget *book_label, *pages_label;
    gchar *selected_text;

    book_label = g_object_get_data(G_OBJECT(type_combo), "book-label");
    pages_label = g_object_get_data(G_OBJECT(type_combo), "pages-label");
    
    if (type_combo == NULL || book_label == NULL || pages_label == NULL) {
        g_print("One of the widgets is null\n");
        return;
    }

    selected_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(type_combo));
    if (g_strcmp0(selected_text, "Video") == 0) {
        gtk_label_set_text(GTK_LABEL(book_label), "Video:");
        gtk_label_set_text(GTK_LABEL(pages_label), "Duration(mins):");
    } 
    else if (g_strcmp0(selected_text, "Summarizing") == 0){
        gtk_label_set_text(GTK_LABEL(book_label), "Name:");
        gtk_label_set_text(GTK_LABEL(pages_label), "Pages:");
    }
    else if (g_strcmp0(selected_text, "Writing") == 0){
        gtk_label_set_text(GTK_LABEL(book_label), "Name:");
        gtk_label_set_text(GTK_LABEL(pages_label), "Pages:");
    }
    else {
        if (is_questions) {
            if (g_strcmp0(selected_text, "Test Book") == 0) {
                gtk_label_set_text(GTK_LABEL(book_label), "Test Book:");
                gtk_label_set_text(GTK_LABEL(pages_label), "Questions:");
            } else {
                gtk_label_set_text(GTK_LABEL(book_label), "Book:");
                gtk_label_set_text(GTK_LABEL(pages_label), "Pages:");
            }
        } else {
            gtk_label_set_text(GTK_LABEL(book_label), "Book:");
            gtk_label_set_text(GTK_LABEL(pages_label), "Pages:");
        }
    }
    g_free(selected_text);
}

void handle_save(GtkWidget *button, gpointer user_data) {
    GtkWidget *grid = (GtkWidget *)user_data;
    GtkWidget *book_entry;
    const gchar *book_text;
    int i, is_fine = 1;

    for (i = 0; i < num_rows+1; i++){
        book_entry = gtk_grid_get_child_at(GTK_GRID(grid), 3, i);
        book_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(book_entry)));

        if (strlen(book_text) > 36){
            is_fine = 0;
        }
    }

    if (!is_fine) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Error: Text too long. Please limit to 36 characters.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        save_materials(button, user_data); // Call the save_materials function if the entry is valid
    }
}

void test_switch(GtkWidget *widget, gpointer data) {
    GtkWidget *questions_label = GTK_WIDGET(data);

    if (is_questions) {
        gtk_label_set_text(GTK_LABEL(questions_label), "Test Book Pages:");
        is_questions = FALSE;
    } else {
        gtk_label_set_text(GTK_LABEL(questions_label), "Test Questions:");
        is_questions = TRUE;
    }
}

void show_books_of_type(GtkGrid *grid, Book **books_array, int type, int *row, float hours) {
    if (grid == NULL || books_array == NULL) {
        printf("grid or books_array is null\n");
        return;
    }

    int i = 0;
    int hour, min, total_pages;
    float single_time;
    while (books_array[i] != NULL && i < num_books) {
        if (books_array[i]->type == type) {
            if (books_array[i]->name == NULL || books_array[i]->pages == 0) {
                printf("books_array[i]->name is null or books_array[i]->pages is 0\n");
            }
            char label_text[80];
            switch (type)
            {
            case 0:
                total_pages = total_reading_pages;
                break;
            case 1:
                total_pages = total_test_pages;
                break;
            case 2:
                total_pages = total_sum_pages;
                break;
            case 3:
                total_pages = total_writing_pages;
                break;
            default:
                total_pages = 0;
                break;
            }
            single_time = hours * 60 * ((float)(books_array[i]->pages) / total_pages);
            hour = single_time / 60;
            min = single_time - hour * 60;
            sprintf(label_text, "Book: %s,  Pages: %d,  %d Hours %d Mins", books_array[i]->name, books_array[i]->pages, hour, min);
            GtkWidget *label = gtk_label_new(label_text);
            if (label == NULL) {
                printf("Failed to create label\n");
            }

            gtk_grid_attach(GTK_GRID(grid), label, 0, *row, 1, 1);
            (*row)++;
        }
        i++;
    }
}

void show_videos(GtkGrid *grid, Video **videos_array, int *row, float hours) {
    if (grid == NULL || videos_array == NULL) {
        return;
    }

    int i = 0;
    int single_time;
    while (videos_array[i] != NULL && i < num_videos) {
        if (videos_array[i]->name[0] != '\0' && videos_array[i]->duration > 0) {
            char label_text[80];
            single_time = hours * 60 * (videos_array[i]->duration / total_duration);
            sprintf(label_text, "Video: %s, Duration: %.2f, %d Mins per day", videos_array[i]->name, videos_array[i]->duration, single_time);
            GtkWidget *label = gtk_label_new(label_text);
            if (label == NULL) {
                g_error("Failed to create label");
            }

            gtk_grid_attach(GTK_GRID(grid), label, 0, *row, 1, 1);
            (*row)++;
        }
        i++;
    }
}

