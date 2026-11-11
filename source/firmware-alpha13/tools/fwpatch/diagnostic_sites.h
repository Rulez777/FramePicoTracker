struct Site {
  unsigned va;
  const char *before;
  const char *after;
  const char *reason;
};
static const Site diagnostic_sites[] = {
    {0x6214, "44f6de14", "12f080bb",
     "Stock/Frame MOVW selector; LR and APSR preserved"},
    {0x6218, "41f27720", "12f092bb",
     "Stock/Frame MOVW selector; LR and APSR preserved"},
    {0x630a, "44f6de10", "12f02dbb",
     "Stock/Frame MOVW selector; LR and APSR preserved"},
    {0x630e, "41f27721", "12f03fbb",
     "Stock/Frame MOVW selector; LR and APSR preserved"},
    {0x6cde, "40f6a760", "11f06bbe",
     "Stock/Frame MOVW selector; LR and APSR preserved"},
    {0x6ce4, "40f61b50", "11f07cbe",
     "Stock/Frame MOVW selector; LR and APSR preserved"},
    {0x6cba, "d3e90023", "11f0a5be",
     "Stock/Frame double selector; LR and APSR preserved"},
    {0x6cc4, "d3e90023", "11f0b6be",
     "Stock/Frame double selector; LR and APSR preserved"},
    {0x6d2c, "d3e90023", "11f098be",
     "Stock/Frame double selector; LR and APSR preserved"},
    {0x6d36, "d3e90023", "11f0a9be",
     "Stock/Frame double selector; LR and APSR preserved"},
    {0x6f64, "d3e90023", "11f0a8bd",
     "Stock/Frame double selector; LR and APSR preserved"},
    {0xc3ec, "f7b5344e", "0cf07abb",
     "Clear profile at native startup-button routine entry"},
    {0xc444, "032a2b80", "0cf076bb",
     "Ordinary resetreason0: select window ends at150 with event1; other reset "
     "reasons preserve native hold"},
    {0xc4aa, "0120f4e7", "0cf057bb",
     "Select Frame only at startup release count100..149"},
    {0x8d16, "30b58bb0", "0ff065bf",
     "Read-only command1/subcommand7e schema1 nonce16; native fallback "
     "unchanged"},
    {0xbce6, "faf77bf9", "0cf04fff",
     "Clear profile after native HFCLK stop in full power-off path"},
    {0x1348, "50510020", "70510020",
     "Reserve32bytes observer/profile state; heap512->480, fixed end "
     "unchanged"},
    {0x134c, "50510020", "70510020",
     "Reserve32bytes observer/profile state; heap512->480, fixed end "
     "unchanged"},
    {0x6c98, "fef7a8fd", "12f0bcf9",
     "Apply pending profile after native receive-complete radio halt; leave "
     "TIMER0 stopped until native ADDRESS rearm"},
    {0x8d58, "fef7ccfd", "10f0cef9", "observe21"},
    {0x7b8e, "1c600e4b", "11f0d1ba", "observe_start"},
    {0x7be2, "1a707047", "11f0c9ba", "observe_stop"},
    {0x65c0, "1c70094b", "12f0f6bd", "observe_clear"},
    {0x785a, "1a704b78", "11f0cbbc", "observe_compare"},
};
