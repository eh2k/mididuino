#include <MD.h>
#include <MDRandomizer.h>

MDRandomizerClass MDRandomizer;

class RandomizePage : 
public EncoderPage {
public: 
  MDTrackFlashEncoder trackEncoder;
  RangeEncoder amtEncoder;
  EnumEncoder selectEncoder;

  RandomizePage() :
  trackEncoder("TRK"),
  amtEncoder(0, 128, "AMT"),
  selectEncoder(MDRandomizerClass::selectNames, countof(MDRandomizerClass::selectNames), "SEL") {
    encoders[0] = &trackEncoder;
    encoders[1] = &amtEncoder;
    encoders[2] = &selectEncoder;

    MDRandomizer.setTrack(trackEncoder.getValue());
  }

  virtual void loop() {
    if (trackEncoder.hasChanged()) {
      MDRandomizer.setTrack(trackEncoder.getValue());
    }
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      MDRandomizer.randomize(amtEncoder.getValue(), selectEncoder.getValue());
      return true;
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      GUI.setLine(GUI.LINE1);
      if (MDRandomizer.undo()) {
        GUI.flash_p_string_fill(PSTR("UNDO"));
      } 
      else {
        GUI.flash_p_string_fill(PSTR("UNDO XXX"));
      }
      return true;
    }

    return false;
  }
};

RandomizePage randomizePage;

class MDRandomizeSketch : 
public Sketch {
public:

  void setup() {
    MDTask.setup();
    MDTask.autoLoadKit = true;
    MDTask.reloadGlobal = true;
    MDTask.addOnKitChangeCallback(_onKitChanged);
    MDTask.checkSettingsFlags = MDTask.MD_CHECK_SETTINGS_PARAMS;
    MDTask.addOnGlobalChangeCallback(_onGlobalChanged);
    GUI.addTask(&MDTask);

    setPage(&randomizePage);
  }
};

MDRandomizeSketch sketch;

void setup() {
  GUI.flash_p_string_clear(PSTR("MD RANDOMIZE"));
  sketch.setup();
  GUI.setSketch(&sketch);
  Midi.setOnControlChangeCallback(_onCCCallback);
}

void loop() {
}

void _onCCCallback(uint8_t *msg) {
  MDRandomizer.onCCCallback(msg);
}

void _onKitChanged() {
  MDRandomizer.onKitChanged();
  GUI.setLine(GUI.LINE1);
  GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
  GUI.setLine(GUI.LINE2);
  GUI.flash_string_fill(MD.kit.name);
}

void _onGlobalChanged() {
  MDTask.reloadGlobal = true;
}
