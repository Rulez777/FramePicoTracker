void seed(Cpu &c) {
  c.store(0x20000e10, 1, 1);
  c.store(0x200008c8, 4, 1);
  c.store(0x20000f21, 1, 1);
  c.mmio[0x40001550] = 0;
}
void command_tests(const Bytes &image, const Elf &, bool crumbs, unsigned revision=11) {
  for (unsigned schema = 1; schema <= 19; ++schema)
    for (unsigned pm : {0u, 1u}) {
      Cpu c(image);
      seed(c);
      c.primask = pm;
      c.r[0] = 0x20003000;
      c.r[1] = 18;
      for (unsigned i = 0; i < 32; ++i)
        c.store(c.r[0] + i, i * 7 + 3, 1);
      c.store(c.r[0], 0x7e, 1);
      c.store(c.r[0] + 1, schema, 1);
      for (unsigned i = 0; i < 4; ++i)
        c.store(0x20000efe + i, 0x90 + i, 1);
      for (unsigned i = 0; i < 20; ++i)
        c.store(0x20005310 + i, 0x80 + i, 1);
      auto original = c.r;
      unsigned flags = c.apsr;
      c.stops.insert(0x8d1a);
      bool supported =
          schema <= 2 || (crumbs && (schema >= 12 && schema <= 17));
      check(c.run(0x8d16), "query returned/fell back");
      check(c.primask == pm, "query PRIMASK");
      if (supported) {
        check(c.r[13] == original[13], "query balanced stack");
        check(c.reply.size() == (schema == 1   ? 24
                                 : schema == 2 ? 26
                                               : 32),
              "query fixed length");
        check(c.reply[0] == 0xfe && c.reply[1] == schema, "query header");
        for (unsigned i = 2; i < 18; ++i)
          check(c.reply[i] == uint8_t(i * 7 + 3), "query nonce");
        for (unsigned i = 0; i < 4; ++i)
          check(c.reply[18 + i] == 0x90 + i, "query chip");
        if (schema <= 2) {
          check(c.reply[22] == 0 && c.reply[23] == (schema == 1 ? 1 : revision), "query mode/revision");
          if (schema == 2)
            check(c.reply[24] == 1 && c.reply[25] == 0, "query N/pending");
        } else {
          check(c.reply[22] == 0, "live source advertised");
          for (unsigned i = 0; i < 9; ++i) {
            unsigned off = (schema - 12) * 9 + i;
            check(c.reply[23 + i] == (off < 20 ? 0x80 + off : 0),
                  "breadcrumb/crash page bytes");
          }
        }
        for (unsigned i = 4; i <= 8; ++i)
          check(c.r[i] == original[i], "query callee saved");
      } else {
        check(c.reply.empty() && c.r[15] == 0x8d1a, "removed page fallback");
        check(c.r[13] == original[13] - 56 && c.apsr == flags,
              "fallback displaced prologue/flags");
        for (unsigned i : {0u, 1u, 2u, 3u, 4u, 5u, 12u, 14u})
          check(c.r[i] == original[i], "fallback registers");
      }
    }
  // Setter: success, same, changed N, OTA busy, and each validation boundary.
  for (unsigned scenario = 0; scenario < 15; ++scenario)
    for (unsigned pm : {0u, 1u}) {
      Cpu c(image);
      seed(c);
      c.primask = pm;
      c.r[0] = 0x20003000;
      c.r[1] = 24;
      for (unsigned i = 0; i < 24; ++i)
        c.store(c.r[0] + i, i + 1, 1);
      c.store(c.r[0], 0x7d, 1);
      c.store(c.r[0] + 1, 2, 1);
      c.store(c.r[0] + 22, 1, 1);
      c.store(c.r[0] + 23, 3, 1);
      for (unsigned i = 0; i < 4; ++i) {
        c.store(0x20000efe + i, 0xa0 + i, 1);
        c.store(c.r[0] + 18 + i, 0xa0 + i, 1);
      }
      bool valid = true;
      unsigned status = 1;
      if (scenario == 1) {
        c.store(0x20005150, 1, 1);
        c.store(0x20000f21, 3, 1);
        status = 0;
      }
      if (scenario == 2) {
        c.store(0x20002240, 1, 1);
        status = 2;
      }
      if (scenario >= 3 && scenario <= 6) {
        c.store(c.r[0] + 18 + scenario - 3, 0, 1);
        valid = false;
      }
      if (scenario == 7) {
        c.store(c.r[0] + 22, 2, 1);
        valid = false;
      }
      if (scenario == 8) {
        c.store(c.r[0] + 23, 41, 1);
        valid = false;
      }
      if (scenario == 9) {
        c.r[1] = 23;
        valid = false;
      }
      if (scenario == 10) {
        c.store(c.r[0] + 1, 1, 1);
        valid = false;
      }
      if (scenario == 11) {
        c.store(c.r[0], 0x7c, 1);
        valid = false;
      }
      if (scenario == 12)
        c.store(c.r[0] + 23, 40, 1);
      if (scenario == 13)
        c.store(c.r[0] + 23, 0, 1);
      if (scenario == 14) {
        c.store(c.r[0] + 22, 0, 1);
        c.store(c.r[0] + 23, 1, 1);
        status = 0;
      }
      auto original = c.r;
      c.stops.insert(0x8d1a);
      check(c.run(0x8d16), "setter return");
      check(c.primask == pm, "setter PRIMASK");
      if (valid) {
        check(c.reply.size() == 26 && c.reply[0] == 0xfd && c.reply[25] == revision &&
                  c.reply[24] == status,
              "setter status/revision");
        check(c.load(0x20005150, 1) == (scenario == 1 ? 1u : 0u),
              "setter never changes live mode");
        check(c.load(0x20005151, 1) == (status == 1 ? 2u : 0u),
              "setter pending");
        if (status == 1)
          check(c.load(0x20005152, 1) == c.load(original[0] + 23, 1),
                "setter staged N");
        check(c.r[13] == original[13], "setter SP");
      } else
        check(c.reply.empty() && c.r[15] == 0x8d1a, "setter invalid fallback");
    }
  // Wrong query length and opcode always take stock fallback.
  for (unsigned len : {0u, 1u, 17u, 19u, 25u, 255u}) {
    Cpu c(image);
    c.r[0] = 0x20003000;
    c.r[1] = len;
    c.store(c.r[0], 0x7e, 1);
    c.store(c.r[0] + 1, 2, 1);
    c.stops.insert(0x8d1a);
    check(c.run(0x8d16) && c.reply.empty(), "wrong query length");
  }
}
void boundary_tests(const Bytes &image, const Elf &, bool crumbs) {
  for (unsigned scenario = 0; scenario < 10; ++scenario)
    for (unsigned pm : {0u, 1u}) {
      Cpu c(image);
      seed(c);
      c.primask = pm;
      c.store(0x20005151, 2, 1);
      c.store(0x20005152, 3, 1);
      if (scenario == 1)
        c.store(0x20005151, 0, 1);
      if (scenario == 2)
        c.store(0x20002240, 1, 1);
      if (scenario == 3)
        c.store(0x20000e10, 0, 1);
      if (scenario == 4)
        c.mmio[0x40001550] = 1;
      if (scenario == 5)
        c.store(0x20000f6a, 1, 1);
      if (scenario == 6)
        c.store(0x20005151, 3, 1);
      if (scenario == 7)
        c.store(0x200008c8, 0, 1);
      if (scenario == 8) {
        c.disabled_arrives = false;
      }
      if (scenario == 9) {
        c.store(0x20005150, 1, 1);
        c.store(0x20005151, 1, 1);
      }
      auto original = c.r;
      c.stops.insert(0x6c9c);
      bool returned = c.run(0x6c98, 4000);
      if (scenario == 8) {
        check(!returned, "DISABLED-never remains blocked");
        check(c.r[15] >= 0x581c && c.r[15] <= 0x5822, "DISABLED wait exact PC");
        check(c.primask == 1, "blocked pending keeps PRIMASK masked");
        check(c.load(0x20005150, 1) == 0 && c.load(0x20005151, 1) == 2,
              "blocked transition uncommitted");
        if (crumbs)
          check(c.load(0x2000531a, 1) == 0, "mode zero blocked path has no breadcrumb");
        continue;
      }
      check(returned && c.r[13] == original[13], "boundary returned/SP");
      check(c.primask == pm, "boundary restores PRIMASK");
      for (unsigned i = 4; i <= 6; ++i)
        check(c.r[i] == original[i], "boundary callee saved");
      bool commit = scenario == 0 || scenario == 9;
      if (commit) {
        check(c.load(0x20005150, 1) == (scenario == 9 ? 0u : 1u) &&
                  c.load(0x20005151, 1) == 0,
              "boundary mode commit");
        check(c.load(0x20000f21, 1) == 3 && c.load(0x20000f24, 1) == 3 &&
                  c.load(0x20000f22, 1) == 0,
              "boundary N commit");
        check(c.mmio[0x40008010] == 1 && c.mmio[0x4000800c] == 1 &&
                  c.mmio[0x40008140] == 0 && c.mmio[0x40008144] == 0,
              "timer shutdown/clear");
        check(c.mmio[0xe000e280] == 0x100, "NVIC timer pending cleared");
        // No ADDRESS delivered: exact boundary ends without TASKS_START.
        check(c.mmio[0x40008000] == 0, "missing ADDRESS leaves timer stopped");
      } else {
        check(c.load(0x20005150, 1) == 0, "failed/deferred no commit");
        unsigned pending =
            (scenario == 1 || scenario == 2 || scenario == 3 || scenario == 6)
                ? 0
                : 2;
        check(c.load(0x20005151, 1) == pending, "boundary cancel/defer");
      }
      if (crumbs)
        check(c.load(0x2000531a, 1) == (scenario == 0 ? 11u : scenario == 9 ? 21u : 0u), "mode gated boundary exit breadcrumb");
    }
}
void breadcrumb_tests(const Bytes &image, const Elf &e) {
  for (unsigned pm : {0u, 1u})
    for (unsigned valid : {0u, 1u}) {
      Cpu c(image);
      c.primask = pm;
      c.store(0x20005310, valid ? 0x42435231 : 0xdeadbeef, 4);
      c.store(0x20005314, 0xfffffffe, 4);
      c.store(0x20005318, 0x01020304, 4);
      c.mmio[0x40000400] = 0x10008;
      auto original = c.r;
      unsigned flags = c.apsr;
      c.stops.insert(0x128e);
      check(c.run(0x126a, 200000), "boot/startup reaches runtime");
      check(c.load(0x20005310, 4) == 0x42435231 &&
                c.load(0x2000531c, 4) == 0x10008,
            "boot magic/full RESETREAS");
      check(c.load(0x20005314, 4) == (valid ? 0xfffffffeu : 0u),
            "boot preserves valid sequence");
      check(c.load(0x20005318, 4) == (valid ? 0x01020304u : 0u),
            "boot preserves last event/command/stage/PRIMASK");
      check(c.load(0x20005158, 4) == 0 && c.load(0x2000515c, 4) == 440,
            "startup heap head/reserved tail");
      check(c.primask == pm && c.r[13] == original[13], "boot stack/PRIMASK");
      c.stops.clear();
      c.store(0x20005150, 1, 1);
      c.r[0] = 12;
      c.r[1] = 7;
      c.r[2] = 2;
      c.r[3] = 17;
      c.r[14] = 0xdead0001;
      check(c.run(e.symbols.at("breadcrumb_write")), "breadcrumb write");
      check(c.primask == pm &&
                c.load(0x20005318, 4) ==
                    (7u | 17u << 8 | 12u << 16 | pm << 24) &&
                c.load(0x20005320, 4) == 2,
            "breadcrumb fields/PRIMASK");
      check(c.load(0x20005314, 4) == (valid ? 0xffffffffu : 1u),
            "breadcrumb sequence");
      c.r[14] = 0xdead0001;
      check(c.run(e.symbols.at("breadcrumb_write")), "breadcrumb second write");
      check(c.load(0x20005314, 4) == (valid ? 0u : 2u),
            "breadcrumb sequence wraps");
      auto record = Bytes(c.ram.begin() + 0x5310, c.ram.begin() + 0x5324);
      c.r[14] = 0xdead0001;
      check(c.run(e.symbols.at("profile_power_off")),
            "profile power-off cleanup");
      check(std::equal(record.begin(), record.end(), c.ram.begin() + 0x5310),
            "profile power-off leaves breadcrumb");
      (void)flags;
    }
  for (auto stage :
       {20u, 21u, 22u, 40u, 41u, 50u, 51u, 60u, 61u, 80u}) {
    Cpu c(image);
    seed(c);
    c.store(0x20005150, 1, 1);
    unsigned start = 0;
    switch (stage) {
    case 20:
      start = 0x57ec;
      c.disabled_arrives = false;
      break;
    case 21:
      start = 0x57ec;
      break;
    case 22:
      start = 0x57ec;
      c.store(0x200008c8, 0, 1);
      break;
    case 30:
    case 31:
      start = 0x532c;
      c.stops.insert(0x5330);
      if (stage == 30)
        c.stops.insert(0x62e4);
      break;
    case 40:
      start = 0x8054;
      c.stops.insert(0x8058);
      break;
    case 41:
      start = 0x80a6;
      c.r[2] = 1;
      c.r[3] = 0x40000000;
      c.stops.insert(0x80aa);
      break;
    case 50:
      start = 0x83ea;
      c.stops.insert(0x83ee);
      c.r[6] = 0x20000fd0;
      c.r[3] = 3;
      break;
    case 60:
      start = 0xc376;
      c.stops.insert(0xc37a);
      c.r[4] = 0x20000fd0;
      c.r[3] = 5;
      break;
    case 51:
      start = 0x83fc;
      c.stops.insert(0x8400);
      c.r[0] = 1;
      break;
    case 61:
      start = 0xc388;
      c.stops.insert(0xc38c);
      c.r[0] = 1;
      break;
    case 80:
      start = 0x951e;
      c.r[10] = 0x20003000;
      c.store(0x20003001, 17, 1);
      c.stops.insert(0x9522);
      break;
    }
    bool done = c.run(start, 4000);
    check(done || stage == 20, "instrumented stock hook completes/waits");
    check(c.load(0x2000531a, 1) == stage,
          "exact hook stage " + std::to_string(stage));
    if (stage == 41)
      check(c.mmio[0x40000500] == 1, "SYSTEMOFF write preserved");
    if (stage == 50 || stage == 60)
      check(c.load(0x20005320, 4) == (stage == 50 ? 1u : 2u),
            "power origin preserved");
    if (stage == 80)
      check(c.load(0x20005319, 1) == 17, "dispatcher command byte");
  }
}
void static_and_negative_tests() {
  auto s = stock();
  unsigned heap_refs = 0;
  for (unsigned off = 0; off + 4 <= s.size(); off += 4) {
    unsigned v = u32(s, off);
    if (v >= 0x20005310 && v <= 0x20005350) {
      check(off + 0x1000 == 0x1350 && v == 0x20005350,
            "retained tail aligned literal ownership");
      ++heap_refs;
    }
  }
  check(heap_refs == 1, "single heap end literal");
  for (std::string n : {"a", "b1", "c2"}) {
    auto b = read("outputs/frame-release-" + n + "/image.bin");
    check(u32(b, 0x1348 - 0x1000) == 0x20005158 &&
              u32(b, 0x134c - 0x1000) == 0x20005158,
          "profile/BSS reservation");
    check(u32(b, 0x1350 - 0x1000) == (n == "c2" ? 0x20005310u : 0x20005350u),
          "heap tail boundary");
    check(std::equal(b.begin(), b.begin() + 0x200, s.begin()),
          "full vector region unchanged");
  }
  auto rejected = [](const std::function<void()> &fn) {
    try {
      fn();
      return false;
    } catch (const std::exception &) {
      return true;
    }
  };
  check(rejected([] { branch(0x1000, 0x1001004, false); }),
        "positive branch overflow rejected");
  check(rejected([] { branch(0x2000000, 0, false); }),
        "negative branch overflow rejected");
  check(rejected([] { branch(0x1001, 0x2000, false); }),
        "misaligned source rejected");
  check(rejected([] { movw(16, 1); }), "invalid MOVW register rejected");
  check(rejected([] { movw(0, 65536); }), "invalid MOVW immediate rejected");
  check(movw(0, 9922) == hex("42f2c260") && movw(0, 10980) == hex("42f6e420"),
        "MOVW stock encoding vectors");
  check(branch(0x6c98, 0x57ed, true) == hex("fef7a8fd"),
        "BL stock encoding vector");
  // This fixture is the committed C candidate before the invalid-return fix.
  // Rejection is the regression's expected result; never a deployment image.
  auto old = read("outputs/regressions/c-disable-invalid.fixture");
  check(sha256(old) ==
            "b43616889910e0e5984898558a14c939a470b6a970706f74af63dd43fb5a7f15",
        "regression fixture hash");
  check(rejected([&] {
          Cpu c(old);
          c.store(0x200008c8, 0, 1);
          require(c.run(0x57ec, 4000) && c.r[0] == 0x101 &&
                      c.r[13] == 0x20005e00,
                  "invalid disable return regression");
        }),
        "old invalid disable regression detected");
  Cpu fixed(read("outputs/frame-release-c2/image.bin"));
  fixed.store(0x200008c8, 0, 1);
  check(fixed.run(0x57ec, 4000) && fixed.r[0] == 0x101 &&
            fixed.r[13] == 0x20005e00,
        "fixed invalid disable return");
}
void hook_equivalence_tests(const Bytes &image) {
  auto s = stock();
  for (unsigned site : {0x57ecu, 0x532cu, 0x8054u, 0x80a6u, 0x83eau, 0xc376u,
                        0x83fcu, 0xc388u, 0x951eu})
    for (unsigned pm : {0u, 1u})
      for (unsigned zero : {0u, 1u}) {
        Cpu c(image), base(s);
        for (Cpu *p : {&c, &base}) {
          seed(*p);
          p->primask = pm;
          p->r[0] = zero;
          p->r[3] = 3;
          p->r[4] = 0x20000fd0;
          p->r[6] = 0x20000fd0;
          p->r[10] = 0x20003000;
          p->store(0x20003001, 17, 1);
          if (site == 0x57ec)
            p->store(0x200008c8, zero ? 0 : 4, 1);
          if (site == 0x532c)
            p->stops.insert(0x5330);
          if (site == 0x8054)
            p->stops.insert(0x8058);
          if (site == 0x80a6) {
            p->r[2] = 1;
            p->r[3] = 0x40000000;
            p->stops.insert(0x80aa);
          }
          if (site == 0x83ea)
            p->stops.insert(0x83ee);
          if (site == 0xc376)
            p->stops.insert(0xc37a);
          if (site == 0x83fc)
            p->stops.insert(zero ? 0x8400 : 0x840a);
          if (site == 0xc388)
            p->stops.insert(zero ? 0xc38c : 0xc3c4);
          if (site == 0x951e)
            p->stops.insert(0x9522);
        }
        check(c.run(site) && base.run(site), "hook equivalence stop");
        for (unsigned i = 0; i <= 13; ++i)
          check(c.r[i] == base.r[i], "hook displaced register equivalence");
        check(c.apsr == base.apsr && c.primask == base.primask,
              "hook flags/PRIMASK equivalence");
        check(c.load(0x20000ffe, 1) == base.load(0x20000ffe, 1),
              "hook stock power marker equivalence");
      }
}
void frozen_record_tests(const Bytes &image) {
  Cpu c(image);
  seed(c);
  for (unsigned a = 0x20005300; a < 0x20005360; ++a)
    c.store(a, 0xa5, 1);
  c.store(0x20005310, 0x42435231, 4);
  c.store(0x20005314, 55, 4);
  c.store(0x20005318, 2u | 17u << 8 | 20u << 16 | 1u << 24, 4);
  c.store(0x2000531c, 0x1234, 4);
  c.store(0x20005320, 2, 4);
  c.mmio[0x40000400] = 0x10008;
  c.stops.insert(0x128e);
  check(c.run(0x126a, 200000), "snapshot application startup");
  check(c.load(0x20005328, 4) == 0x42435231 && c.load(0x2000532c, 4) == 55,
        "snapshot magic/sequence");
  check(c.load(0x20005330, 4) == (2u | 17u << 8 | 20u << 16 | 1u << 24) &&
            c.load(0x20005334, 4) == 0x10008 && c.load(0x20005338, 4) == 2,
        "snapshot previous stage and new reset reason");
  c.store(0x20005150, 1, 1);
  c.stops.clear();
  c.stops.insert(0x9522);
  c.r[10] = 0x20003000;
  c.store(0x20003001, 1, 1);
  check(c.run(0x951e), "first dispatcher after restart");
  check(c.load(0x2000531a, 1) == 80 && c.load(0x20005319, 1) == 1,
        "new traffic updates current record");
  Bytes frozen;
  for (unsigned i = 0; i < 20; ++i)
    frozen.push_back(uint8_t(c.load(0x20005328 + i, 1)));
  for (unsigned schema : {12u, 13u}) {
    c.stops.clear();
    c.r[0] = 0x20003000;
    c.r[1] = 18;
    c.r[14] = 0xdead0001;
    c.store(c.r[0], 0x7e, 1);
    c.store(c.r[0] + 1, schema, 1);
    check(c.run(0x8d16), "snapshot query returns");
    check(c.reply.at(22) == 1, "frozen source advertised");
    for (unsigned i = 0; i < 9; ++i)
      check(c.reply.at(23 + i) == frozen.at(i + (schema == 13 ? 9 : 0)),
            "query returns frozen evidence after new dispatcher traffic");
  }
  for (unsigned a = 0x20005300; a < 0x20005360; ++a)
    if (!(a >= 0x20005310 && a < 0x20005324) &&
        !(a >= 0x20005328 && a < 0x2000533c))
      check(c.load(a, 1) == 0xa5, "breadcrumb reservation red zones intact");
  for (unsigned start : {0x8406u, 0xc398u, 0x9a94u})
    for (unsigned event : {1u, 2u}) {
      Cpu now(image), base(stock());
      for (Cpu *p : {&now, &base}) {
        p->store(0x20005150, 1, 1);
        p->r[0] = event;
        p->stops.insert(0x9a98);
      }
      check(now.run(start) && base.run(start), "event post resumes stock body");
      for (unsigned i = 0; i < 15; ++i)
        check(now.r[i] == base.r[i],
              "event post registers and displaced prologue");
      check(now.apsr == base.apsr && now.primask == base.primask,
            "event post flags/PRIMASK");
      unsigned stage = start == 0x8406   ? 52
                       : start == 0xc398 ? 62
                       : event == 2      ? 70
                                         : 0,
               reason = start == 0x8406   ? 1
                        : start == 0xc398 ? 2
                        : event == 2      ? 3
                                          : 0;
      check(now.load(0x2000531a, 1) == stage &&
                now.load(0x20005320, 4) == reason,
            "actual event-2 origin/other classification");
    }
}
void diagnostic_reply_equivalence(const Bytes &image) {
  for (unsigned kind : {1u, 2u, 0x7du})
    for (unsigned mode : {0u, 1u})
      for (unsigned pm : {0u, 1u}) {
        Cpu now(image), baseline(read("inputs/diagnostic10.bin"));
        for (Cpu *p : {&now, &baseline}) {
          seed(*p);
          p->primask = pm;
          p->store(0x20005150, mode, 1);
          p->r[0] = 0x20003000;
          p->r[1] = kind == 0x7d ? 24 : 18;
          for (unsigned i = 0; i < 24; ++i)
            p->store(p->r[0] + i, i * 3 + 1, 1);
          p->store(p->r[0], kind == 0x7d ? 0x7d : 0x7e, 1);
          p->store(p->r[0] + 1, kind == 0x7d ? 2 : kind, 1);
          for (unsigned i = 0; i < 4; ++i) {
            p->store(0x20000efe + i, 0xa0 + i, 1);
            p->store(p->r[0] + 18 + i, 0xa0 + i, 1);
          }
          p->store(p->r[0] + 22, 1 - mode, 1);
          p->store(p->r[0] + 23, 3, 1);
        }
        check(now.run(0x8d16) && baseline.run(0x8d16),
              "diagnostic/release exact-code query or setter return");
        unsigned revision = kind == 0x7d ? 25 : 23;
        check(baseline.reply.at(revision) == (kind == 1 ? 1 : 7),
              "diagnostic original revision field");
        if (kind != 1)
          baseline.reply[revision] = 11;
        check(now.reply == baseline.reply, "reply differs only in revision");
        check(now.primask == baseline.primask && now.primask == pm,
              "diagnostic/release PRIMASK equivalence");
        for (unsigned i = 0; i < 3; ++i)
          check(now.load(0x20005150 + i, 1) == baseline.load(0x20005150 + i, 1),
                "diagnostic/release setter side effects equivalent");
      }
}
