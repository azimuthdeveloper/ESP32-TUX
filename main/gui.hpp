
#include "widgets/tux_panel.h"
LV_IMG_DECLARE(dev_bg)
LV_IMG_DECLARE(weather_clear_day)
LV_IMG_DECLARE(weather_clear_night)
LV_IMG_DECLARE(weather_cloudy_snowing)

// LV_FONT_DECLARE(font_7seg_16)
// LV_FONT_DECLARE(font_7seg_24)
// LV_FONT_DECLARE(font_7seg_28)
// LV_FONT_DECLARE(font_7seg_32)

// LV_FONT_DECLARE(font_7seg_48)
// LV_FONT_DECLARE(font_7seg_56)
LV_FONT_DECLARE(font_7seg_64)

LV_FONT_DECLARE(font_fa_14)
#define FA_SYMBOL_BLE "\xEF\x8A\x94"      // 0xf294
#define FA_SYMBOL_SETTINGS "\xEF\x80\x93" // 0xf0ad

/*********************
 *      DEFINES
 *********************/
#define HEADER_HEIGHT 30
#define FOOTER_HEIGHT 30

/******************
 *  LV DEFINES
 ******************/
static const lv_font_t *font_large;
static const lv_font_t *font_normal;
static const lv_font_t *font_symbol;
static const lv_font_t *font_fa;

static lv_obj_t *panel_header;
static lv_obj_t *panel_title;
static lv_obj_t *panel_status; // Status icons in the header
static lv_obj_t *content_container;
static lv_obj_t *screen_container;

static lv_obj_t *island_wifi;
static lv_obj_t *island_ota;
static lv_obj_t *island_devinfo;

static lv_obj_t *label_title;
static lv_obj_t *label_message;

static lv_obj_t *icon_storage;
static lv_obj_t *icon_wifi;
static lv_obj_t *icon_ble;
static lv_obj_t *icon_battery;

static lv_coord_t screen_h;
static lv_coord_t screen_w;

/******************
 *  LVL STYLES
 ******************/
static lv_style_t style_content_bg;

static lv_style_t style_message;
static lv_style_t style_title;
static lv_style_t style_iconstatus;
static lv_style_t style_storage;
static lv_style_t style_wifi;
static lv_style_t style_ble;
static lv_style_t style_battery;

static lv_style_t style_ui_island;

/******************
 *  LVL ANIMATION
 ******************/
static lv_anim_t anim_labelscroll;

/******************
 *  LVL FUNCS & EVENTS
 ******************/
static void create_page_home(lv_obj_t *parent);
static void create_page_settings(lv_obj_t *parent);

// Home page islands

// Setting page islands
static void panel_wifi_island(lv_obj_t *parent);
static void panel_ota_island(lv_obj_t *parent);
static void panel_devinfo_island(lv_obj_t *parent);

static void create_header(lv_obj_t *parent);
static void create_footer(lv_obj_t *parent);

static void footer_message(const char *fmt, ...);
static void switch_theme(bool dark);
static void qrcode_ui(lv_obj_t *parent);

static void rotate_event_handler(lv_event_t *e);
static void theme_switch_event_handler(lv_event_t *e);
// static void espwifi_event_handler(lv_event_t* e);
static void espble_event_handler(lv_event_t *e);
static void checkupdates_event_handler(lv_event_t *e);
static void home_clicked_eventhandler(lv_event_t *e);
static void status_clicked_eventhandler(lv_event_t *e);

// static void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj);

void lv_setup_styles()
{
    font_symbol = &lv_font_montserrat_14;
    font_normal = &lv_font_montserrat_14;
    font_large = &lv_font_montserrat_16;
    font_fa = &font_fa_14;

    screen_h = lv_obj_get_height(lv_scr_act());
    screen_w = lv_obj_get_width(lv_scr_act());

    /* CONTENT CONTAINER BACKGROUND */
    lv_style_init(&style_content_bg);
    lv_style_set_bg_opa(&style_content_bg, LV_OPA_50);
    lv_style_set_radius(&style_content_bg, 0);

    // Image Background
    // CF_INDEXED_8_BIT for smaller size - resolution 480x480
    lv_style_set_bg_img_src(&style_content_bg, &dev_bg);
    // lv_style_set_bg_img_opa(&style_content_bg,LV_OPA_50);

    // Gradient Background
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_palette_main(LV_PALETTE_GREY);
    grad.stops[1].color = theme_current->color_primary;
    grad.stops[0].frac = 100;
    grad.stops[1].frac = 192;
    // lv_style_set_bg_grad(&style_content_bg, &grad);

    // DASHBOARD TITLE
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    lv_style_set_align(&style_title, LV_ALIGN_LEFT_MID);
    lv_style_set_pad_left(&style_title, 15);
    lv_style_set_border_width(&style_title, 0);
    lv_style_set_size(&style_title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    // HEADER STATUS ICON PANEL
    lv_style_init(&style_iconstatus);
    lv_style_set_size(&style_iconstatus, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_style_set_pad_all(&style_iconstatus, 0);
    lv_style_set_border_width(&style_iconstatus, 0);
    lv_style_set_align(&style_iconstatus, LV_ALIGN_RIGHT_MID);
    lv_style_set_pad_right(&style_iconstatus, 15);

    lv_style_set_layout(&style_iconstatus, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(&style_iconstatus, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(&style_iconstatus, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(&style_iconstatus, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_pad_row(&style_iconstatus, 3);

    // BATTERY
    lv_style_init(&style_battery);
    lv_style_set_text_font(&style_battery, font_symbol);
    lv_style_set_align(&style_battery, LV_ALIGN_RIGHT_MID);

    // SD CARD
    lv_style_init(&style_storage);
    lv_style_set_text_font(&style_storage, font_symbol);
    lv_style_set_align(&style_storage, LV_ALIGN_RIGHT_MID);

    // WIFI
    lv_style_init(&style_wifi);
    lv_style_set_text_font(&style_wifi, font_symbol);
    lv_style_set_align(&style_wifi, LV_ALIGN_RIGHT_MID);

    // BLE
    lv_style_init(&style_ble);
    lv_style_set_text_font(&style_ble, font_fa);
    lv_style_set_align(&style_ble, LV_ALIGN_RIGHT_MID);

    // FOOTER MESSAGE & ANIMATION
    lv_anim_init(&anim_labelscroll);
    lv_anim_set_delay(&anim_labelscroll, 1000);        // Wait 1 second to start the first scroll
    lv_anim_set_repeat_delay(&anim_labelscroll, 3000); // Repeat the scroll 3 seconds after the label scrolls back to the initial position

    lv_style_init(&style_message);
    lv_style_set_anim(&style_message, &anim_labelscroll); // Set animation for the style
    // lv_style_set_text_color(&style_message, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_opa(&style_message, LV_OPA_COVER);
    lv_style_set_text_font(&style_message, font_normal);
    lv_style_set_align(&style_message, LV_ALIGN_LEFT_MID);
    lv_style_set_pad_left(&style_message, 15);
    lv_style_set_pad_right(&style_message, 15);

    // UI ISLANDS
    lv_style_init(&style_ui_island);
    lv_style_set_bg_color(&style_ui_island, bg_theme_color);
    lv_style_set_bg_opa(&style_ui_island, LV_OPA_80);
    lv_style_set_border_color(&style_ui_island, bg_theme_color);
    lv_style_set_border_opa(&style_ui_island, LV_OPA_80);
    lv_style_set_border_width(&style_ui_island, 1);
    lv_style_set_radius(&style_ui_island, 5);
}

static void create_header(lv_obj_t *parent)
{
    // HEADER PANEL
    panel_header = lv_obj_create(parent);
    lv_obj_set_size(panel_header, LV_PCT(100), HEADER_HEIGHT);
    lv_obj_set_style_pad_all(panel_header, 0, 0);
    lv_obj_set_style_radius(panel_header, 0, 0);
    lv_obj_set_align(panel_header, LV_ALIGN_TOP_MID);
    lv_obj_set_scrollbar_mode(panel_header, LV_SCROLLBAR_MODE_OFF);

    // HEADER TITLE PANEL
    panel_title = lv_obj_create(panel_header);
    lv_obj_add_style(panel_title, &style_title, 0);
    lv_obj_set_scrollbar_mode(panel_title, LV_SCROLLBAR_MODE_OFF);

    // HEADER TITLE
    label_title = lv_label_create(panel_title);
    lv_label_set_text(label_title, LV_SYMBOL_HOME " DASHBOARD");

    // HEADER STATUS ICON PANEL
    panel_status = lv_obj_create(panel_header);
    lv_obj_add_style(panel_status, &style_iconstatus, 0);
    lv_obj_set_scrollbar_mode(panel_status, LV_SCROLLBAR_MODE_OFF);

    // BLE
    icon_ble = lv_label_create(panel_status);
    lv_label_set_text(icon_ble, FA_SYMBOL_BLE);
    lv_obj_add_style(icon_ble, &style_ble, 0);

    // WIFI
    icon_wifi = lv_label_create(panel_status);
    lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    lv_obj_add_style(icon_wifi, &style_wifi, 0);

    // SD CARD
    icon_storage = lv_label_create(panel_status);
    lv_label_set_text(icon_storage, LV_SYMBOL_SD_CARD);
    lv_obj_add_style(icon_storage, &style_storage, 0);

    // BATTERY
    icon_battery = lv_label_create(panel_status);
    lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_add_style(icon_battery, &style_battery, 0);

    lv_obj_add_event_cb(panel_title, home_clicked_eventhandler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(panel_status, status_clicked_eventhandler, LV_EVENT_CLICKED, NULL);
}

static void create_footer(lv_obj_t *parent)
{
    lv_obj_t *panel_footer = lv_obj_create(parent);
    lv_obj_set_size(panel_footer, LV_PCT(100), FOOTER_HEIGHT);
    // lv_obj_set_style_bg_color(panel_footer, bg_theme_color, 0);
    lv_obj_set_style_pad_all(panel_footer, 0, 0);
    lv_obj_set_style_radius(panel_footer, 0, 0);
    lv_obj_set_align(panel_footer, LV_ALIGN_BOTTOM_MID);

    // Create Footer label and animate if longer
    label_message = lv_label_create(panel_footer); // full screen as the parent
    lv_obj_set_width(label_message, LV_PCT(100));
    lv_label_set_long_mode(label_message, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_add_style(label_message, &style_message, LV_STATE_DEFAULT);

    // Show LVGL version
    footer_message("LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
}

static void create_page_home(lv_obj_t *parent)
{
    /* HOME PAGE PANELS */
    // ******** UI ISLAND
    lv_obj_t *island_0 = tux_panel_create(parent, LV_SYMBOL_BELL " CLOCK", 120);
    lv_obj_add_style(island_0, &style_ui_island, 0);

    lv_obj_t *cont_0 = tux_panel_get_content(island_0);
    // footer_message("Children count: %d",lv_obj_get_child_cnt(island_0));

    lv_obj_t *l = lv_label_create(cont_0);
    lv_obj_align(l, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(l, &font_7seg_64, 0);
    // Lime green = lv_color_make(50,205,50)
    // lv_obj_set_style_shadow_width(l,50,0);
    // lv_obj_set_style_shadow_color(l,lv_palette_main(LV_PALETTE_BLUE),0);
    lv_obj_set_style_text_color(l, lv_color_make(50, 205, 50), 0);
    lv_label_set_text(l, "20:25");

    // ******** UI ISLAND
    lv_obj_t *island_1 = tux_panel_create(parent, LV_SYMBOL_TINT " WEATHER", 120);
    lv_obj_add_style(island_1, &style_ui_island, 0);
    // tux_panel_set_title_bg_color(island_1,lv_palette_main(LV_PALETTE_RED));

    lv_obj_t *cont_1 = tux_panel_get_content(island_1);

    lv_obj_set_layout(cont_1, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont_1, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // static lv_style_t imgstyle;
    // lv_style_init(&imgstyle);

    lv_obj_t *img1 = lv_img_create(cont_1);
    lv_img_set_src(img1, &weather_clear_day);

    lv_obj_t *img2 = lv_img_create(cont_1);
    lv_img_set_src(img2, &weather_clear_night);

    lv_obj_t *img3 = lv_img_create(cont_1);
    lv_img_set_src(img3, &weather_cloudy_snowing);

    lv_obj_set_style_img_opa(img1, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(img1, lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img1, LV_OPA_COVER, 0);

    lv_obj_set_style_img_opa(img2, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(img2, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_obj_set_style_img_recolor_opa(img2, LV_OPA_COVER, 0);

    lv_obj_set_style_img_opa(img3, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(img3, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_img_recolor_opa(img3, LV_OPA_COVER, 0);

    // ******** UI ISLAND
    lv_obj_t *island_2 = tux_panel_create(parent, LV_SYMBOL_EDIT " CONFIGURE", 135);
    lv_obj_add_style(island_2, &style_ui_island, 0);

    // Get Content Area to add UI elements
    lv_obj_t *cont_2 = tux_panel_get_content(island_2);

    lv_obj_set_flex_flow(cont_2, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(cont_2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(cont_2);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Theme : Dark");

    lv_obj_t *sw = lv_switch_create(cont_2);
    lv_obj_add_event_cb(sw, theme_switch_event_handler, LV_EVENT_ALL, label);

    // Rotate
    lv_obj_t *btn2 = lv_btn_create(cont_2);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn2, rotate_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_t *lbl2 = lv_label_create(btn2);
    lv_label_set_text(lbl2, "Rotate to Landscape");
    lv_obj_center(lbl2);
}

// Provision WIFI
static void panel_wifi_island(lv_obj_t *parent)
{
    /******** PROVISION WIFI ********/
    island_wifi = tux_panel_create(parent, LV_SYMBOL_WIFI " WIFI PROVISIONING", 100);
    lv_obj_add_style(island_wifi, &style_ui_island, 0);
    // tux_panel_set_title_color(island_wifi, lv_palette_main(LV_PALETTE_BLUE));

    // Get Content Area to add UI elements
    lv_obj_t *cont_1 = tux_panel_get_content(island_wifi);
    lv_obj_set_flex_flow(cont_1, LV_FLEX_FLOW_COLUMN);

    ///* ESP-WIFI - Wifi SoftAP Provisioning */
    // lv_obj_t* esp_wifi = lv_obj_create(cont_1);
    // lv_obj_set_size(esp_wifi, LV_PCT(100), LV_SIZE_CONTENT);
    // lv_obj_set_style_bg_opa(esp_wifi, LV_OPA_TRANSP, 0);
    // lv_obj_set_style_border_width(esp_wifi, 0, 0);

    // lv_obj_t* label_1 = lv_label_create(esp_wifi);
    // lv_obj_align(label_1, LV_ALIGN_LEFT_MID, 0, 0);
    // lv_label_set_text(label_1, "Using WiFi SoftAP");

    // lv_obj_t* sw_1 = lv_switch_create(esp_wifi);
    // lv_obj_align(sw_1, LV_ALIGN_RIGHT_MID, 0, 0);

    // lv_obj_add_event_cb(sw_1, espwifi_event_handler, LV_EVENT_ALL, lbl_status);

    /* ESP-BLE */
    lv_obj_t *esp_ble = lv_obj_create(cont_1);
    lv_obj_set_size(esp_ble, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(esp_ble, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(esp_ble, 3, 0);
    lv_obj_set_style_border_width(esp_ble, 0, 0);

    lv_obj_t *label_2 = lv_label_create(esp_ble);
    lv_obj_align(label_2, LV_ALIGN_LEFT_MID, 0, 0);
    lv_label_set_text(label_2, "Provision using Bluetooth");

    lv_obj_t *sw_2 = lv_switch_create(esp_ble);
    lv_obj_align(sw_2, LV_ALIGN_RIGHT_MID, 0, 0);

    /* ESP TOUCH/ble STATUS*/
    lv_obj_t *esp_status = lv_obj_create(cont_1);
    lv_obj_set_size(esp_status, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(esp_status, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(esp_status, 0, 0);
    lv_obj_set_flex_flow(esp_status, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(esp_status, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_add_event_cb(sw_2, espble_event_handler, LV_EVENT_ALL, esp_status);
}

static void panel_ota_island(lv_obj_t *parent)
{
    /******** OTA UPDATES ********/
    island_ota = tux_panel_create(parent, LV_SYMBOL_DOWNLOAD " OTA UPDATES", 180);
    lv_obj_add_style(island_ota, &style_ui_island, 0);

    // Get Content Area to add UI elements
    lv_obj_t *cont_ota = tux_panel_get_content(island_ota);
    lv_obj_set_flex_flow(cont_ota, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_ota, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Current Firmware version
    lv_obj_t *lbl_version = lv_label_create(cont_ota);
    lv_obj_set_size(lbl_version, LV_SIZE_CONTENT, 30);
    lv_obj_align(lbl_version, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(lbl_version, "Firmware Version 1.1.0");

    // Check for Updates button
    lv_obj_t *btn_checkupdates = lv_btn_create(cont_ota);
    lv_obj_set_size(btn_checkupdates, LV_SIZE_CONTENT, 40);
    lv_obj_align(btn_checkupdates, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn_checkupdates, checkupdates_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_t *lbl2 = lv_label_create(btn_checkupdates);
    lv_label_set_text(lbl2, "Check for Updates");
    lv_obj_center(lbl2);

    lv_obj_t *esp_updatestatus = lv_obj_create(cont_ota);
    lv_obj_set_size(esp_updatestatus, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(esp_updatestatus, LV_OPA_10, 0);
    lv_obj_set_style_border_width(esp_updatestatus, 0, 0);

    lv_obj_t *lbl_update_status = lv_label_create(esp_updatestatus);
    lv_obj_set_style_text_color(lbl_update_status, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_obj_align(lbl_update_status, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(lbl_update_status, "New version available - v 1.2.0");
}

static void panel_devinfo_island(lv_obj_t *parent)
{
    island_devinfo = tux_panel_create(parent, LV_SYMBOL_TINT " DEVICE INFO", 100);
    lv_obj_add_style(island_devinfo, &style_ui_island, 0);
}

static void create_page_settings(lv_obj_t *parent)
{
    /* SETTINGS PAGE PANELS */
    panel_devinfo_island(parent);
    panel_wifi_island(parent);
    panel_ota_island(parent);
}

// Show QR Code for BLE based Wifi Provisioning
static void qrcode_ui(lv_obj_t *parent)
{
    /* QR CODE */
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_GREY, 4);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t *qr = lv_qrcode_create(parent, 100, fg_color, bg_color);

    /* Set data - format of BLE provisioning data */
    // {"ver":"v1","name":"TUX_4F4440","pop":"abcd1234","transport":"ble"}
    const char *data = "https://www.sukesh.me";
    lv_qrcode_update(qr, data, strlen(data));

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);

    lv_obj_t *lbl_status = lv_label_create(parent);
    lv_label_set_text(lbl_status, "Install 'ESP BLE Prov' App & Scan");
}

static void draw_ui()
{
    // screen_container is the root of the UX
    screen_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(screen_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(screen_container, 0, 0);
    lv_obj_align(screen_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(screen_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(screen_container, 0, 0);
    lv_obj_set_scrollbar_mode(screen_container, LV_SCROLLBAR_MODE_OFF);

    /* HEADER & FOOTER*/
    create_header(screen_container);
    create_footer(screen_container);

    /* CONTENT CONTAINER */
    content_container = lv_obj_create(screen_container);
    lv_obj_set_size(content_container, screen_w, screen_h - HEADER_HEIGHT - FOOTER_HEIGHT);
    lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, HEADER_HEIGHT);
    lv_obj_set_style_border_width(content_container, 0, 0);

    // Gradient / Image Background for content container
    lv_obj_add_style(content_container, &style_content_bg, 0);
    lv_obj_set_flex_flow(content_container, LV_FLEX_FLOW_COLUMN);

    // Show Home Page
    create_page_home(content_container);
}

static void rotate_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);

    if (code == LV_EVENT_CLICKED)
    {
        lvgl_acquire();

        if (lv_disp_get_rotation(disp) == LV_DISP_ROT_270)
            lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);
        else
            lv_disp_set_rotation(disp, (lv_disp_rot_t)(lv_disp_get_rotation(disp) + 1));

        if (LV_HOR_RES > LV_VER_RES)
            lv_label_set_text(label, "Rotate to Portrait");
        else
            lv_label_set_text(label, "Rotate to Landscape");

        lvgl_release();

        // Update
        screen_h = lv_obj_get_height(lv_scr_act());
        screen_w = lv_obj_get_width(lv_scr_act());
        lv_obj_set_size(content_container, screen_w, screen_h - HEADER_HEIGHT - FOOTER_HEIGHT);

        // footer_message("%d,%d",screen_h,screen_w);
    }
}

static void theme_switch_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *udata = (lv_obj_t *)lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        if (lv_obj_has_state(obj, LV_STATE_CHECKED))
        {
            switch_theme(false);
            lv_label_set_text(udata, "Theme : Light");
        }
        else
        {
            switch_theme(true);
            lv_label_set_text(udata, "Theme : Dark");
        }
    }
}

static void footer_message(const char *fmt, ...)
{
    char buffer[200];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    lv_label_set_text(label_message, buffer);
    va_end(args);
}

static void home_clicked_eventhandler(lv_event_t *e)
{
    // footer_message("Home clicked!");
    //  Clean the content container first
    lv_obj_clean(content_container);
    create_page_home(content_container);
}

static void status_clicked_eventhandler(lv_event_t *e)
{
    // footer_message("Status icons touched but this is a very long message to show scroll animation!");
    //  Clean the content container first
    lv_obj_clean(content_container);
    create_page_settings(content_container);
}

void switch_theme(bool dark)
{
    if (dark)
    {
        theme_current = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                              lv_palette_main(LV_PALETTE_GREEN),
                                              1, /*Light or dark mode*/
                                              &lv_font_montserrat_14);
        bg_theme_color = lv_palette_darken(LV_PALETTE_GREY, 5);
        lv_disp_set_theme(disp, theme_current);
        //bg_theme_color = theme_current->flags & LV_USE_THEME_DEFAULT ? lv_palette_darken(LV_PALETTE_GREY, 5) : lv_palette_lighten(LV_PALETTE_GREY, 2);
        // lv_theme_set_apply_cb(theme_current, new_theme_apply_cb);

        lv_style_set_bg_color(&style_ui_island, bg_theme_color);
        //lv_style_set_bg_opa(&style_ui_island, LV_OPA_80);

        ESP_LOGI(TAG,"Dark theme set");
    }
    else
    {
        theme_current = lv_theme_default_init(disp,
                                              lv_palette_main(LV_PALETTE_BLUE),
                                              lv_palette_main(LV_PALETTE_RED),
                                              0, /*Light or dark mode*/
                                              &lv_font_montserrat_14);
        //bg_theme_color = lv_palette_lighten(LV_PALETTE_GREY, 5);    // #BFBFBD
        // bg_theme_color = lv_color_make(0,0,255); 
        bg_theme_color = lv_color_hex(0xBFBFBD); //383837


        lv_disp_set_theme(disp, theme_current);
        // lv_theme_set_apply_cb(theme_current, new_theme_apply_cb);
        lv_style_set_bg_color(&style_ui_island, bg_theme_color);
        ESP_LOGI(TAG,"Light theme set");        

    }
}

// /*Will be called when the styles of the base theme are already added
//   to add new styles*/
// static void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj)
// {
//     LV_UNUSED(th);

//     if(lv_obj_check_type(obj, &tux_panel_class)) {
//         lv_obj_add_style(obj, &style_ui_island, 0);
//         //lv_style_set_bg_color(&style_ui_island,theme_current->color_primary);
//     }

// }

// static void espwifi_event_handler(lv_event_t* e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t* obj = lv_event_get_target(e);
//     lv_obj_t* lbl = (lv_obj_t*)lv_event_get_user_data(e);

//     if (code == LV_EVENT_VALUE_CHANGED) {
//         LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
//         if (lv_obj_has_state(obj, LV_STATE_CHECKED))
//         {
//             lv_label_set_text_fmt(lbl, "WiFi Provisioning...ON");
//         }
//         else
//         {
//             lv_label_set_text_fmt(lbl, "WiFi Provisioning...OFF");
//         }
//     }
// }

static void espble_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *esp_status_panel = (lv_obj_t *)lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        if (lv_obj_has_state(obj, LV_STATE_CHECKED))
        {
            tux_panel_set_height(island_wifi, 250);
            qrcode_ui(esp_status_panel);
        }
        else
        {
            tux_panel_set_height(island_wifi, 100);
            lv_obj_clean(esp_status_panel);
        }
    }
}

inline void checkupdates_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t *label = lv_obj_get_child(btn, 0);

        lv_label_set_text_fmt(label, "Checking for updates...");
        LV_LOG_USER("Clicked");
    }
}
