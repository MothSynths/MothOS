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

#include "Samples/sfx1.h"
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
  for (int i = 0; i < 48; i++) {
    int valOut = (int)(250 * pow(((i + 12) / 12.0), 2));
    noteFreqLookup[i] = valOut;
  }

  octave = 1;
  sampleIndex = kick1Length * 1000;
  SetEnvelopeLength(1);

  volume = 2;
  ResetEffects();

  for (int i = 0; i <= 100; i++) {

    whooshSin[i] = (int)((cos((float)i / 50.0 * 3.14) + 1.0) * 5.0);

    int i2 = (200 - i * 2);
    int i3 = i * 2;
    if (i2 > 100)
      i2 = 100;
    if (i3 > 100)
      i3 = 100;


    envelopes[0][i] = i2;
    envelopes[1][i] = i3;
    envelopes[2][i] = 100;
    envelopes[3][i] = 100;
    if (i > 95) {
      //fade out envs
      envelopes[0][i] /= 1 + ((i - 95) * 20);
      envelopes[1][i] /= 1 + ((i - 95) * 20);
    }
  }
}


int Voice::UpdateVoice() {

  int sample = 0;
  if (voiceNum > 1 || samplerMode) {
    sample = ReadWaveform();
  } else if (voiceNum == 1) {
    sample = ReadSfxWaveform();
  } else {
    sample = ReadDrumWaveform();
  }

  if (phaserMult > 0) {
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

  if (delayMult > 0) {
    sample += GetHistorySample(delayMult * 11600 / bps) / 5;
  }

  if (whooshMult > 0) {

    whooshOffset += whooshMult * bps / 2;
    if (whooshOffset > 15000)
      whooshOffset = 0;
    int whoosh = whooshSin[whooshOffset / 150] + 1;
    for (int i = 0; i < whoosh; i++) {
      sample += GetHistorySample(i + 1);
    }
    sample /= whoosh + 1;
  }

  if (lowPassMult > 0) {
    for (int i = 1; i < 4 * lowPassMult; i++) {
      sample += GetHistorySample(i);
    }
    sample /= 4 * lowPassMult;
  }

  UpdateHistory(sample);

  if (reverbMult > 0) {
    int rSample = 0;
    for (int i = 2; i < 7; i++) {
      rSample += GetHistorySample(i * 450 * reverbMult);
    }
    sample = rSample / 2;
  }
  if (soloMute || mute) {
    sample = 0;
  }

  if (overdrive) {
    sample = sample * 10 / 7;
    int limit = 5000;
    if (sample > limit) {
      sample = limit;
    } else if (sample < -limit) {
      sample = -limit;
    }
  }
  return sample;
}

//look up how to refactor this in c++, use a list or dict not a switch
int Voice::ReadWaveform() {
  int sampleLen = 0;
  int sampleNext = 0;
  int sampleIndexReduced = sampleIndex / 1000;
  int vSel = voiceNum;
  int baseFreqLocal = baseFreq;

  if (samplerMode) {
    vSel = note;
    int oct = octave + 1;
    if (recOctave > -1)
      oct = recOctave + 1;

    baseFreqLocal = oct * 500;
    if (vSel < 2) {
      return 0;
    }
  }

  switch (vSel) {
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

  if (pitchMult > 0) {
    if (pitchDur > 0) {
      pitchDur -= pitchMult;
      if (pitchMult == 1)
        baseFreqLocal -= pitchDur / 5;
      if (pitchMult == 2)
        baseFreqLocal += pitchDur / 5;
    }
  }

  if (chordMult > 0) {
    chordStep++;
    int step = 1500;
    if (chordMult == 1) {
      if (chordStep < step) {

      } else if (chordStep < step * 2) {
        baseFreqLocal = baseFreq_ch1;
      } else if (chordStep < step * 3) {
        baseFreqLocal = baseFreq_ch2;
      } else {
        chordStep = 0;
      }
    }

    if (chordMult == 2) {
      if (chordStep < step) {

      } else if (chordStep < step * 2) {
        baseFreqLocal = baseFreq_ch3;
      } else if (chordStep < step * 3) {
        baseFreqLocal = baseFreq_ch4;
      } else {
        chordStep = 0;
      }
    }
  }

  sampleIndex += baseFreqLocal;

  if (sampleIndex >= sampleLen * 1000) {

    if (envelopeNum == 2) {
      sampleIndex = (sampleLen - 1) * 1000;
    } else {
      sampleIndex = sampleIndex - sampleLen * 1000;
    }
  }

  envelopeIndex += envelopeLength;
  if (envelopeIndex > 50000) {
    envelopeIndex = 50000;
    if (envelopeNum == 3)
      envelopeIndex = 1;
  }

  sample = (sample * volume * envelopes[envelopeNum][envelopeIndex / 500]) / 300;  //300 because of 3 volume levels

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

    sampleIndex += oct * 500;
    if (pitchMult > 0) {
      if (pitchDur > 0) {
        pitchDur -= pitchMult;
        if (pitchMult == 1)
          sampleIndex -= pitchDur / 5;
        if (pitchMult == 2)
          sampleIndex += pitchDur / 5;
      }
    }
  }
  sample = (sample * volume / 3);
  if (isDelay) {
    sample /= 3;
  }
  return sample;
}



int Voice::ReadSfxWaveform() {

  subSampleIndex = sampleIndex / 1000;
  if (envelopeNum > 1) {
    subSampleIndex = sampleLen - sampleIndex / 1000 - 1;
  }

  switch (note) {
    case 0:
      sampleLen = sfx1Length;
      sample = sfx1[subSampleIndex];
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

    sampleIndex += oct * 500;

    if (pitchMult > 0) {
      if (pitchDur > 0) {
        pitchDur -= pitchMult;
        if (pitchMult == 1)
          sampleIndex -= pitchDur / 5;
        if (pitchMult == 2)
          sampleIndex += pitchDur / 5;
      }
    }
  }
  sample = (sample * volume / 3);
  if (isDelay) {
    sample /= 3;
  }
  return sample;
}

int Voice::GetBaseFreq(int val, int ioctave) {
  if (val > 11) {
    val -= 12;
    ioctave++;
  }
  if (val < 0) {
    val += 12;
    ioctave--;
  }
  int valOut = noteFreqLookup[val + (ioctave * 12)];
  return valOut;
}

void Voice::SetNote(int val, bool delay, int optOctave, int optInstrument) {

  sampleIndex = 0;
  pitchDur = 7000;
  envelopeIndex = 0;
  note = val;

  voiceNum = optInstrument;
  recOctave = optOctave;
  if (optOctave == -1) {
    baseFreq = GetBaseFreq(val, octave);
    baseFreq_ch1 = GetBaseFreq(val - 4, octave);
    baseFreq_ch2 = GetBaseFreq(val + 3, octave);
    baseFreq_ch3 = GetBaseFreq(val - 5, octave);
    baseFreq_ch4 = GetBaseFreq(val + 7, octave);
  } else {
    baseFreq = GetBaseFreq(val, optOctave);
    baseFreq_ch1 = GetBaseFreq(val - 4, optOctave);
    baseFreq_ch2 = GetBaseFreq(val + 3, optOctave);
    baseFreq_ch3 = GetBaseFreq(val - 5, optOctave);
    baseFreq_ch4 = GetBaseFreq(val + 7, optOctave);
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
      mute = !mute;
      break;
    case 1:
      if (volume == 2) {
        volume = 3;
      } else {
        volume = 2;
      }
      break;
    case 2:
      overdrive = !overdrive;
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
  envelopeLength = 4 - val;
}

void Voice::SetEffectNum(int val) {

  if (val == 0) {
    ResetEffects();
  }

  if (val == 1) {
    lowPassMult++;
    if (lowPassMult > 2) {
      lowPassMult = 0;
    }
  }

  if (val == 2) {
    reverbMult++;
    if (reverbMult > 2) {
      reverbMult = 0;
    }
  }

  if (val == 3) {
    phaserMult++;
    if (phaserMult > 2) {
      phaserMult = 0;
    }
  }
  if (val == 4) {
    delayMult++;
    if (delayMult > 2) {
      delayMult = 0;
    }
  }

  if (val == 5) {
    chordMult++;
    if (chordMult > 2) {
      chordMult = 0;
    }
  }

  if (val == 6) {
    whooshMult++;
    if (whooshMult > 2) {
      whooshMult = 0;
    }
  }

  if (val == 7) {
    pitchMult++;
    if (pitchMult > 2) {
      pitchMult = 0;
    }
  }
}

void Voice::ResetEffects() {
  samplerMode = false;
  phaserMult = 0;
  delayMult = 0;
  reverbMult = 0;
  lowPassMult = 0;
  chordMult = 0;
  whooshMult = 0;
  pitchMult = 0;
}

void Voice::UpdateHistory(int sample) {
  sampleHistory[sampleHistoryIndex / 2] = sample;
  sampleHistoryIndex++;
  if (sampleHistoryIndex > 24000 - 2) {
    sampleHistoryIndex = 0;
  }
}

int Voice::GetHistorySample(int backoffset) {
  int ind = sampleHistoryIndex - backoffset;

  if (ind < 0) {
    ind = 24000 + ind;
  }
  return sampleHistory[ind / 2];
}
