#ifndef Voice_h
#define Voice_h
class Voice {
public:

  int arpNum;
  int delay;
  int octave;
  int recOctave;
  Voice();
  int UpdateVoice();

  void SetNote(int val, bool delay, int optOctave, int optInstrument);
  void SetVolume(int val);
  void SetOctave(int val);
  void SetDelay(int val);
  void SetEnvelopeNum(int val);
  void SetEnvelopeLength(int val);
  void SetArpNum(int val);

private:
  int phaserMult;
  int lowPassMult;
  int reverbMult;
  int oldArpNum;
  int envelopes[4][101];
  int envelopeIndex;
  int sample;
  int effect;
  int sampleLen;
  int phaserOffset;
  int phaserDir;
  bool isDelay;
  int arpCount;
  int arps[4][4];
  int envelopeLength;
  int envelope;
  int envelopeNum;
  int voiceNum;
  int output;
  int note;
  int sampleHistory[12000];
  int sampleHistoryIndex = 0;
  int lastSample;
  int baseFreq;
  long sampleIndex;
  int sampleIndexNext;
  int subSampleIndex;
  int volume;
  int volumeNum;
  int ReadWaveform();
  int ReadDrumWaveform();
  int ReadSfxWaveform();
  int GetBaseFreq(int val, int ioctave);
  float GetVolumeRatio();
  void SetupArps();
  float LerpSample(int sampleA, int sampleB, float ratio);
  void UpdateHistory(int);
  int GetHistorySample(int backOffset);
};
#endif