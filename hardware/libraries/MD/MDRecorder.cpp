#include <inttypes.h>
#include "WProgram.h"
#include "helpers.h"
#include "MDRecorder.h"

void _MDRecorder_onNoteOnCallback(uint8_t *msg);
void _MDRecorder_onCCCallback(uint8_t *msg);
void _MDRecorder_on16Callback();

MDRecorderClass::MDRecorderClass() {
  recording = false;
  playing = false;
  recordLength = 0;
  playPtr = NULL;
  looping = true;
  md_playback_phase = MD_PLAYBACK_NONE;
}

void MDRecorderClass::setup() {
  MidiClock.addOn16Callback(_MDRecorder_on16Callback);
}

void MDRecorderClass::startRecord(uint8_t length, uint8_t boundary) {
  if (playing) {
    stopPlayback();
  }
  
  USE_LOCK();
  SET_LOCK();

  eventList.freeAll();
  
  //  start16th = MidiClock.div16th_counter;
  rec16th_counter = 0;
  if (boundary != 0) {
    recordingBoundary = boundary;
    recordingTriggered = true;
    recording = false;
  } else {
    recording = true;
  }
  recordLength = length;
  
  MidiUart.addOnNoteOnCallback(_MDRecorder_onNoteOnCallback);
  MidiUart.addOnControlChangeCallback(_MDRecorder_onCCCallback);
  CLEAR_LOCK();
}

void MDRecorderClass::stopRecord() {
  USE_LOCK();
  SET_LOCK();
  recording = false;
  MidiUart.removeOnNoteOnCallback(_MDRecorder_onNoteOnCallback);
  MidiUart.removeOnControlChangeCallback(_MDRecorder_onCCCallback);
  CLEAR_LOCK();
  eventList.reverse();
}

void MDRecorderClass::startMDPlayback(uint8_t boundary) {
  md_playback_phase = MD_PLAYBACK_HITS;
  startPlayback(boundary);
}

void MDRecorderClass::startPlayback(uint8_t boundary) {
  if (recording) {
    stopRecord();
  }
  
  USE_LOCK();
  SET_LOCK();

  play16th_counter = 0;
  if (boundary != 0) {
    playbackBoundary = boundary;
    playbackTriggered = true;
    playing = false;
  } else {
    playing = true;
  }
  playPtr = eventList.head;

  CLEAR_LOCK();
}

void MDRecorderClass::stopPlayback() {
  USE_LOCK();
  SET_LOCK();

  playing = false;

  CLEAR_LOCK();
}

void MDRecorderClass::onNoteOnCallback(uint8_t *msg) {
  USE_LOCK();
  SET_LOCK();
  uint8_t pos = rec16th_counter;
  CLEAR_LOCK();
  
  ListElt<md_recorder_event_t> *elt = eventList.pool.alloc();
  if (elt != NULL) {
    elt->obj.channel = msg[0] & 0xF;
    elt->obj.pitch = msg[1];
    elt->obj.value = msg[2];
    elt->obj.step = pos;
    eventList.push(elt);
    //    GUI.setLine(GUI.LINE2);
    //    GUI.put_value(1, pos);
  }
}

void MDRecorderClass::onCCCallback(uint8_t *msg) {
  USE_LOCK();
  SET_LOCK();
  uint8_t pos = rec16th_counter;
  CLEAR_LOCK();
  
  ListElt<md_recorder_event_t> *elt = eventList.pool.alloc();
  if (elt != NULL) {
    elt->obj.channel = (msg[0] & 0xF) | 0x80;
    elt->obj.pitch = msg[1];
    elt->obj.value = msg[2];
    elt->obj.step = pos;
    eventList.push(elt);
  }
}

void MDRecorderClass::on16Callback() {
  USE_LOCK();
  SET_LOCK();

  if (recording) {
    if (++rec16th_counter >= recordLength) {
      stopRecord();
    }
  }
  
  if (recordingTriggered) {
    if ((MidiClock.div16th_counter % recordingBoundary) == 0) {
      recordingTriggered = false;
      recording = true;
    }
  }

  if (playbackTriggered) {
    if ((MidiClock.div16th_counter % playbackBoundary) == 0) {
      playbackTriggered = false;
      playing = true;
    }
  }

  if (playing) {
    while ((playPtr != NULL) && (playPtr->obj.step <= play16th_counter)) {
      if (playPtr->obj.channel & 0x80) {
	if (md_playback_phase == MD_PLAYBACK_NONE) {
	  MidiUart.sendCC(playPtr->obj.channel & 0xF, playPtr->obj.pitch, playPtr->obj.value);
	} else if (md_playback_phase == MD_PLAYBACK_CCS) {
	  MidiUart.sendCC(playPtr->obj.channel & 0xF, playPtr->obj.pitch, playPtr->obj.value - 5);
	  delayMicroseconds(100);
	  MidiUart.sendCC(playPtr->obj.channel & 0xF, playPtr->obj.pitch, playPtr->obj.value);
	}
      } else {
	if (md_playback_phase != MD_PLAYBACK_CCS) {
	  MidiUart.sendNoteOn(playPtr->obj.channel, playPtr->obj.pitch, playPtr->obj.value);
	}
      }
      playPtr = playPtr->next;
    }
    
    if (++play16th_counter >= recordLength) {
      if (md_playback_phase == MD_PLAYBACK_HITS) {
	md_playback_phase = MD_PLAYBACK_CCS;
	play16th_counter = 0;
	playing = true;
	playPtr = eventList.head;
	return;
      } else if (md_playback_phase == MD_PLAYBACK_CCS) {
	md_playback_phase = MD_PLAYBACK_NONE;
      }
	
      if (looping) {
	play16th_counter = 0;
	playing = true;
	playPtr = eventList.head;
      } else {
	stopPlayback();
      }
    }
  }
  
  CLEAR_LOCK();
}

MDRecorderClass MDRecorder;

void _MDRecorder_onNoteOnCallback(uint8_t *msg) {
  MDRecorder.onNoteOnCallback(msg);
}

void _MDRecorder_onCCCallback(uint8_t *msg) {
  MDRecorder.onCCCallback(msg);
}

void _MDRecorder_on16Callback() {
  MDRecorder.on16Callback();
}