#ifndef Voice_h
#define Voice_h
class Voice {
public:
  bool samplerMode;
  int bps;
  int arpNum;
  int delay;
  int octave;
  bool overdrive;
  int recOctave;
  Voice();
  int UpdateVoice();
  int phaserMult;
  int lowPassMult;
  int reverbMult;
  int chordMult;
  int pitchMult;
  int delayMult;
  int whooshMult;
  void SetNote(int val, bool delay, int optOctave, int optInstrument);
  void SetVolume(int val);
  void SetOctave(int val);
  void SetDelay(int val);
  void SetEnvelopeNum(int val);
  void SetEnvelopeLength(int val);
  void SetEffectNum(int val);
  void ResetEffects();
  int volume;
  bool soloMute;
  bool mute;

private:
  int noteFreqLookup[48];
  int pitchDur;
  int chordStep;
  int whooshOffset;
  int whooshSin[101];
  int oldArpNum;
  int envelopes[4][101];
  int envelopeIndex;
  int sample;
  int effect;
  int sampleLen;
  int phaserOffset;
  int phaserDir;
  bool isDelay;
  int envelopeLength;
  int envelope;
  int envelopeNum;
  int voiceNum;
  int output;
  int note;
  int sampleHistory[12001];
  int sampleHistoryIndex = 0;
  int lastSample;
  int baseFreq;
  int baseFreq_ch1;
  int baseFreq_ch2;
  int baseFreq_ch3;
  int baseFreq_ch4;
  long sampleIndex;
  int sampleIndexNext;
  int subSampleIndex;
  int volumeNum;
  int ReadWaveform();
  int ReadDrumWaveform();
  int ReadSfxWaveform();
  int GetBaseFreq(int val, int ioctave);
  float GetVolumeRatio();
  void UpdateHistory(int);
  int GetHistorySample(int backOffset);
};
#endif