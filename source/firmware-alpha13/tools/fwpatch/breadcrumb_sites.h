struct ExtraSite {
  unsigned va;
  const char *before;
  const char *symbol;
  bool link;
};
static const ExtraSite breadcrumb_sites[] = {
    {0x126a, "34483449", "breadcrumb_boot", false},
    {0x57ec, "10b51249", "breadcrumb_disable_entry", false},
    {0x5828, "02f080fe", "breadcrumb_disable_exit", true},
    {0x5830, "40f20110", "breadcrumb_disable_invalid", false},
    {0x8054, "174d08b5", "breadcrumb_systemoff_entry", false},
    {0x80a6, "c3f80025", "breadcrumb_systemoff_write", false},
    {0x83ea, "86f82e30", "breadcrumb_battery", false},
    {0x83fc, "28b10023", "breadcrumb_battery_exit", false},
    {0x951e, "9af80100", "breadcrumb_command", false},
    {0xc376, "84f82e30", "breadcrumb_button", false},
    {0xc388, "e0b10023", "breadcrumb_button_exit", false},
    {0x8406, "01f045fb", "breadcrumb_battery_event2", true},
    {0xc398, "fdf77cbb", "breadcrumb_button_event2", false},
    {0x9a94, "10b40b4a", "breadcrumb_event_post", false},
};
