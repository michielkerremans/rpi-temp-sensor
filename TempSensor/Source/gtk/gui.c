#include <gtk/gtk.h>
#include "gui.h"
#include "../gpio/gpio.h"
#include "../mqtt/mqtt.h"

// --- Function prototypes ---
static gboolean toggle_gpio17_cb(gpointer user_data);
static gboolean toggle_gpio19_cb(gpointer user_data);
static void on_toggle17(GtkToggleButton *button, gpointer user_data);
static void on_toggle19(GtkToggleButton *button, gpointer user_data);
static void on_interval17_changed(GtkSpinButton *spin, gpointer button);
static void on_interval19_changed(GtkSpinButton *spin, gpointer button);
static gboolean update_temp_label(gpointer user_data);
void launch_gtk_gui(void);

// --- Static state ---
static int gpio17_state = 0, gpio19_state = 0;
static guint timer17_id = 0, timer19_id = 0;
static int interval17 = 0, interval19 = 0;
static GtkWidget *temp_label = NULL;

// --- Function definitions ---
static gboolean toggle_gpio17_cb(gpointer user_data)
{
  gpio17_state = !gpio17_state;
  GPIO_Write(17, gpio17_state);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(user_data), gpio17_state);
  return TRUE;
}
static gboolean toggle_gpio19_cb(gpointer user_data)
{
  gpio19_state = !gpio19_state;
  GPIO_Write(19, gpio19_state);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(user_data), gpio19_state);
  return TRUE;
}
static void on_toggle17(GtkToggleButton *button, gpointer user_data)
{
  gpio17_state = gtk_toggle_button_get_active(button);
  GPIO_Write(17, gpio17_state);
}
static void on_toggle19(GtkToggleButton *button, gpointer user_data)
{
  gpio19_state = gtk_toggle_button_get_active(button);
  GPIO_Write(19, gpio19_state);
}
static void on_interval17_changed(GtkSpinButton *spin, gpointer button)
{
  int val = gtk_spin_button_get_value_as_int(spin);
  if (timer17_id)
    g_source_remove(timer17_id);
  interval17 = val;
  if (interval17 > 0)
    timer17_id = g_timeout_add_seconds(interval17, toggle_gpio17_cb, button);
}
static void on_interval19_changed(GtkSpinButton *spin, gpointer button)
{
  int val = gtk_spin_button_get_value_as_int(spin);
  if (timer19_id)
    g_source_remove(timer19_id);
  interval19 = val;
  if (interval19 > 0)
    timer19_id = g_timeout_add_seconds(interval19, toggle_gpio19_cb, button);
}
static gboolean update_temp_label(gpointer user_data)
{
  const char *payload = mqtt_get_last_payload();
  char buf[64];
  snprintf(buf, sizeof(buf), "Temperature: %s", payload[0] ? payload : "--");
  gtk_label_set_text(GTK_LABEL(temp_label), buf);
  return TRUE; // keep timer running
}

void launch_gtk_gui(void)
{
  int argc = 0;
  char **argv = NULL;
  gtk_init(&argc, &argv);
  GPIO_Init();
  GPIO_Mode(17, 1);
  GPIO_Mode(19, 1);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "GPIO Toggle GUI");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

  // GPIO 17
  GtkWidget *toggle17 = gtk_toggle_button_new_with_label("GPIO 17");
  GtkWidget *spin17 = gtk_spin_button_new_with_range(0, 60, 1);
  gtk_grid_attach(GTK_GRID(grid), toggle17, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), spin17, 1, 0, 1, 1);

  // GPIO 19
  GtkWidget *toggle19 = gtk_toggle_button_new_with_label("GPIO 19");
  GtkWidget *spin19 = gtk_spin_button_new_with_range(0, 60, 1);
  gtk_grid_attach(GTK_GRID(grid), toggle19, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), spin19, 1, 1, 1, 1);

  // Temperature label
  temp_label = gtk_label_new("Temperature: --");
  gtk_grid_attach(GTK_GRID(grid), temp_label, 0, 2, 2, 1);

  gtk_container_add(GTK_CONTAINER(window), grid);

  // Connect signals
  g_signal_connect(toggle17, "toggled", G_CALLBACK(on_toggle17), NULL);
  g_signal_connect(toggle19, "toggled", G_CALLBACK(on_toggle19), NULL);
  g_signal_connect(spin17, "value-changed", G_CALLBACK(on_interval17_changed), toggle17);
  g_signal_connect(spin19, "value-changed", G_CALLBACK(on_interval19_changed), toggle19);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);

  // Start timer to update temperature label every second
  g_timeout_add_seconds(1, update_temp_label, NULL);

  gtk_main();
}