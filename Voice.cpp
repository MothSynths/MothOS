#include <Arduino.h>
#include "Voice.h"
//drums
#include "Samples/kick1.h"
#include "Samples/kick2.h"
#include "Samples/kick3.h"
#include "Samples/hihat1.h"
#include "Samples/hihat2.h"
#include "Samples/hihat3.h"
#include "Samples/snare1.h"
#include "Samples/snare2.h"
#include "Samples/snare3.h"
#include "Samples/special1.h"
#include "Samples/special2.h"
#include "Samples/special3.h"
//sfx


#include "Samples/sfx2.h"
#include "Samples/sfx3.h"
#include "Samples/sfx4.h"
#include "Samples/sfx5.h"
#include "Samples/sfx6.h"
#include "Samples/sfx7.h"
#include "Samples/sfx8.h"
#include "Samples/sfx9.h"
#include "Samples/sfx10.h"
#include "Samples/sfx11.h"
#include "Samples/sfx12.h"

//instruments

#include "Samples/instrument1.h"
#include "Samples/instrument2.h"
#include "Samples/instrument3.h"
#include "Samples/instrument4.h"
#include "Samples/instrument5.h"
#include "Samples/instrument6.h"
#include "Samples/instrument7.h"
#include "Samples/instrument8.h"
#include "Samples/instrument9.h"
#include "Samples/instrument10.h"


Voice::Voice() {
  phaserMult = 1;
  reverbMult = 1;
  lowPassMult = 1;
  octave = 1;
  sampleIndex = kick1Length * 1000;
  envelopeLength = 2;

  SetVolume(2);
  SetupArps();
  for (int i = 0; i <= 100; i++) {
    int i2 = i * 2 - 100;
    int i3 = i * 2;
    if (i3 > 100)
      i3 = 100;
    if (i2 < 0)
      i2 = 0;

    envelopes[0][i] = (sqrt(sqrt(i2) * 5) / 3) + 1;
    envelopes[1][i] = (sqrt(sqrt(100 - i3) * 5) / 3) + 1;
    envelopes[2][i] = 1;
    envelopes[3][i] = 1;
    if (i == 100) {
      envelopes[0][i] = 10000;
      envelopes[1][i] = 10000;
      envelopes[2][i] = 10000;
    }
  }
}



int Voice::UpdateVoice() {

  int sample = 0;
  if (voiceNum > 1) {
    sample = ReadWaveform();
  } else if (voiceNum == 1) {
    sample = ReadSfxWaveform();
  } else {
    sample = ReadDrumWaveform();
  }

  if (arpNum == 1) {
    UpdateHistory(sample);
    sample = 0;
    for (int i = 0; i < 3 * lowPassMult; i++) {
      sample += GetHistorySample(i);
    }
    sample /= 3 * lowPassMult;
  }

  if (arpNum == 2) {
    UpdateHistory(sample);
    int rSample = 0;
    for (int i = 1; i < 6; i++) {
      rSample += GetHistorySample(i * 350 * reverbMult);
    }

    sample = rSample / 2;
  }

  if (arpNum == 3) {
    UpdateHistory(sample);
    if (phaserDir >= 0) {
      phaserOffset++;
      if (phaserOffset > 2000 * phaserMult) {
        phaserDir = -1;
      }
    } else {
      phaserOffset--;
      if (phaserOffset < 0) {
        phaserDir = 1;
      }
    }
    int rSample = GetHistorySample(phaserOffset);
    sample = (sample + rSample) / 2;
  }

  if (volumeNum == 3) {
    int limit = 6000;
    if (abs(sample) > 6000) {
      if (sample > 0) {
        sample = limit;
      } else {
        sample = limit * -1;
      }
      sample = (sample * 2);
    }
  }

  return sample;
}

//look up how to refactor this in c++, use a list or dict not a switch
int Voice::ReadWaveform() {
  int sampleLen = 0;
  int sampleNext = 0;

  int sampleIndexReduced = sampleIndex / 1000;
  switch (voiceNum) {
    case 2:
      sampleLen = instrument1Length;
      sample = instrument1[sampleIndexReduced];
      break;
    case 3:
      sampleLen = instrument2Length;
      sample = instrument2[sampleIndexReduced];
      break;
    case 4:
      sampleLen = instrument3Length;
      sample = instrument3[sampleIndexReduced];
      break;
    case 5:
      sampleLen = instrument4Length;
      sample = instrument4[sampleIndexReduced];
      break;
    case 6:
      sampleLen = instrument5Length;
      sample = instrument5[sampleIndexReduced];
      break;
    case 7:
      sampleLen = instrument6Length;
      sample = instrument6[sampleIndexReduced];
      break;
    case 8:
      sampleLen = instrument7Length;
      sample = instrument7[sampleIndexReduced];
      break;
    case 9:
      sampleLen = instrument8Length;
      sample = instrument8[sampleIndexReduced];
      break;
    case 10:
      sampleLen = instrument9Length;
      sample = instrument9[sampleIndexReduced];
      break;
    case 11:
      sampleLen = instrument10Length;
      sample = instrument10[sampleIndexReduced];
      break;
  }


  sampleIndex += baseFreq;

  if (sampleIndex >= sampleLen * 1000) {
    sampleIndex = sampleIndex - sampleLen * 1000;
  }

  envelopeIndex += envelopeLength;
  if (envelopeIndex > 25000) {
    envelopeIndex = 25000;
    if (envelopeNum == 3)
      envelopeIndex = 1;
  }

  sample = sample / volume / envelopes[envelopeNum][envelopeIndex / 250];

  if (isDelay) {
    sample /= 3;
  }
  return sample;
}

int Voice::ReadDrumWaveform() {


  subSampleIndex = sampleIndex / 1000;
  if (envelopeNum > 1) {
    subSampleIndex = sampleLen - sampleIndex / 1000 - 1;
  }

  switch (note) {
    case 0:
      sampleLen = kick1Length;
      sample = kick1[subSampleIndex];
      break;
    case 1:
      sampleLen = snare1Length;
      sample = snare1[subSampleIndex];
      break;
    case 2:
      sampleLen = special1Length;
      sample = special1[subSampleIndex];
      break;
    case 3:
      sampleLen = hihat1Length;
      sample = hihat1[subSampleIndex];
      break;
    case 4:
      sampleLen = kick2Length;
      sample = kick2[subSampleIndex];
      break;
    case 5:
      sampleLen = snare2Length;
      sample = snare2[subSampleIndex];
      break;
    case 6:
      sampleLen = special2Length;
      sample = special2[subSampleIndex];
      break;
    case 7:
      sampleLen = hihat2Length;
      sample = hihat2[subSampleIndex];
      break;
    case 8:
      sampleLen = kick3Length;
      sample = kick3[subSampleIndex];
      break;
    case 9:
      sampleLen = snare3Length;
      sample = snare3[subSampleIndex];
      break;
    case 10:
      sampleLen = special3Length;
      sample = special3[subSampleIndex];
      break;
    case 11:
      sampleLen = hihat3Length;
      sample = hihat3[subSampleIndex];
      break;
  }

  if (sampleIndex >= sampleLen * 1000) {
    sample = 0;
  } else {
    int oct = octave + 1;
    if (recOctave > -1)
      oct = recOctave + 1;

    sampleIndex += oct * 1000;
  }
  sample = (int)(sample / volume);
  if (isDelay) {
    sample /= 3;
  }
  return sample * 2;
}

int Voice::ReadSfxWaveform() {

  subSampleIndex = sampleIndex / 1000;
  if (envelopeNum > 1) {
    subSampleIndex = sampleLen - sampleIndex / 1000 - 1;
  }

  switch (note) {
    case 0:
      sampleLen = sfx2Length;
      sample = sfx2[subSampleIndex];
      break;
    case 1:
      sampleLen = sfx2Length;
      sample = sfx2[subSampleIndex];
      break;
    case 2:
      sampleLen = sfx3Length;
      sample = sfx3[subSampleIndex];
      break;
    case 3:
      sampleLen = sfx4Length;
      sample = sfx4[subSampleIndex];
      break;
    case 4:
      sampleLen = sfx5Length;
      sample = sfx5[subSampleIndex];
      break;
    case 5:
      sampleLen = sfx6Length;
      sample = sfx6[subSampleIndex];
      break;
    case 6:
      sampleLen = sfx7Length;
      sample = sfx7[subSampleIndex];
      break;
    case 7:
      sampleLen = sfx8Length;
      sample = sfx8[subSampleIndex];
      break;
    case 8:
      sampleLen = sfx9Length;
      sample = sfx9[subSampleIndex];
      break;
    case 9:
      sampleLen = sfx10Length;
      sample = sfx10[subSampleIndex];
      break;
    case 10:
      sampleLen = sfx11Length;
      sample = sfx11[subSampleIndex];
      break;
    case 11:
      sampleLen = sfx12Length;
      sample = sfx12[subSampleIndex];
      break;
  }
  if (sampleIndex >= sampleLen * 1000) {
    sample = 0;
  } else {
    int oct = octave + 1;
    if (recOctave > -1)
      oct = recOctave + 1;

    sampleIndex += oct * 1000;
  }
  sample = (sample / volume);
  if (isDelay) {
    sample /= 3;
  }
  return sample;
}

float Voice::LerpSample(int sampleA, int sampleB, float ratio) {
  return (float)sampleA + ((float)(sampleB - sampleA) * ratio);
}

int Voice::GetBaseFreq(int val, int ioctave) {
  int freqAdd = 2;
  //if (arpNum > 0) {
  // val += arps[arpNum][arpCount];
  //}
  int valOut = 1000 + ((val + ((ioctave)*12)) * freqAdd * freqAdd * 23);  // + (octave * freqAdd * 12);

  return valOut;
}

void Voice::SetNote(int val, bool delay, int optOctave, int optInstrument) {

  sampleIndex = 0;

  envelopeIndex = 0;
  note = val;

  voiceNum = optInstrument;
  recOctave = optOctave;
  if (optOctave == -1)
    baseFreq = GetBaseFreq(val, octave);
  else
    baseFreq = GetBaseFreq(val, optOctave);

  arpCount++;
  if (arpCount > 3) {
    arpCount = 0;
  }
  isDelay = delay;
}

void Voice::SetDelay(int val) {
  delay = val;
}

void Voice::SetVolume(int val) {
  volumeNum = val;
  switch (val) {
    case 0:
      volume = 4;
      break;
    case 1:
      volume = 2;
      break;
    case 2:
      volume = 1;
      break;
    case 3:
      volume = 1;
      break;
  }
}

void Voice::SetOctave(int val) {
  octave = val;
}

void Voice::SetEnvelopeNum(int val) {
  envelopeNum = val;
}

void Voice::SetEnvelopeLength(int val) {
  envelopeLength = val + 1;
}

void Voice::SetArpNum(int val) {
  if (oldArpNum != val) {
    phaserMult = 1;
    reverbMult = 1;
    lowPassMult = 1;
  } else {
    if (val == 1) {
      lowPassMult++;
      if (lowPassMult > 3) lowPassMult = 1;
    }
    if (val == 2) {
      reverbMult++;
      if (reverbMult > 3) reverbMult = 1;
    }
    if (val == 3) {
      phaserMult++;
      if (phaserMult > 3) phaserMult = 1;
    }
  }
  arpNum = val;
  oldArpNum = val;
}

void Voice::SetupArps() {
  arps[1][0] = -5;
  arps[1][1] = -3;
  arps[1][2] = 0;
  arps[1][3] = 3;

  arps[2][0] = 0;
  arps[2][1] = 7;
  arps[2][2] = 12;
  arps[2][3] = 5;

  arps[3][0] = 0;
  arps[3][1] = 5;
  arps[3][2] = 7;
  arps[3][3] = 12;
}

void Voice::UpdateHistory(int sample) {
  sampleHistory[sampleHistoryIndex] = sample;
  sampleHistoryIndex++;
  if (sampleHistoryIndex > 11999) {
    sampleHistoryIndex = 0;
  }
}

int Voice::GetHistorySample(int backoffset) {
  int ind = sampleHistoryIndex - backoffset;

  if (ind < 0) {
    ind = 11999 + ind;
  }
  return sampleHistory[ind];
}
