#ifndef MNM_H__
#define MNM_H__

#include <inttypes.h>

#include "WProgram.h"
#include "MNMMessages.hh"
#include "MNMPattern.hh"
#include "MNMParams.hh"

#ifndef HOST_MIDIDUINO
class MNMEncoder : public CCEncoder {
 public:
  uint8_t track;
  uint8_t param;

  virtual uint8_t getCC();
  virtual uint8_t getChannel();
  virtual void initCCEncoder(uint8_t _channel, uint8_t _cc);
  void initMNMEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0) {
    track = _track;
    param = _param;
    setName(_name);
    setValue(init);
  }

  MNMEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0);
  void loadFromKit();
};
#endif

class MNMClass {
 public:
  MNMClass();

  uint8_t currentTrack;
  
  int currentGlobal;
  bool loadedGlobal;
  MNMGlobal global;
  
  int currentKit;
  bool loadedKit;
  MNMKit kit;

  int currentPattern;

#ifndef HOST_MIDIDUINO
  void sendMultiTrigNoteOn(uint8_t note, uint8_t velocity);
  void sendMultiTrigNoteOff(uint8_t note);
  void sendMultiMapNoteOn(uint8_t note, uint8_t velocity);
  void sendMultiMapNoteOff(uint8_t note);
  void sendAutoNoteOn(uint8_t note, uint8_t velocity);
  void sendAutoNoteOff(uint8_t note);
  void sendNoteOn(uint8_t note, uint8_t velocity) {
    sendNoteOn(currentTrack, note, velocity);
  }
  void sendNoteOn(uint8_t track, uint8_t note, uint8_t velocity);
  void sendNoteOff(uint8_t note) {
    sendNoteOff(currentTrack, note);
  }
  void sendNoteOff(uint8_t track, uint8_t note);

  void setParam(uint8_t param, uint8_t value) {
    setParam(currentTrack, param, value);
  }
  void setParam(uint8_t track, uint8_t param, uint8_t value);
  void setAutoParam(uint8_t param, uint8_t value);
  void setAutoLevel(uint8_t level);

  void setMultiEnvParam(uint8_t param, uint8_t value);
  void setMidiParam(uint8_t param, uint8_t value) {
    setMidiParam(currentTrack, param, value);
  }
  void setMidiParam(uint8_t track, uint8_t param, uint8_t value);
  void setTrackPitch(uint8_t pitch) {
    setTrackPitch(currentTrack, pitch);
  }
  void setTrackPitch(uint8_t track, uint8_t pitch);

  void setTrackLevel(uint8_t level) {
    setTrackLevel(currentTrack);
  }
  void setLevel(uint8_t track, uint8_t level);

  void triggerTrack(bool amp = false, bool lfo = false, bool filter = false) {
    triggerTrack(currentTrack, amp, lfo, filter);
  }
  void triggerTrackAmp() {
    triggerTrackAmp(currentTrack);
  }
  void triggerTrackAmp(uint8_t track) {
    triggerTrack(track, true, false, false);
  }
  void triggerTrackLFO() {
    triggerTrackLFO(currentTrack);
  }
  void triggerTrackLFO(uint8_t track) {
    triggerTrack(track, false, true, false);
  }
  void triggerTrackFilter() {
    triggerTrackFilter(currentTrack);
  }
  void triggerTrackFilter(uint8_t track) {
    triggerTrack(track, false, false, true);
  }
  void triggerTrack(uint8_t track, bool amp = false, bool lfo = false, bool filter = false);
  
  bool parseCC(uint8_t channel, uint8_t cc, uint8_t *track, uint8_t *param);

  void setStatus(uint8_t id, uint8_t value);

  void loadGlobal(uint8_t id);
  void loadKit(uint8_t id);
  void loadPattern(uint8_t id);
  void loadSong(uint8_t id);

  void setSequencerMode(bool songMode);
  void setAudioMode(bool polyMode);
  void setSequencerModeMode(bool midiMode);
  void setAudioTrack(uint8_t track);
  void setMidiTrack(uint8_t track);

  void setCurrentKitName(char *name);
  void saveCurrentKit(uint8_t id);

  void sendRequest(uint8_t type, uint8_t param);
  void requestKit(uint8_t kit);
  void requestPattern(uint8_t pattern);
  void requestSong(uint8_t song);
  void requestGlobal(uint8_t global);

  void assignMachine(uint8_t model, bool initAll = false, bool initSynth = false) {
    assignMachine(currentTrack, model, initAll, initSynth);
  }
  void assignMachine(uint8_t track, uint8_t model, bool initAll = false, bool initSynth = false);
  void setMachine(MNMMachine *machine) {
    setMachine(currentTrack, machine);
  }
  void setMachine(uint8_t track, MNMMachine *machine);

  void setMute(bool mute) {
    setMute(currentTrack, mute);
  }
  void setMute(uint8_t track, bool mute);
  void muteTrack() {
    muteTrack(currentTrack);
  }
  void muteTrack(uint8_t track) {
    setMute(track, true);
  }
  void unmuteTrack() {
    unmuteTrack(currentTrack);
  }
  void unmuteTrack(uint8_t track) {
    setMute(track, false);
  }
  void setAutoMute(bool mute);
  void muteAutoTrack() {
    setAutoMute(true);
  }
  void unmuteAutoTrack() {
    setAutoMute(false);
  }
#endif
  
  PGM_P getMachineName(uint8_t machine);
  PGM_P getModelParamName(uint8_t model, uint8_t param);
  void getPatternName(uint8_t pattern, char str[5]);
};

extern MNMClass MNM;

#include "MNMSysex.hh"

#ifndef HOST_MIDIDUINO
#include "MNMTask.hh"
#endif

#endif /* MNM_H__ */